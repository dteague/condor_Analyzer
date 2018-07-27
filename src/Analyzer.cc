#include "Analyzer.h"

using namespace std;


Analyzer::Analyzer(string jetType) : chain("Delphes"),  treeReader(new TTreeReader(&chain)), Electron(treeReader, "ElectronCHS", 0), Muon_T(treeReader, "MuonTightCHS", 0), Muon_L(treeReader, "MuonLoose", 0), MET(*treeReader, "MissingET.MET"), MET_phi(*treeReader, "MissingET.Phi") {

  Jet = Part(treeReader, jetType);
  
  gErrorIgnoreLevel=kError; /// Ignores silly errors in ExRoot dictionaries    
  partList.push_back(&Jet);
  partList.push_back(&Electron);
  partList.push_back(&Muon_T);
  //    partList.push_back(&Muon_L);

  passCut.resize(E_LAST_CUT);
}


void Analyzer::write_out(string outfile) {
  cout << "Number passed: " << passed << endl;
  histo.write_out(outfile);
}

void Analyzer::add_run_file(string filename) {
  cout << filename << " trying" << endl;
  chain.AddFile(filename.c_str());

  TFile* tmpfile;
  tmpfile = TFile::Open(filename.c_str());
  TH1D* tmphist = 0;
  tmpfile->GetObject("prune_events", tmphist);
  prunedTotal += tmphist->GetBinContent(1);
  tmpfile->Close();
  numberOfEntries = treeReader->GetEntries(1);
  cout << "Number of Events: " << numberOfEntries << endl;
    

}


bool Analyzer::next() {
  counter++;
  bool goNext = treeReader->Next();
  if(goNext) for(auto part : partList) part->setup_lvectors();
  
  return goNext;
}
    
void Analyzer::selection() {
  overlap(Muon_T);
  Muon_T.basic_cuts(15, 2.4, 0.15);
  Electron.basic_cuts(15, 2.5, 0.06);
  Jet.basic_cuts(30, 4.7); 


  bool N_Jet_cut = (Jet.size() > 1); 

  set_cut(E_BJET_N, Jet.n_bjets()==0);

  
  bool min_mass = true;
  bool passZ = true;
  bool N_Lep_cut =  (Muon_T.size()+Electron.size() == 3);
  bool triLepCut = true;
  
  TLorentzVector triLep;
  for(auto lep : Muon_T) triLep += lep;
  for(auto lep : Electron) triLep += lep;

  if(N_Lep_cut) {
    triLepCut = (triLep.M() > 100);

    passZ = Zcand.passZCuts(Muon_T, Electron) && (Zcand.getW().Pt() > 20);
    vector<TLorentzVector> leps;
    for(auto lep : Muon_T) leps.push_back(lep);
    for(auto lep : Electron) leps.push_back(lep);
    min_mass = min_mass && ((leps.at(0)+leps.at(1)).M() >4);
    min_mass = min_mass && ((leps.at(0)+leps.at(2)).M() >4);
    min_mass = min_mass && ((leps.at(1)+leps.at(2)).M() >4);
  }


  set_cut(E_LEP, N_Lep_cut && min_mass && passZ && triLepCut);
  
  /// vbs selection
  bool vbsSelection = (N_Jet_cut) ? (Jet.at(0).Pt() > 50 && Jet.at(1).Pt() > 50 ) && (fabs(Jet.at(0).Eta()-Jet.at(1).Eta()) > 2.5) && ((Jet.at(0)+Jet.at(1)).M() > 500) : false;
  bool ZeppCut = (N_Jet_cut && N_Lep_cut) ? (fabs( triLep.Eta() - 0.5*(Jet.at(0).Eta() + Jet.at(1).Eta())) < 2.5) : false;

  set_cut(E_JET_CUTS, vbsSelection && ZeppCut && N_Jet_cut);

  if (pass_allCuts()) passed++;
}



void Analyzer::fill_histo() {
  histo.fill_hevents(passCut, pass_allCuts());

  ///////////////////////////////
  /////// Pass all cuts ////////
  /////////////////////////////

  if(pass_allCuts()) {
    if(Muon_T.size() == 3) write_lepCh("hall_", "mmm_");
    if(Muon_T.size() == 2) write_lepCh("hall_", "mme_");
    if(Muon_T.size() == 1) write_lepCh("hall_", "mee_");
    if(Muon_T.size() == 0) write_lepCh("hall_", "eee_");
    write_lepCh("hall_", "");
  }
  if(pass_ncut(E_JET_CUTS)) {
    write_lepCh("hjets_", "");
  }
	
}

void Analyzer::write_lepCh(string section, string lepVals) {
  string str = section + lepVals;
  if(Jet.size() > 0) {
    histo.fill(str + "leadjet_Pt", Jet.at(0).Pt());
    histo.fill(str + "leadjet_Eta", fabs(Jet.at(0).Eta()));
  }
  if(Jet.size() > 1) {
    histo.fill(str + "vbsjet_DEta", fabs(Jet.at(0).Eta()-Jet.at(1).Eta()));
    histo.fill(str + "vbsjet_Mass", (Jet.at(0)+Jet.at(1)).M());
  }

  for( auto jet : Jet) {
    histo.fill(str + "jet_Pt",jet.Pt());
    histo.fill(str + "jet_Eta", fabs(jet.Eta()));
  }
  histo.fill(str + "jet_N",Jet.size());
  histo.fill(str + "met",MET.At(0));

  histo.fill(str + "Z_Mass", Zcand.mass());
  TLorentzVector tmpWZ;
  tmpWZ.SetPtEtaPhiM(MET.At(0), 0, MET_phi.At(0), 0);
  histo.fill(str + "WZ_Mass", Zcand.WZ_Mt(tmpWZ));
  
  TLorentzVector leadlep;
  for( auto lep : Muon_T) {
    histo.fill(str + "lep_Pt",lep.Pt());
    histo.fill(str + "lep_Eta", fabs(lep.Eta()));
    if(leadlep.Pt() < lep.Pt()) leadlep = lep;
  }
  for( auto lep : Electron) {
    histo.fill(str + "lep_Pt",lep.Pt());
    histo.fill(str + "lep_Eta",fabs(lep.Eta()));
    if(leadlep.Pt() < lep.Pt()) leadlep = lep;
  }
  histo.fill(str + "leadlep_Pt", leadlep.Pt());
  histo.fill(str + "leadlep_Eta", leadlep.Eta());
  TLorentzVector triLep;
  for(auto lep : Muon_T) triLep += lep;
  for(auto lep : Electron) triLep += lep;
  histo.fill(str + "trilep_Mass", triLep.M());
  if(Jet.size() > 2) histo.fill(str + "trilep_Zepp", fabs(triLep.Eta() - 0.5*(Jet.at(0).Eta() + Jet.at(1).Eta())));

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
      if(Jet.at(i).DeltaR(e) < 0.4) {
	failed = true;
	break;
      }
    }
    if(failed) continue;
    for( auto mu : Muon) {
      if(Jet.at(i).DeltaR(mu) < 0.4) {
	failed = true;
	break;
      }
    }
    if(! failed) passedOverlap.push_back(i);
  }
  Jet.remove(passedOverlap);
}
  
