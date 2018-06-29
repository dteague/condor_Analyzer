#include <vector>
#include <TLorentzVector.h>
#include <TTreeReaderArray.h>

class Part {
public:
  typedef typename vector<TLorentzVector>::iterator iterator;
  typedef typename vector<TLorentzVector>::const_iterator const_iterator;

  Part() {}

  Part(TTreeReader *reader, string name, double mass) {
    Pt = new TTreeReaderArray<float>(*reader, (name+".PT").c_str());
    Eta = new TTreeReaderArray<float>(*reader, (name+".Eta").c_str());
    Phi = new TTreeReaderArray<float>(*reader, (name+".Phi").c_str());
    Charge = new TTreeReaderArray<int>(*reader, (name+".Charge").c_str());
    mass = this->mass;
  }


  Part(TTreeReader *reader, string name) : Part(reader, name, 0) {
    M = new TTreeReaderArray<float>(*reader, (name+".Mass").c_str());
    isTau = new TTreeReaderArray<uint>(*reader, (name+".TauTag").c_str());
    isJet=true;
  }

  void basic_cuts(double ptCut, double etaCut);

  virtual void setup_lvectors() {
    vecs.clear();
    thisCharge.clear();
    thisTau.clear();
    TLorentzVector tmp_vec;
    for(int i = 0; i < (int)Pt->GetSize(); i++) {
      if(isJet) {
	mass = M->operator[](i);
	thisTau.push_back((bool)isTau->At(i));
      }
      tmp_vec.SetPtEtaPhiM(Pt->operator[](i), Eta->operator[](i), Phi->operator[](i), mass);
      vecs.push_back(tmp_vec);
      thisCharge.push_back(Charge->At(i));
    }
  }

  void remove(int nAfter) {
    vecs.erase(begin() + nAfter);
    thisCharge.erase(thisCharge.begin() + nAfter);
  }

  int q(int spot) const {return thisCharge.at(spot);}
  bool is_tau(int spot) const {return thisTau.at(spot);}

  inline int size() const {return vecs.size();} 
  const TLorentzVector operator[](int i) {return vecs[i];} 
  inline iterator begin() noexcept { return vecs.begin(); }
  inline const_iterator cbegin() const noexcept { return vecs.cbegin(); }
  inline iterator end() noexcept { return vecs.end(); }
  inline const_iterator cend() const noexcept { return vecs.end(); }

protected:
  
  TTreeReaderArray<float>* Pt;
  TTreeReaderArray<float>* Eta;
  TTreeReaderArray<float>* Phi;
  TTreeReaderArray<float>* M;
  TTreeReaderArray<int>* Charge;
  TTreeReaderArray<uint>* isTau;
  double mass;
  bool isJet = false;

  vector<TLorentzVector> vecs;
  vector<int> thisCharge;
  vector<bool> thisTau;
};

void Part::basic_cuts(double ptCut, double etaCut) {
  for( auto p = begin(); p != end(); p++) {
    if(p->Pt() > ptCut) continue;
    if(fabs(p->Eta()) > etaCut) continue;

    remove(p - begin());
    p--;
  }
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class Tau : public Part {
public:
  Tau() {}

  void add_tau(TLorentzVector& tmp) {vecs.push_back(tmp); }
  void setup_lvectors() {vecs.clear();}
};



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////





class ZCandidate {
public:  
  ZCandidate(Part& _Muons, Part& _Electrons) : Muons(_Muons), Electrons(_Electrons) {

    ZCuts(Muons, zCanMuon);
    ZCuts(Electrons, zCanElec);
  }

  inline int size() const {return zCanMuon.size() + zCanElec.size();} 

private:
  const double ZMASS = 91.1876;
  const double acceptBand = 15;
  const double acceptBandUp = ZMASS+15;
  const double acceptBandDown = ZMASS-15;
  Part &Muons;
  Part &Electrons;
  vector<pair<int, int> > zCanMuon;
  vector<pair<int, int> > zCanElec;

  void ZCuts(Part& part,   vector<pair<int, int> >& zCan);


};


void ZCandidate::ZCuts(Part& part,   vector<pair<int, int> >& zCan) {
  if( part.size() >= 2) {
    for(auto p1 = part.begin(); p1 != part.end(); p1++) {
      for(auto p2 = p1+1; p2 != part.end(); p2++) {
	if (part.q(p1-part.begin())*part.q(p2-part.begin()) > 0) continue;
	double mass = (*p1 + *p2).M();
	if (mass > acceptBandUp || mass < acceptBandDown) continue;

	zCan.push_back(make_pair(p1-part.begin(), p2-part.begin()));
      }
    }
  }
  
}
