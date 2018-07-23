#ifndef _ANALYZER_H_
#define _ANALYZER_H_

#include "TTreeReader.h"
#include "TChain.h"
#include "TFile.h"
#include <TTreeReaderArray.h>
#include <TTreeReaderValue.h>

#include <vector>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "Particles.h"
#include "Histogramer.h"
#include "ZCandidate.h"

using namespace std;

class Analyzer {
public:
  
  Analyzer(string);

  void write_out(string outfile);
  void write_lepCh(string section, string lepVals);
  bool next();
  void selection();
  void fill_histo();
  void add_run_file(string filename);
  inline int event_number() {return counter;}
  inline double event_percent() {return ((double)counter)/numberOfEntries;}
  inline int n_events() {return numberOfEntries;}
  
  ~Analyzer() {delete treeReader;}
  void set_events() {histo.setup_hevents(cutNames, prunedTotal);}
  
private:
  TChain chain;
  TTreeReader *treeReader;
  int numberOfEntries = 0;
  int counter = 0;
  int passed = 0;
  
  Part Jet;
  Part Electron;
  Part Muon_T;
  Part Muon_L;
  vector<Part*> partList;
  TTreeReaderArray<float> MET;
  TTreeReaderArray<float> MET_phi;
  ZCandidate Zcand;


  int prunedTotal = 0;
  
  Histograms histo;

  void overlap(Part& Muon);
  bool pass_ncut(int) const;
  
  vector<bool> passCut;
  void set_cut(int cut, bool pass) { passCut[cut] = pass;}


  bool pass_allCuts() const;
  
  enum CUT_NAMES            {  E_JET_CUTS,    E_BJET_N,    E_LEP, E_LAST_CUT };
  vector<string> cutNames = {"Jet Cuts", "BJet Number", "Lepton"};

};

#endif
