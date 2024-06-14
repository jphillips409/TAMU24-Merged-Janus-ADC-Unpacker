#ifndef histo_
#define histo_
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "TH1F.h"
#include "TF1.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TFile.h"
#include "TGraph.h"
#include "TCanvas.h"


using namespace std;

class histo
{
protected:

  TFile * file_read; //!< output root file

public:
  histo();  //!< constructor
  ~histo();

  //Directories for 1d TOA and TOT plots for both boards
  TDirectory * dir1dBoard0TOA_R;
  TDirectory * dir1dBoard0TOT_R;

  TDirectory * dir1dBoard1TOA_R;
  TDirectory * dir1dBoard1TOT_R;

  TDirectory * dir1dADCEnergy_R;

  //Directories for summary plots and hit maps
  TDirectory * dirSummary;
  TDirectory * dirHitMaps;

  //number of fibers on each board
  static const int fibernum = 64;
  static const int ADCchan = 2;

  //1d spectra
  TH1I * B0TOA_R[fibernum];
  TH1I * B0TOT_R[fibernum];

  TH1I * B1TOA_R[fibernum];
  TH1I * B1TOT_R[fibernum];

  TH1I * ADCEnergy_R[ADCchan];

  //create summary spectra for both fiber boards
  //Board 0
  TH2I * FibervsTOA_Board0;
  TH2I * FibervsTOT_Board0;

  //Board 1
  TH2I * FibervsTOA_Board1;
  TH2I * FibervsTOT_Board1;

  //Board 0 vs Board 1
  TH2I * FiberHitMap_R; // Fiber vs fiber hit map, matches all fibers
  TH2I * FiberHitMap_Highest; // Fiber vs fiber hit map, uses highest TOT pair


};
#endif
