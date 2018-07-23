#ifndef _ZCANDIDATE_H_
#define _ZCANDIDATE_H_

#include <TLorentzVector.h>

#include <vector>
#include <math.h>

#include "Particles.h"
#include "Z_helper.h"

class ZCandidate {
public:  
  bool passZCuts(const Part&, const Part&);
  double mass() {return (Zpair.first + Zpair.second).M();}
  TLorentzVector getW() {return Wlep;}
  double WZ_Mt(TLorentzVector);
  
private:
  const double ZMASS = 91.1876;
  const double acceptBand = 15;
  const double acceptBandUp = ZMASS+acceptBand;
  const double acceptBandDown = ZMASS-acceptBand;

  double testPair(const Z_Help p1, const Z_Help p2);
  pair<TLorentzVector,TLorentzVector> Zpair;
  TLorentzVector Wlep;
};

#endif
