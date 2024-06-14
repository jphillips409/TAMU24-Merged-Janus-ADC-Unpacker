#include "caen.h"
#include <iostream>
#include <iomanip>
using namespace std;



//*********************************************************************
bool caen::read(unsigned short * &point)
{

  //Read ADC size
  adcsize = *point++;

  //Read SIS clock
  low1 = *point++;
  mid1 = *point++;
  low2 = *point++;
  mid2 = *point++;
  high1 = *point++;
  high2 = *point++;

  //Reorder SIS clock to get SIS timestamp
  int64_t high2_shift = high2;
  high2_shift = high2_shift << 32;
  int32_t mid2_shift = mid2;
  mid2_shift = mid2_shift << 16;
  SISTimeStamp = high2_shift + mid2_shift + low2;

  // read header
  unsigned short header2 = *point++;
  unsigned short header1 = *point++;

  //cout << "here " << header2 << " " << header1 << endl;

  //extract information
  unsigned short bit =  (header1>>9) & 0x1;
  //if (bit == 0) cout << "header not found" << endl;
  geographic = header1>>11;

  crate = (header1 >> 3) & 0x7F;
  number =(header2 >> 8) & 0x3F;

  //cout << dec << "here " << geographic << " " << crate << " " << number << endl;

  // loop over each channel
  int j=0;
  for (int i=0;i<number;i++)
  {
    unsigned short data2 = *point++;
    unsigned short data1 = *point++;
    bit = (data1>>10) & 0x1;

    unsigned short Ichan = data1 & 0x1F;
    unsigned short value = data2 & 0xFFF;
    //cout << data2 << " " << 0xFFF << " " << value << endl;
    
    if (bit)
    {
      //this is a trailer block
      //cout << "missing caen data, j=" << j <<"  number = " << number << " i= " << i << endl;
      //cout << "Ichan = " << Ichan << " value = " << value << endl;
      //cout << data1 << " " << data2 << endl;
      //point -= 2;
      number = j;
      return true;
    }
 
    if (Ichan < 32)
    {
      channel[j] = Ichan;
      underflow[j] = (data2>>13)&0x1;
      overflow[j] = (data2>>12)&0x1;
      if (overflow[j]) cout << "overflow "<< value << endl;
      if (underflow[j]) cout << "underflow " << value << endl;

      data[j] = value;

      //cout << dec << "here " << Ichan << " " << value << endl;
  
      NSolutions++;

      j++;
      if (j > 64) 
      {
        cout << "more than 64 channels in Caen" << endl;
        break;
      }

     }
    //cout << Ichan << " " << value << endl;
  }

  //read end of block
  header2 = *point++;
  header1 = *point++;
  bit = (header1>>10) & 0x1;

  //cout << hex << "here " << header2 << " " << header1 << endl; 

  if (bit == 0) 
  {
    cout << "caen End-of-Block not found for slot "  
         << geographic << endl;
    return false;
  }
  return true;
}

