#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <TLorentzVector.h>
#include <TTreeReaderArray.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TDirectory.h>
#include <TEntryList.h>
#include <TROOT.h>
#include <TBranch.h>
#include <TH1.h>
#include <TFile.h>

#include <bitset>
#include <vector>

using namespace std;
using namespace ROOT::Internal;

class Part {
public:
  typedef typename vector<TLorentzVector>::iterator iterator;
  typedef typename vector<TLorentzVector>::const_iterator const_iterator;

  Part() {}
  Part(TTreeReader *reader, string name, double _mass);
  Part(TTreeReader *reader, string name);

  void basic_cuts(double ptCut, double etaCut);
  void basic_cuts(double ptCut, double etaCut, double);

  void setup_lvectors();
  void remove(vector<int> toKeep);
  int n_bjets() const;
  
  int q(int spot) const {return thisCharge.at(spot);}

  inline int size() const {return vecs.size();} 
  const TLorentzVector at(int i) const {return vecs.at(i);} 
  inline iterator begin() noexcept { return vecs.begin(); }
  inline const_iterator cbegin() const noexcept { return vecs.cbegin(); }
  inline iterator end() noexcept { return vecs.end(); }
  inline const_iterator cend() const noexcept { return vecs.end(); }

protected:
  
  TTreeReaderArray<float>* Pt;
  TTreeReaderArray<float>* Eta;
  TTreeReaderArray<float>* Phi;
  TTreeReaderArray<float>* M;
  TTreeReaderArray<float>* iso;
  TTreeReaderArray<int>* Charge;
  TTreeReaderArray<uint>* BTag;

  double mass;
  bool isJet = false;

  vector<TLorentzVector> vecs;
  vector<int> thisCharge;
  vector<uint> isBJet;
  vector<float> isoVar;
};

#endif







