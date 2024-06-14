#include "det.h"

//Johnathan Phillips 2024_06_10
//Class written to handle 128 scintillating fibers, a 0 degree plastic scintillator, and a TAC

//using namespace std;
using std::cout;
using std::endl;


det::det(histo * Histo1)
{


  Histo = Histo1;


  ADC = new caen();
  Janus = new sipm();
}

det::~det()
{
  cout << "start det destr" << endl;
  delete ADC;
  cout << "end det destr" << endl;
  
}

bool det::unpack(ifstream *point, int evtsize)
{

  //Reset solution counters
  ADC->NSolutions = 0;

  Janus->NSolutions = 0;
  Janus->NB0 = 0;
  Janus->NB1 = 0;

  counter++;
  bool stat = true;

  //*****************************************************************
  //Built Physics Event Header

  //TODO  Use typecasting to read it in automatically

  //Read in physics event header body size
  int32_t BuiltPhysHeaderSize[1];
  point->read((char*)BuiltPhysHeaderSize,4);

  //Read in the event timestamp, 64 bit
  int64_t EvtTStamp[1];
  point->read((char*)EvtTStamp,8);
  int EventTimeStamp = EvtTStamp[0];

  //Read in built event source ID (should be 10) and barrier type (should be 0)
  int32_t BuiltPhysSource_Barr[2];
  point->read((char*)BuiltPhysSource_Barr,8);
  int32_t BuiltPhysSource = BuiltPhysSource_Barr[0];
  int32_t BuiltPhysBarr = BuiltPhysSource_Barr[1];

  //cout << "here " << BuiltPhysSource << " " << BuiltPhysBarr << endl;

  //Skip if the built phys source and barrier are wrong
  if (BuiltPhysSource != 10 || BuiltPhysBarr != 0)
  {
    cout << "Bad merged source ID or barrier type" << endl;
    point->ignore(evtsize - BuiltPhysHeaderSize[0]);
    return false;
  }

  //*****************************************************************
  //Built Physics Event Body
  //Maximum number of fragments: 3 (2 from janus, one from ADC)
  //Each fragment is identified by the source id, must be chosen for janus ring buffer and wudaq ring buffer
  //Check if we have reached the end of the body size after each fragment is processed

  //Read in body size
  int32_t BuiltPhysBodySize[1];
  point->read((char*)BuiltPhysBodySize,4);

  int BodySize = BuiltPhysBodySize[0]; //use this to check if we have more fragments
  int SizeCount = sizeof(BuiltPhysBodySize[0]); //Use this to check fragment iteration

  //TODO Compare body size to the event size - event header size

  //Fragment loop, go until end of body size
  for (;;)
  {
    //Get the first fragment timestamp
    int64_t FragTimeStamp[1];
    point->read((char*)FragTimeStamp,8);
    long int FragTStamp = FragTimeStamp[0];
    SizeCount += sizeof(FragTimeStamp[0]);

    //Get first fragment source id, must be chosen for the janus ring buffer and the wudaq ring buffer
    int32_t FragSourceID[1];
    point->read((char*)FragSourceID,4);
    int SourceID = FragSourceID[0];
    SizeCount += sizeof(FragSourceID[0]);

    //Skip past repeat payload size and barr type
    point->ignore(8);
    SizeCount += 8;

    //Get payload size, includes skips from this point
    int32_t FragPaySize[1];
    point->read((char*)FragPaySize,4);
    int PayloadSize = FragPaySize[0];
    SizeCount += PayloadSize; //Skip the entire payload size for the size counting variable

    //Skip past repeat variables. Size should remain constant
    int repeatvars = 24;
    point->ignore(repeatvars);

    //Pick which unpacker you need, assuming 1 for ADC, 0 for Janus
    if (SourceID == 1) //CAEN ADC unpacker, 16 bit chunks
    {
      //Read out the ADC buffer in 16 bit chunks and pass its pointer to the caen unpacker
      int CAENBufferBytes = PayloadSize - repeatvars - sizeof(FragPaySize[0]);
      int CAENBufferWords = CAENBufferBytes/2;
      unsigned short CAENBuffer[CAENBufferWords];
      point->read((char*)CAENBuffer,CAENBufferBytes);

      unsigned short * pbuf = CAENBuffer;
      ADC->read(pbuf);

      ADC->TimeStamp = FragTStamp;

      //Debugging line. Lets you skip past each event using its size.
      //point->ignore(FragPaySize[0]-28);
    }

    if (SourceID == 0) //Janus unpacker, 8 bit chunks
    {
      //Pass the ifstream pointer to Janus to unpack
      Janus->read(point,PayloadSize);

      Janus->TimeStamp = FragTStamp;

      //Debugging line for skipping events.
      //point->ignore(FragPaySize[0]-28);
    }


    //If you have reached the end of the built physics body, stop unpacking
    if (SizeCount == BodySize) break;
    
  }

  //Calibrate the ADC channels, just do it manually here
  ADC->data[0] = 1. * ADC->data[0];   //0 degree scintillator
  ADC->data[1] = 1. * ADC->data[1];   //TAC


  if (ADC->NSolutions > 0)
  {
    Histo->ADCEnergy_R[0]->Fill(ADC->data[0]);
    Histo->ADCEnergy_R[1]->Fill(ADC->data[1]);
  }

  //Histograms the raw fiber data, no coincidence requirements
  for (int i=0;i<Janus->NB0;i++)
  {
    Histo->B0TOA_R[Janus->channel_B0[i]]->Fill(Janus->TOA_B0[i]);
    Histo->B0TOT_R[Janus->channel_B0[i]]->Fill(Janus->TOT_B0[i]);

    Histo->FibervsTOA_Board0->Fill(Janus->channel_B0[i],Janus->TOA_B0[i]);
    Histo->FibervsTOT_Board0->Fill(Janus->channel_B0[i],Janus->TOT_B0[i]);;
  }
  for (int i=0;i<Janus->NB1;i++)
  {
    Histo->B1TOA_R[Janus->channel_B1[i]]->Fill(Janus->TOA_B1[i]);
    Histo->B1TOT_R[Janus->channel_B1[i]]->Fill(Janus->TOT_B1[i]);

    Histo->FibervsTOA_Board1->Fill(Janus->channel_B1[i],Janus->TOA_B1[i]);
    Histo->FibervsTOT_Board1->Fill(Janus->channel_B1[i],Janus->TOT_B1[i]);
  }

  //These lines only apply if you have fibers fire on board 0 and 1. End event analysis if not
  if (Janus->NB0 == 0 || Janus->NB1 == 0) return true;
  
  cntTwoFiber++;

  int xfiber;
  int yfiber;

  //Plots Fiber vs Fiber for a raw x-y hitmap
  for (int i=0;i<Janus->NB0;i++)
  {
    xfiber = Janus->channel_B0[i];

    for (int j=0;j<Janus->NB1;j++)
    {
      yfiber = Janus->channel_B1[j];

      Histo->FiberHitMap_R->Fill(xfiber,yfiber);
    }
  }

  //Sets channel num and TOT to first in array
  xfiber = Janus->channel_B0[0];
  double xTOT = Janus->TOT_B0[0];
  yfiber = Janus->channel_B1[0];
  double yTOT = Janus->TOT_B1[0];

  //Gates on the highest TOT x-y fiber pair
  for (int i=1;i<Janus->NB0;i++)
  {
    if (Janus->TOT_B0[i] > xTOT)
    {
      xfiber = Janus->channel_B0[i];
      xTOT = Janus->TOT_B0[i];
    }
    else continue;
  }

  for (int i=1;i<Janus->NB1;i++)
  { 
    if (Janus->TOT_B1[i] > yTOT)
    {
      yfiber = Janus->channel_B1[i];
      yTOT = Janus->TOT_B1[i];
    }
    else continue;
  }

  //Fills a hist with the highest x-y fiber pair
  Histo->FiberHitMap_Highest->Fill(xfiber,yfiber);

  //Should have two ADC channels for a correct coincident event - 0 degree scinttilator and TAC
  //Gating on 0 degree scintillator coincidence
  if (ADC->NSolutions < 2) return true;

  cntTwoFiberADC++;

  return true;
}
