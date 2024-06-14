//file to unpack data for the scintillating fiber test experiment at TAMU.
//JP, HW, ND, RJC 2024

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include "det.h"
#include "histo.h"
#include <cstdlib>
#include <filesystem>

using namespace std;

int main(int argc, char* argv[])
{
  clock_t t;
  t = clock();

  int NphysicsEvent = 0;
  int NscalerBuffer = 0;
  int Nscalercounter = 0;
  int Npauses = 0;
  int Nresumes = 0;
  int runno = 0;
  int runnum = 0;
  unsigned short *point; //For 2 byte chunks

  ifstream *f_point; //just pass the pointer for evtfile

  int argcounter = 1;
  //TODO change this directory so that you know what files you are reading
  //string directory = "/events/experiment/";
  string directory = "";

  ostringstream outstring;

  //build three classes that are used to organize and unpack each physicsevent
  histo * Histo = new histo();
  det Det(Histo); //I'll try to stick to lowercase=classname, uppercase=variable

  ifstream runFile;
  // default case, get run numbers from numbers.beam
  if (argc == 1)
  {
    runFile.open("numbers.beam");
    //check if file exits
    if (!runFile.is_open())
    {
      cout << "could not open numbers.beam" << endl;
      return 1; 
    }
  }

  else if (argc == 2) // check to see if the argument if a file and if so get run numbers from it.
  {
    runFile.open(argv[argcounter]);
    if (runFile.is_open())
    {
      argc = 1;
      cout << "using runs from " << argv[argcounter] << endl;
    }     
  } 

  ifstream evtfile;
  for (;;)
  {
    //open the file "numbers.beam", a list of run numbers to be unpacked
    //TODO keep a logbook of run numbers so that you know which ones to use after you return
    //I suggest keeping files that list types of runs, e.g. numbers.He6_CD2 for He6 on a CD2
    //target. Later you can run: cp numbers.He6_CD2 numbers.beam

    if (evtfile.is_open())
      cout << "previous run file not closed" << endl;

    if (argc == 1) // get runnumbers from a file
    {
      runFile >> runnum;
      if (runFile.eof()) break;
      if (runFile.bad()) break;
    }
    else if (argc > 1)  // get run numbers from keyboard input
    {
      if (argcounter == argc)
      {
        break;
      }
      else
      {
        runnum = atoi(argv[argcounter]);
        argcounter++;
      }
    }

    // if runs are very long, the data are split into multiple files.
    //you are unlikely to have more than a couple of parts to the data as long as you
    //stop and restart the run once per hour
    for (int iExtra=0;iExtra<3;iExtra++) //loop over split evtfiles
    {
      outstring.str("");
      //get the correct file name with potential of extra split evtfiles
      outstring << directory << "run" << runnum << "/run-" << setfill('0') << setw(4) << runnum;
      outstring << "-" << setfill('0') << setw(2) << iExtra << ".evt";
      
      evtfile.clear();
      evtfile.open(outstring.str().c_str(), ios::binary);

      //move past events that don't have split files
      if (iExtra>0 && !evtfile){break;}

      if (!evtfile.is_open())
      {
        cout << "could not open event file" << endl;
        cout << outstring.str() << endl;
        abort();
        return 1;
      }

      if(evtfile.bad())
      {
        cout << " bad file found" << endl;
        break;
      }

      cout << "reading file: " << outstring.str() << endl;

      int cnt = 0;
      for(;;) //loop over items in a evtfile
      //for(int cnt=0; cnt < 20; cnt++) //useful for debugging
      {
        int const hBufferWords = 4;
        int const hBufferBytes = hBufferWords*2;
        unsigned short hBuffer[hBufferWords];

        //read a section 8 bytes long and split into 2 byte chunks 
        //(each 2byte chunk is little endian)
        evtfile.read((char*)hBuffer,hBufferBytes);
        //example 0x0032    0x0000    0x001e    0x0000
        //        nbytes    nbytes2   type      type2
          
        if(evtfile.eof())
        {
          cout << "eof found" << endl;
          break;
        }

        int nbytes = hBuffer[0];
        int nbytes2 = hBuffer[1];
        int type = hBuffer[2];
        int type2 = hBuffer[3];

        //if there is an issue reading ring buffer, look at these
        //cout << hex << nbytes << " " << nbytes2 << " " << type << " " << type2 << endl;
        //if done properly, nbytes and nbytes2 should be combined but in practice
        //values are never large enough.
        
          
        //debug code to look at contents of each ring buffer
        //cout << "evnt cnt:" << cnt << endl;
       
        //unknown of what causes these large readouts. We only want events 
        //that come in coincidence 2, then get read out imediately
        //if(n{bytes > 100 && type != 2 && physicsEvent > 1)
        //{
        //  cout << "occurs at " << physicsEvent << endl;
        //  cout << "nbytes " << nbytes << " " << nbytes2 << " " << type << " " << type2 << endl;
        //  continue;
        //}


        if (type == 30) //type 30 gives physics information
        {
          NphysicsEvent++;

          //If it's the first physics event, skip and advance by the length - 8
          if (NphysicsEvent == 1)
          {
            evtfile.ignore(nbytes-hBufferBytes);
            continue;
          }

          //eventually unpack the point


          //Send the ifstream pointer to the unpacker
          bool stat = Det.unpack(&evtfile, nbytes-8);
          if (!stat) break;
        }
        else if (type == 20)
        {
          NscalerBuffer++;
        }
        else if (type == 31)
        {
          //cout << "I don't know what this is yet. please help me." << endl;
        }
        else if (type == 12)
        {
          cout << "ring format" << endl; 

          //Skip 8 bytes
          evtfile.ignore(8);
        }
        else if (type == 1)  //begin run 
        {
          evtfile.ignore(20); //Jump to file number
          unsigned short runbuf[1];
          evtfile.read((char*)runbuf,2);
          runno = runbuf[0]; //The run number is stored in the first part of the buffer header
          
          cout << "run number = " << runno << ", should match " << runnum << endl;  
          evtfile.ignore(11); //Jump over some stuff

          //Read in 20 16 bit segments
          unsigned short titlebuf[20];
          evtfile.read((char*)titlebuf,40);

          //title can have 40 charecters
          //Converts unsigned shorts to ascii characters
          for (int ii=0;ii<20;ii++) 
          {
            unsigned short  cc = titlebuf[ii];
            unsigned short cc1 = cc&0xFF;
            unsigned short cc2 = cc>>8;
            cout << char(cc1) << char(cc2);        
          }
          cout << endl;

          evtfile.ignore(68); //Jump over a bunch of zeros and a nonsense event type.        

        }
        else if (type == 2)
        {
          cout << "got type == 2, flag for end of run" << endl;
          break;
        }
        else if (type == 3)
        {
          Npauses++;
        }
        else if (type == 4)
        {
          Nresumes++;
        }
        else if (type == 42)
        {

        }
        else
        {
          cout << " unknown event type " << type << " found" << endl;
          break;
        }
        cnt++;

        //if (cnt == 3) abort();

      }//loop over items in a evtfile

      evtfile.close();
      evtfile.clear(); //clear event status in case we had a bad file
    }//loop over split event file
  }//loop over number.beam values


  cout << endl;
  cout << "debug counters" << endl;
  cout << "Number of analyzed events = " << Det.counter << endl;
  cout << "Number of events with two fibers = " << Det.cntTwoFiber << endl;
  cout << "Number of events with two fibers and the adc = " << Det.cntTwoFiberADC << endl;
  cout << "Number of cntNoAdc = " << Det.cntNoAdc<< "  ratio:" << (float)Det.cntNoAdc/(float)Det.counter << endl;;

  t = clock() - t;
  cout << "run time: " << (float)t/CLOCKS_PER_SEC/60 << " min" << endl;

  delete Histo;

  int roll = rand() % (20 - 1 + 1) + 1;
  string line,line2;
  line = R"(          _-_.      )";
  cout << line << endl;
  line = R"(     _-',^. `-_.    )";
  cout << line << endl;
  line = R"( ._-' ,'   `.   `-_ )";
  cout << line << endl;
  line = R"(!`-_._________`-':::)";
  cout << line << endl;
  line = R"(!   /\        /\::::)";
  cout << line << endl;
  if (roll >= 10)
    line = R"(;  /  \  )";
  else
    line = R"(;  /  \   )";
  line2 = R"(  /..\:::)";
  cout << line << roll << line2 << endl;
  line = R"(! /    \    /....\::)";
  cout << line << endl;
  line = R"(!/      \  /......\:)";
  cout << line << endl;
  line = R"(;--.___. \/_.__.--;;)";
  cout << line << endl;
  line = R"( '-_    `:!;;;;;;;' )";
  cout << line << endl;
  line = R"(    `-_, :!;;;''    )";
  cout << line << endl;
  line = R"(        `-!'        )";
  cout << line << endl;
  if (roll == 20) { cout << "Nat 20!" << endl;}
  else if (roll == 1) { cout << "Nat 1 :(" << endl;}

  return 0;
}
