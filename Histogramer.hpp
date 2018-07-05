#include <unordered_map>
#include <TH1.h>
#include <TTree.h>
#include <TBranch.h>

class Histograms {
public: 
  Histograms();
  ~Histograms();

  void setup_hevents(vector<string> binNames);
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
};
