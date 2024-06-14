#ifndef SiPM
#define SiPM
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

/**
 *
 * This class deals with the read out of Janus SiPM data from the NSCL merged data stream
 * Uses the CAEN DT5202(?) to control the SiPMs
 */

class sipm
{
 public:

  //This template class does the unpacking work. It will take in a position p in
  //the buffer and the variable t it expects with type T, saving the data in the
  //buffer to the indicated variable t, then advancing the buffer.
  //reads in data Little Endian
  template<class T> void set_val(T& t, char*& p)
  {
    t = *reinterpret_cast<T*>(p);
    p += sizeof(T);
  }

  //Store data in arrays board, channel, TOA, TOT.
  //128 Fiber limit, NSolutions tells you how many to iterate over

  unsigned char TimeUnit;
  unsigned char acqMode;
  unsigned short evtsize;
  unsigned char boardID;
  double Tref;
  unsigned short hitnum;

  int board[128];
  int channel[128];
  int TOA[128];
  int TOT[128];
  bool read(ifstream *point, int payloadsize);

  int NSolutions; //Total solution number
  int NB0; //Number from board 0
  int NB1; //Number from board 1

  int channel_B0[64];
  int TOA_B0[64];
  int TOT_B0[64];

  int channel_B1[64];
  int TOA_B1[64];
  int TOT_B1[64];

  //This is the timestamp that appears in the fragment header
  double TimeStamp;

};
#endif
