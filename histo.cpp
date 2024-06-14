#include "histo.h"

histo::histo()
{

  //create root file
  file_read = new TFile ("sort.root","RECREATE");

  file_read->cd();

  //Make directories
  dir1dBoard0TOA_R = new TDirectoryFile("Board0TOA_R","Board0TOA_R");
  dir1dBoard0TOT_R = new TDirectoryFile("Board0TOT_R","Board0TOT_R");

  dir1dBoard1TOA_R = new TDirectoryFile("Board1TOA_R","Board1TOA_R");
  dir1dBoard1TOT_R = new TDirectoryFile("Board1TOT_R","Board0TOT_R");

  dir1dADCEnergy_R = new TDirectoryFile("ADCEnergy_R","ADCEnergy_R");

  dirSummary = new TDirectoryFile("FiberSum","FiberSum");
  dirHitMaps = new TDirectoryFile("HitMaps","HitMaps");

  ostringstream name;

  int fnum = 64;
  int adcnum = 2;

  //Make 1d plots
  for (int i=0;i<fnum;i++)
  {
    dir1dBoard0TOA_R->cd();
    name.str("");
    name << "Board0_TOA_" << i << "_R";
    B0TOA_R[i] = new TH1I(name.str().c_str(),"",1024,0,4095);

    dir1dBoard0TOT_R->cd();
    name.str("");
    name << "Board0_TOT_" << i << "_R";
    B0TOT_R[i] = new TH1I(name.str().c_str(),"",1024,0,4095);

    dir1dBoard1TOA_R->cd();
    name.str("");
    name << "Board1_TOA_" << i << "_R";
    B1TOA_R[i] = new TH1I(name.str().c_str(),"",1024,0,4095);

    dir1dBoard1TOT_R->cd();
    name.str("");
    name << "Board1_TOT_" << i << "_R";
    B1TOT_R[i] = new TH1I(name.str().c_str(),"",1024,0,4095);
  }

  for (int i=0;i<adcnum;i++)
  {
    dir1dADCEnergy_R->cd();
    name.str("");
    name << "ADCEnergy_" << i << "_R";
    ADCEnergy_R[i] = new TH1I(name.str().c_str(),"",1024,0,4095);
  }

  dirSummary->cd();

  //create summary spectra for both fiber boards
  //Board 1
  FibervsTOA_Board0 = new TH2I("FibervsTOA_Board0","",64,-0.5,63.5,500,0,2000);
  FibervsTOT_Board0 = new TH2I("FibervsTOT_Board0","",64,-0.5,63.5,512,0,512);

  //Board 2
  FibervsTOA_Board1 = new TH2I("FibervsTOA_Board1","",64,-0.5,63.5,500,0,2000);
  FibervsTOT_Board1 = new TH2I("FibervsTOT_Board1","",64,-0.5,63.5,512,0,512);

  dirHitMaps->cd();

  //Board 1 vs Board 2
  FiberHitMap_R = new TH2I("FiberHitMap_R","",64,-0.5,63.5,64,-0.5,63.5); // Channel vs Channel for board 1 vs board 2
  FiberHitMap_Highest = new TH2I("FiberHitMap_Highest","",64,-0.5,63.5,64,-0.5,63.5); // xy fiber hitmap

}


//*********************************************
histo::~histo()
{
  cout << "start histo destructor" << endl;
  file_read->Write();
  cout << "histo written" << endl;
  file_read->Close();
  cout << "end histo destructor" << endl;

}
