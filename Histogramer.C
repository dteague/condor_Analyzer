#include <unordered_map>
#include <TFile.h>
#include <TH1.h>

class Histograms {
public: 
  Histograms() {
    ifstream ifs;
    ifs.open("hist.config", ifstream::in);
    for (std::string line; std::getline(ifs, line); ) {
      istringstream iss(line);
      vector<string> tokens{istream_iterator<string>{iss},
	  istream_iterator<string>{}};
      if(tokens.size() == 0) continue;
      else if(tokens[0].at(0) == '#') continue;
      else if(tokens.size() != 4) {
	cout << line << endl;
	cout << "==== This line has a problem: not 4 items" << endl;
	exit(1);
      }
      hMap[tokens[0]] = new TH1D(tokens[0].c_str(), tokens[0].c_str(), stod(tokens[1]), stod(tokens[2]), stod(tokens[3])); 
      std::cout << line << std::endl;
    }
  }
  ~Histograms() {}


  void write_out(string outfile) {
    TFile* out = new TFile(outfile.c_str(), "RECREATE");
    out->cd();
    for(auto graph : hMap) {
      out->WriteTObject(graph.second);
    }
    out->Close();
  }

  void fill(string histname, double value) {
    if(hMap[histname] == nullptr) {
      cout << histname << " : isn't in setupfile, please restart" << endl;
      exit(1);
    }
    hMap[histname]->Fill(value);
  }

  void draw(string histname) {
    if(hMap[histname] == nullptr) {
      cout << histname << " : isn't in setupfile, please restart" << endl;
      exit(1);
    }
    hMap[histname]->Draw();
  }

private:
  unordered_map<string, TH1D*> hMap;
};
