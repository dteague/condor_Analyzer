#include "Histogramer.h"

using namespace std;

Histograms::Histograms() {
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
  }
}


void Histograms::setup_hevents(vector<string> binNames, int totalEvents) {
  madeEvents = true;

  events = new TH1D("Events", "Events", 2, 0, 2);
  events->SetBinContent(1, totalEvents);

  eventsCutFlow.resize(binNames.size()+2);
  eventTree = new TTree("EventsCutFlow", "EventsCutFlow");
  eventBranch = eventTree->Branch("EventBranch", &eventsCutFlow);

  eventCutNames = new TList();
  for(auto names: binNames) eventCutNames->Add(new TObjString(names.c_str()));
}

void Histograms::fill_hevents(vector<bool>& cutflows, bool pass) {
  if(!madeEvents) return;
  eventsCutFlow[0] = true;
  for (int i=0;i<(int)cutflows.size();i++)  {
    eventsCutFlow[i+1] = cutflows.at(i);
  }
  eventsCutFlow[cutflows.size()+1] = pass;
  if(pass) events->Fill(1);
    
  eventTree->Fill();
}
  
void Histograms::write_out(string outfile) {
  TFile* out = new TFile(outfile.c_str(), "RECREATE");
  out->cd();

  for(auto d : dirs ) {
    out->mkdir(d.c_str());
  }
  out->mkdir(allDir.c_str());
    
  for(auto graph : hMap) {
    out->cd("");
    string graphName = graph.first;
    bool inTotal = true;
    for(auto d : dirs ) {
      if(graphName.find(d) != string::npos) {
	gDirectory->cd(d.c_str());
	graph.second->Write();
	inTotal = false;
	break;
      }
    }
    if(inTotal) {
      gDirectory->cd(allDir.c_str());
      graph.second->Write();
    }
  }

    
  gDirectory->cd();
  if(madeEvents) {
    out->WriteTObject(eventTree);
    out->WriteTObject(eventCutNames);
    out->WriteTObject(events);
  }
  out->Close();
}

void Histograms::fill(string histname, double value) {
  if(hMap[histname] == nullptr) {
    cout << histname << " : isn't in setupfile, please restart" << endl;
    exit(1);
  }
  hMap[histname]->Fill(value);
}

void Histograms::draw(string histname) {
  if(hMap[histname] == nullptr) {
    cout << histname << " : isn't in setupfile, please restart" << endl;
    exit(1);
  }
  hMap[histname]->Draw();
}

