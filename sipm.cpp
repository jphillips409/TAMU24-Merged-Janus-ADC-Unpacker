#include "sipm.h"
#include <fstream>
#include <iomanip>
using namespace std;

//*********************************************************************

  // Channel number is unraveled so that it maps directly onto channel number
  // So for board 0, channel 0 will be fiber 0
  // For the total arrays, they can hold 128 channels in whatever order they are read in

//*********************************************************************
bool sipm::read(ifstream *point, int payloadsize)
{

  //Read in the inclusive size in case the event is bad
  unsigned short InclSize[1];
  point->read((char*)InclSize,2);
  short InclusiveSize = InclSize[0];

  //Read in a buffer of the length Inclusive size - 2 bytes
  //int janusbytes = InclusiveSize - sizeof(InclusiveSize);
  //Temp fix for when the file doesn't have correct source ID
  int janusbytes = payloadsize - 28 - sizeof(InclusiveSize);
  char janusbuff[janusbytes];
  point->read((char*)janusbuff,janusbytes);

  //Make pointer to the buffer, now we can recast variables to their appropriate size using set_val()
  char* p_buff = janusbuff;

  set_val(TimeUnit,p_buff);
  set_val(acqMode,p_buff);

  //If acquisition mode is not 2 (timing), return
  if (acqMode != 2)
  {
    return true;
  }

  //Get event size
  set_val(evtsize,p_buff);

  //Get board ID - only 1 per fragment
  set_val(boardID,p_buff);

  //Get TRef time stamp
  set_val(Tref,p_buff);

  //Get number of hits
  set_val(hitnum,p_buff);

  for (int i=0;i<hitnum;i++)
  {
    //Extract using temporary variables
    char channel_temp = 0;
    char datatype_temp = 0;
    int32_t TOA_temp = 0;
    int16_t TOT_temp = 0;  

    set_val(channel_temp,p_buff);
    set_val(datatype_temp,p_buff);
    set_val(TOA_temp,p_buff);
    set_val(TOT_temp,p_buff);

    //if data_type is not 48, continue. If channel is not between 0 and 63, continue.
    if (channel_temp < 0 || channel_temp > 63 || datatype_temp != 48) continue;

    //Reorder so that fiber num = chan num
    int ichan = channel_temp;
    int ifib = ((ichan - (ichan % 2)) / 2) + ((ichan % 2) * 32);
    
    board[i] = boardID;
    channel[i] = ifib;
    TOA[i] = TOA_temp;
    TOT[i] = TOT_temp;

    NSolutions++; //Tick up the number of solutions

    //Get board solutions
    if (boardID == 0)
    {
      channel_B0[NB0] = ifib;
      TOA_B0[NB0] = TOA_temp;
      TOT_B0[NB0] = TOT_temp;
      NB0++;
    }
    if (board[i] == 1)
    {
      channel_B1[NB1] = ifib;
      TOA_B1[NB1] = TOA_temp;
      TOT_B1[NB1] = TOT_temp;
      NB1++;
    }
  
  }

  return true;
}
