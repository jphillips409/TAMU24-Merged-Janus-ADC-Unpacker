SRC = src
BIN = bin

#list source manually to exclude sim.cpp and simmulti.cpp
SOURCE = det.cpp histo.cpp caen.cpp sipm.cpp
#$(patsubst pattern,replacement,text), this variable has all the object files in directory bin/filename.o
#substitution reference $(var:pattern=replacement)
OBJECT = $(patsubst %, $(BIN)/%, $(notdir $(SOURCE:.cpp=.o)))

CC = g++
CFLAGS= -c -ggdb -std=c++14 -I$(shell root-config --incdir)
LINKOPTION = $(shell root-config --libs) 

sort : $(BIN)/sort.o $(OBJECT)
	@echo "Linking..."
	$(CC) -o $@ $^ $(LINKOPTION)
	@echo "Finished"
	
$(BIN)/%.o : $(SRC)/%.cpp
	@echo "Compiling..."
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(BIN)/*.o
	rm -f sort

print:
	@echo $(OBJECT)
	@echo $(SOURCE)
	
