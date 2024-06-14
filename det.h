#ifndef det_H
#define det_H
#include <iostream>
#include <string>
#include "caen.h"
#include "sipm.h"
#include "histo.h"

//Johnathan Phillips 2024_06_10
//Deals with unpacking a CAEN ADC and Janus events from a merged datastream.
//Also deals with ADC calibrations and sorted data analysis.

class det
{
 public:

  det(histo * Histo1);
  ~det();

  bool unpack(ifstream *point, int evtsize); //evtsize does not include the event header, so it's 8 less than value
  histo * Histo;


//private:
  caen * ADC;
  sipm * Janus;

  int counter=0; //counts unpack numbers

  int itHappened=0; //counter to stop at bad events
  int cntNoAdc=0;
  int cntTwoFiber=0;
  int cntTwoFiberADC=0;
  int runNum = 0;

  int cntADC = 0; //Number of events with the 0 degree scintt
  int cntADC_fiber = 0; //Number of events with a 0 degree scint and fibers from boards 0 and 1

};


#endif
