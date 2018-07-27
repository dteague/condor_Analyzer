
void getCutflow() {
  TFile* infile = new TFile("loose.root");
  TTree* tree = (TTree*)infile->Get("EventsCutFlow");
  TBranch* branch = 0;
  vector<bool>* cutBins = 0;
  tree->SetBranchAddress("EventBranch", &cutBins, &branch);
  int nevents = tree->GetEntries();
  cout << endl;
  TList* cutList = (TList*)infile->Get("TList");
  for(auto item : *cutList) cout << item->GetName() << endl;
  cout << endl;
  
  vector<int> cutflow;
  vector<int> cutOrder = {0,  3, 1, 2};
  vector<string> names;
  for(auto i : cutOrder) {
    if(i == 0) names.push_back("All events");
    else names.push_back(cutList->At(i-1)->GetName());
  }
  
  cutflow.resize(cutOrder.size());


  
  for(int i = 0; i < nevents; i++) {
    Long64_t tentry = tree->LoadTree(i);
    branch->GetEntry(tentry);
    bool overallTruth = true;
    for(int j=0; j < (int)cutOrder.size(); j++) {
      overallTruth = overallTruth && cutBins->at(cutOrder.at(j));
      if(overallTruth) cutflow[j]++;
    }
    
  }
  for(int i = 0; i < cutflow.size(); i++) {
    cout << setw(15) << names[i] << ":   " << cutflow[i] << endl;
  }
}
