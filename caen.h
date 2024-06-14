#ifndef CAEN
#define CAEN
#include <cstdlib>
/**
 * !\brief handles the readout from a CAEN ADC,QDC,TDC
 *
 * This class deals with the read out from a number of 
 * CAEN VME modules
 */

class caen
{
 public:

  //Data is stored in arrays channel, data, underflow, and overflow
  //Only using 2 channels so this is overkill
  //The integer NSolutions will tell you how many channels you need to iterate over
  unsigned short number; // number of converted channels
  unsigned short crate; // crate number
  unsigned short geographic; // geagraphic address of module

  //ADC event length
  unsigned short adcsize;

  //SIS time stamp
  //Timestamp in hex will be organized as high2 + mid2 + low2
  //So something like high2mid2low2 = 0x00000001aeea
  unsigned short low1;
  unsigned short mid1;
  unsigned short low2;
  unsigned short mid2;
  unsigned short high1;
  unsigned short high2;
  double SISTimeStamp;

  unsigned short  channel[32];
  unsigned short  data[32];
  unsigned short  underflow[32];
  unsigned short  overflow[32];
  bool read(unsigned short *& point);

  int NSolutions;

  //This is the timestamp that appears in the fragment header
  double TimeStamp;

};
#endif
