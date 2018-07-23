#include "Analyzer.h"
#include <iostream>
#include <boost/program_options.hpp>

namespace po =  boost::program_options;
using namespace std;

namespace{
  int ERROR_IN_COMMAND_LINE = 1;
} 

int main(int argc, char* argv[]) {
  string infilename;
  string outfile;

  po::options_description desc("Usage: ./Analyzer [OPTION]... -i [INPUT FILE] -o [OUTPUT FILE]\nDelphes Analyzer. Must use pre-skimmed versions to get event information");
  desc.add_options()
    ("cut-info,c",                                  "toggle TTree with cut information to feed to getCutflow.C")
    ("help,h",                                      "Show all options")
    ("infile,i",    po::value<string>(&infilename), "set input root file/input list of files")
    ("outfile,o",   po::value<string>(&outfile),    "set output root file")
    ("puppi-jet,p",                                 "Toggle PUPPI Jets to used")
    ;

  po::positional_options_description p;
  //  p.add("input-file", -1);

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).
	    options(desc).positional(p).run(), vm);

    po::notify(vm);

    if (vm.count("help")) {
      cout << desc;
      return 0;
    }

    po::notify(vm); // throws on error, so do after help in case 
    // there are any problems 
  }   catch(po::error& e)  { 
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
    std::cerr << desc << std::endl; 
    return ERROR_IN_COMMAND_LINE; 
  } 

  if(!vm.count("infile") || !vm.count("outfile")) {
    std::cerr << "ERROR: No input or output file" << std::endl << std::endl; 
    std::cerr << desc << std::endl; 
    return ERROR_IN_COMMAND_LINE; 
  }

  string useJets = "Jet";
  if(vm.count("puppi-jet")) useJets = "JetPUPPI";
  
  Analyzer analyzer(useJets);

  if(infilename.find(".root") != string::npos) {
    analyzer.add_run_file(infilename);
  } else {
    ifstream infile;
    infile.open (infilename);
    if(!infile.good()) {
      std::cerr << "ERROR: Cannont find file" << std::endl << std::endl; 
      std::cerr << "Couldn't open file: " << infilename << std::endl; 
      return ERROR_IN_COMMAND_LINE; 
    }
    
    string line;
    while(getline(infile, line)) {
      analyzer.add_run_file(line);
    }
  } 

  if(vm.count("cut-info")) cout << "cut info" << endl;
  analyzer.set_events();

  if(analyzer.n_events() <=0) exit(0);
  while(analyzer.next()) {
    analyzer.selection();
    analyzer.fill_histo();
    //    int events = analyzer.event_number();
    //    cout << events << endl;
    //    if(events > 500) break;
  }
  analyzer.write_out(outfile);
}
