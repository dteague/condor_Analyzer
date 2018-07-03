
void getCutflow() {
  TFile* infile = new TFile("jet_4.root");
  TTree* tree = (TTree*)infile->Get("EventsPassed");
  TBranch* branch = 0;
  vector<bool>* cutBins = 0;
  tree->SetBranchAddress("EventBranch", &cutBins, &branch);
  int nevents = tree->GetEntries();

  vector<int> cutflow;
  vector<int> cutOrder = {0, 3, 1, 2, 4};
  
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
  for(auto cutValues : cutflow) {
    cout << cutValues << endl;
  }
}
