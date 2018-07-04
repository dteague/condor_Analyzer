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
  bool pass_ncut(int) const;
  
  vector<bool> passCut;
  void set_cut(int cut, bool pass) { passCut[cut] = pass;}


  bool pass_allCuts() const;
  
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

  Muon_T.basic_cuts(20, 2.5);
  Electron.basic_cuts(20, 2.5);
  Jet.basic_cuts(30, 4.7); 
  overlap(Muon_T);

  set_cut(E_JET_N, (Jet.size() >= 2));
  set_cut(E_MET, (MET.At(0) > 30.));    /// MET CUT
  if(!passCut.at(E_JET_N)) {
    set_cut(E_LEP, false);
    set_cut(E_VBS, false);
    return;
  }

  
  bool leptonCuts =  (Muon_T.size()+Electron.size() == 3);
  ZCandidate Z(Muon_T, Electron);
  leptonCuts = leptonCuts && (Z.size() != 0);
  TLorentzVector triLep;
  for(auto lep : Muon_T) triLep += lep;
  for(auto lep : Electron) triLep += lep;
  leptonCuts = leptonCuts && (triLep.M() > 100);
  leptonCuts = leptonCuts && (fabs( triLep.Eta() - 0.5*(Jet.at(0).Eta() + Jet.at(1).Eta())) < 2.5);
  
  set_cut(E_LEP, leptonCuts);

  /// vbs selection
  bool vbsSelection = (Jet.at(0).Pt() > 50 && Jet.at(1).Pt() > 50 ) && (fabs(Jet.at(0).Eta()-Jet.at(1).Eta()) > 2.5) && ((Jet.at(0)+Jet.at(1)).M() > 500);
  set_cut(E_VBS, vbsSelection);


  if (pass_allCuts()) passed++;
}



void Analyzer::fill_histo() {
  histo.fill_hevents(passCut, pass_allCuts());

  ///////////////////////////////
  /////// Pass all cuts ////////
  /////////////////////////////

  if(pass_allCuts()) {
    histo.fill("hno_vbsjet_DEta", fabs(Jet.at(0).Eta()-Jet.at(1).Eta()));
    histo.fill("hno_vbsjet_Mass", (Jet.at(0)+Jet.at(1)).M());

    
    histo.fill("hno_leadjet_Pt", Jet.at(0).Pt());
    histo.fill("hno_leadjet_Eta", fabs(Jet.at(0).Eta()));

    for( auto jet : Jet) {
      histo.fill("hno_jet_Pt",jet.Pt());
      histo.fill("hno_jet_Eta", fabs(jet.Eta()));
    }
    histo.fill("hno_jet_N",Jet.size());
    histo.fill("hno_met",MET.At(0));
    for( auto lep : Muon_T) {
      histo.fill("hno_lep_Pt",lep.Pt());
      histo.fill("hno_lep_Eta", fabs(lep.Eta()));
    }
    for( auto lep : Electron) {
      histo.fill("hno_lep_Pt",lep.Pt());
      histo.fill("hno_lep_Eta",fabs(lep.Eta()));
    }

    TLorentzVector triLep;
    for(auto lep : Muon_T) triLep += lep;
    for(auto lep : Electron) triLep += lep;
    histo.fill("hno_trilep_Mass", triLep.M());
    histo.fill("hno_trilep_Zepp", fabs(triLep.Eta() - 0.5*(Jet.at(0).Eta() + Jet.at(1).Eta())));

  }
}


bool Analyzer::pass_ncut(int cut) const {
  for( int i = 0; i < E_LAST_CUT; i++) {
    if( i != cut && !passCut.at(i) ) {
      return false;
    }
  }
  return true;
}

bool Analyzer::pass_allCuts() const {
  for(auto cut : passCut) {
    if(!cut) return false;
  }
  return true;
}






void Analyzer::overlap(Part& Muon) {
  vector<int> passedOverlap;
  for(int i = 0; i < Jet.size(); i++) {
    bool failed = false;
    for( auto e : Electron) {
      if(Jet.at(i).DeltaR(e) < 0.05) {
	failed = true;
	break;
      }
    }
    if(failed) continue;
    for( auto mu : Muon) {
      if(Jet.at(i).DeltaR(mu) < 0.05) {
	failed = true;
	break;
      }
    }
    if(! failed) passedOverlap.push_back(i);
  }
  Jet.remove(passedOverlap);
}
  
