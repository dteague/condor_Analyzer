#ifndef _HISTOGRAMER_H_
#define _HISTOGRAMER_H_

#include <TFile.h>
#include <TH1.h>
#include <TTree.h>
#include <TBranch.h>
#include <TList.h>

#include <unordered_map>
#include <istream>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class Histograms {
public: 
  Histograms();
  void setup_hevents(vector<string> binNames, int totalEvents);
  void fill_hevents(vector<bool>& cutflows, bool pass);
  void write_out(string outfile);
  void fill(string histname, double value);
  void draw(string histname);


private:
  unordered_map<string, TH1D*> hMap;
  bool madeEvents = false;

  TH1D* events;
  vector<bool> eventsCutFlow;
  TTree* eventTree;
  TBranch* eventBranch;
  TList* eventCutNames;
  vector<string> dirs = {"mmm", "mme", "mee", "eee"};
  string allDir = "all";
    
};

#endif
