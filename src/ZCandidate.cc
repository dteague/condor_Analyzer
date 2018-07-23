#include "ZCandidate.h"

using namespace std;

bool ZCandidate::passZCuts(const Part &Muons, const Part &Elec) {
  double mass[3] = {-1, -1, -1};
  vector<Z_Help> parts;
  for(int i=0; i< Muons.size(); i++) parts.push_back(Z_Help(Muons.q(i), true, Muons.at(i)));
  for(int i=0; i< Elec.size(); i++) parts.push_back(Z_Help(Elec.q(i), false, Elec.at(i)));
  mass[0] = testPair(parts.at(1), parts.at(2));
  mass[1] = testPair(parts.at(0), parts.at(2));
  mass[2] = testPair(parts.at(0), parts.at(1));

  double diff = fabs(mass[0]-ZMASS);
  int index = 0;
 
  for (int c = 1; c < 3; c++) {
    if (fabs(mass[c]-ZMASS) < diff) {
       index = c;
       diff = fabs(mass[c]-ZMASS);
    }
  }

  if(diff > acceptBand) return false;
  int z1 = (index+1)%3;
  int z2 = (index+2)%3;

  Wlep = parts.at(index).pvec;
  Zpair = make_pair(parts.at(z1).pvec, parts.at(z2).pvec);
  return true;
}

double ZCandidate::testPair(const Z_Help p1, const Z_Help p2) {
  if(p1.isMuon != p2.isMuon) return -1;

  if(p1.q * p2.q > 0) return -1;
  double mass = (p1.pvec + p2.pvec).M();
  if (mass > acceptBandUp || mass < acceptBandDown) return -1;
  if (p1.pvec.Pt() < 25 && p2.pvec.Pt() < 25) return -1;
  return mass;
}

double ZCandidate::WZ_Mt(TLorentzVector met) {
  TLorentzVector W_total = met+Wlep;
  TLorentzVector Z_total = Zpair.first + Zpair.second;
  return sqrt(pow(W_total.Et() + Z_total.Et(), 2) -pow((W_total + Z_total).Pt(), 2) );
}


