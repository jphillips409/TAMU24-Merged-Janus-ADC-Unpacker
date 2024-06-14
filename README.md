# TAMU24-Merged-Janus-ADC-Unpacker
Code to unpack and analyze events from a merged that contains information from a CAEN ADC and the CAEN 5202 SiPM controller (Janus software). Used at Texas A&amp;M in 2024 to test scintillating fibers coupled to silicon photomultipliers.

Hello intrepid scientists, today you undertake the most difficult challenge of your career:

  Understanding my code.

But fear not, with this guide in hand you will be safe and secure on your journey to unpacked and analyzed data.

All of the cpp and header files should be placed with a subdirectory called "src" which should be inside the directory that contains the Makefile. Unless, of course, you want to edit the Makefile to have the cpp and header files somewhere else.

This code has 5 components:

  sort.cpp  

  The main file. This will open up each event file and begin the unpacking process. To "sort" a specific run number, type ./sort #   into the terminal while in /SiPMFiberSort/. It can also sort a list of run numbers. Make sure to change your directory path to whatever you are using.

  det.cpp

  This is the workhorse of the program. It oversees the unpacking, sorts different detectors, and analyzes the data. It is also where the histograms are filled and gates can be applied to data.

  sipm.cpp

  This program unpacks the Janus data from the merged data file. This data comes from silicon photomultipliers activated by scintillating fibers. There are two sets of 64 fibers, oriented as x and y to create a grid. The data (board #, channel (fiber) #, time of arrival (TOA), and time over threshold (TOT)) from them is unpacked and sorted into three array types. There is a set of overall arrays that hold the data in the same order as they arrive in the data stream. Then there are two sets, one for each board, that allow you to separate the xy grid and apply gates, such as only using the x and y fibers that have the highest TOT. These arrays can be handled for data analysis in det.cpp

  caen.cpp

  This program unpacks the data from the CAEN ADC in the VME crate. It will also organize the data into arrays that be then be handled in det.cpp

  histo.cpp

  This program defines all of the directories and histograms that will be in the output root file. By default, there are histograms for individual fibers, summaries for different fiber boards, and xy hit maps for the fibers. All of these can be changed and new histograms can be added.
