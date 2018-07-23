#ifndef _Z_HELPER_H_
#define _Z_HELPER_H_

#include <TLorentzVector.h>
#include <vector>

struct Z_Help {
  Z_Help(int _q, bool _isMuon, TLorentzVector _pvec) : q(_q), isMuon(_isMuon), pvec(_pvec) {}
  int q;
  bool isMuon;
  TLorentzVector pvec;
};




#endif
