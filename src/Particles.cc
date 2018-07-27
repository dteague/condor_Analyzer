#include "Particles.h"

typedef std::bitset<16> IntBits;

namespace BTAG {
  int loose = 0;
  int medium = 1;
  int tight = 2;
  int looseMTD = 3;
  int mediumMTD = 4;
  int tightMTD = 5;
}

using namespace ROOT::Internal;

void Part::basic_cuts(double ptCut, double etaCut) {
  vector<int> passCuts;
  int i = 0;
  for( auto p = begin(); p != end(); p++,i++) {
    if(p->Pt() < ptCut) continue;
    if(fabs(p->Eta()) > etaCut) continue;
    passCuts.push_back(i);
  }
  remove(passCuts);
}

void Part::basic_cuts(double ptCut, double etaCut, double isoCut) {
  vector<int> passCuts;
  int i = 0;
  for( auto p = begin(); p != end(); p++,i++) {
    if(p->Pt() < ptCut) continue;
    if(fabs(p->Eta()) > etaCut) continue;
    if(isoVar.at(i) > isoCut) continue;
    passCuts.push_back(i);
  }
  remove(passCuts);
}

Part::Part(TTreeReader *reader, string name, double _mass) : mass(_mass) {
  Pt = new TTreeReaderArray<float>(*reader, (name+".PT").c_str());
  Eta = new TTreeReaderArray<float>(*reader, (name+".Eta").c_str());
  Phi = new TTreeReaderArray<float>(*reader, (name+".Phi").c_str());
  Charge = new TTreeReaderArray<int>(*reader, (name+".Charge").c_str());
  if(mass > -1) iso = new TTreeReaderArray<float>(*reader, (name+".IsolationVarRhoCorr").c_str());
}

Part::Part(TTreeReader *reader, string name) : Part(reader, name, -1) {
  M = new TTreeReaderArray<float>(*reader, (name+".Mass").c_str());
  BTag = new TTreeReaderArray<uint>(*reader, (name+".BTag").c_str());
  isJet=true;
}


void Part::setup_lvectors() {
  vecs.clear();
  thisCharge.clear();
  isBJet.clear();
  isoVar.clear();
    
  TLorentzVector tmp_vec;
  for(int i = 0; i < (int)Pt->GetSize(); i++) {
    if(isJet) {
      mass = M->At(i);
      isBJet.push_back(BTag->At(i));
    } else {
      isoVar.push_back(iso->At(i));
    }
    tmp_vec.SetPtEtaPhiM(Pt->At(i), Eta->At(i), Phi->At(i), mass);
    vecs.push_back(tmp_vec);
    thisCharge.push_back(Charge->At(i));
  }
}


int Part::n_bjets() const {
  int numberBjets = 0;
  for(auto isABjet : isBJet) {
    if(IntBits(isABjet).test(BTAG::tightMTD)) numberBjets++;
  }
  return numberBjets;
}


void Part::remove(vector<int> toKeep) {
  vector<TLorentzVector> tmpVec = vecs;
  vector<int> tmpCharge = thisCharge;
  vector<uint> tmpBJet = isBJet;
  vector<float> tmpIso = isoVar;

  vecs.clear();
  thisCharge.clear();
  isBJet.clear();
  isoVar.clear();
  
  for(auto index: toKeep) {
    if(isJet)  isBJet.push_back(tmpBJet.at(index));
    else  isoVar.push_back(tmpIso.at(index));
    
    vecs.push_back(tmpVec.at(index));
    thisCharge.push_back(tmpCharge.at(index));
  }
}




