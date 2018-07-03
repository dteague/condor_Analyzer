#include "Particles.C"
#include "Histogramer.C"
#include <TTreeReader.h>
#include <TChain.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;


class Analyzer {
public:
  Analyzer() : chain("Delphes"),  treeReader(new TTreeReader(&chain)), Jet(treeReader, "Jet"), Electron(treeReader, "Electron", 0), Muon_T(treeReader, "MuonTight", 0), Muon_L(treeReader, "MuonLoose", 0), MET(*treeReader, "MissingET.MET") {
    gErrorIgnoreLevel=kError; /// Ignores silly errors in ExRoot dictionaries    
    partList.push_back(&Jet);
    partList.push_back(&Electron);
    partList.push_back(&Muon_T);
    partList.push_back(&Muon_L);

    passCut.resize(E_LAST_CUT);
  }
  


  void write_out(string outfile) {
    cout << "Number passed: " << passed << endl;
    histo.write_out(outfile);
  }

  bool next();
  void selection();
  void fill_histo();
  void add_run_file(string filename) {
    chain.AddFile(filename.c_str());
    numberOfEntries = treeReader->GetEntries(1);
    cout << "Number of Events: " << numberOfEntries << endl;

  }
  inline int event_number() {return counter;}
  inline double event_percent() {return ((double)counter)/numberOfEntries;}
  
  ~Analyzer() {
    delete treeReader;
  }

  void set_events() {histo.setup_hevents(cutNames);}
  
private:
  TChain chain;
  TTreeReader *treeReader;
  Long64_t numberOfEntries;
  int counter = 0;
  int passed = 0;
  
  Part Jet;
  Part Electron;
  Part Muon_T;
  Part Muon_L;
  vector<Part*> partList;
  Tau Tau_h;
  TTreeReaderArray<float> MET;

  Histograms histo;

  void overlap(Part& Muon);
  bool pass_ncut(int);
  
  vector<bool> passCut;
  bool passedAllCuts;
  bool set_cut(int cut, bool pass, bool);

  enum CUT_NAMES { E_JET_N, E_MET, E_LEP, E_VBS, E_LAST_CUT };
  vector<string> cutNames = {"Jet Number", "Met", "Lepton", "VBS"};

};




bool Analyzer::next() {
  counter++;
  bool goNext = treeReader->Next();
  if(goNext) for(auto part : partList) part->setup_lvectors();

  return goNext;
}
    
void Analyzer::selection() {
  passedAllCuts = true;
  Muon_T.basic_cuts(15, 2.4);
  Electron.basic_cuts(15, 2.4);
  Jet.basic_cuts(15, 4.7);
  overlap(Muon_T);

  passedAllCuts = set_cut(E_JET_N, Jet.size() >= 2, passedAllCuts);
  passedAllCuts = set_cut(E_MET, MET.At(0) > 30, passedAllCuts);    /// MET CUT
  
  bool leptonCuts =  (Muon_T.size()+Electron.size() == 3);
  ZCandidate Z(Muon_T, Electron);
  leptonCuts = leptonCuts && (Z.size() != 0);
  TLorentzVector triLep;
  for(auto lep : Muon_T) triLep += lep;
  for(auto lep : Electron) triLep += lep;
  leptonCuts = leptonCuts && (triLep.M() > 100);
  leptonCuts = leptonCuts && (fabs( triLep.Eta() - 0.5*(Jet[1].Eta() + Jet[2].Eta())) < 2.5);
  passedAllCuts = set_cut(E_LEP, leptonCuts, passedAllCuts);

  /// vbs selection
  bool vbsSelection = (Jet[0].Pt() > 50 && Jet[1].Pt() > 50 ) && (fabs(Jet[0].Eta()-Jet[1].Eta()) > 2.5) && ((Jet[0]+Jet[1]).M() > 500);
  passedAllCuts = set_cut(E_VBS, vbsSelection, passedAllCuts);


  if (passedAllCuts) passed++;
}

bool Analyzer::set_cut(int cut, bool pass, bool currentTotalPass) {
  passCut[cut] = pass;
  return currentTotalPass && pass;
}


void Analyzer::fill_histo() {
  histo.fill_hevents(passCut, passedAllCuts);

  ///////////////////////////////
  /////// Pass all cuts ////////
  /////////////////////////////
  if( pass_ncut(E_LEP)) {
    histo.fill("AFTER_hvbsjetsDEta", Jet[0].Eta()-Jet[1].Eta());
    histo.fill("AFTER_hvbsjetsMass", (Jet[0]+Jet[1]).M());
  }

  if(pass_ncut(E_VBS)) {
    histo.fill("BEFORE_hvbsjetsDEta", Jet[0].Eta()-Jet[1].Eta());
    histo.fill("BEFORE_hvbsjetsMass", (Jet[0]+Jet[1]).M());

  }

}
    //////////////////////////////////////////////
    ///////////// FILL UP HISTO //////////////////
    //////////////////////////////////////////////
    // histo.fill("hleadjetPt", Jet[0].Pt());
    // histo.fill("hleadjetEta", Jet[0].Eta());
    // histo.fill("hlepN", Muon_T.size()+Electron.size());


    // for( auto jet : Jet) {

    //   histo.fill("hjetPt",jet.Pt());
    //   histo.fill("hjetEta",jet.Eta());
    // }
    // histo.fill("hjetN",Jet.size());
    // histo.fill("htauN",Tau.size());
    // histo.fill("hMet",MET.At(0));




bool Analyzer::pass_ncut(int cut) {
  for( int i = 0; i < E_LAST_CUT; i++) {
    if( i != cut && !passCut.at(i) ) {
      return false;
    }
  }
  return true;
}







void Analyzer::overlap(Part& Muon) {
  for( auto it = Jet.begin(); it != Jet.end(); it++) {
    bool failed = false;
    for( auto e : Electron) {
      if(it->DeltaR(e) < 0.05) {
	Jet.remove(it - Jet.begin());
	failed = true;
	break;
      }
    }
    if(failed) {
      it--;
      continue;
    }
    for( auto mu : Muon) {
      if(it->DeltaR(mu) < 0.05) {
	Jet.remove(it - Jet.begin());
	failed = true;
	break;
      }
    }
    if(failed) {
      it--;
      continue;
    }
    // if(Jet.is_tau(it - Jet.begin())) {
    //   Tau.add_tau(*it);
    // }
  }
  
}
