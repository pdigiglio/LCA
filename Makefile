# Main file to compile
MAIN = reticolo-1d
# header to include
INCLUDE = global.h

# Since main files to be compiled names are the same as their containing
# relative path names, I'll edit them as
SRC = ./$(MAIN)/$(MAIN)

# C++ compiler
CXX = g++

# some cpu-dependent options
MARCH = core2
MASM = intel

# standard language
STD = gnu++11 

# compile options
CXXFLAGS = -Wall -O2 -Wextra -pedantic -march=$(MARCH) -std=$(STD) \
		   -masm=$(MASM) -mtune=$(MARCH) -fopenmp -lm

# main function (to compile main source)
$(SRC): %: %.cpp %.cc %.h Makefile ./$(MAIN)/$(INCLUDE)
	@ echo '#INFO'
	@ echo 'Architettura rilevata:\t\t' ` gcc -march=native -Q --help=target | grep --text march | cut -f3 `
	@ echo -e 'Architettura selezionata:\t' $(MARCH)
	@ echo
	$(CXX) $< -o $@ $(CXXFLAGS)
	@cp --verbose --update --archive $@ ./$(MAIN).a
	@ echo

# uncertainly analyzer and auto-correlator data generator
analisi: analisi.cpp
	$(CXX) $< -o $@ $(CXXFLAGS)

# auto-correlator fit routine (requires ROOT)
fit: fit.cpp
	$(CXX) $< `root-config --libs --cflags` -o $@ $(CXXFLAGS)

# results fit routine (requires ROOT)
fit_risultati: fit_risultati.cpp
	$(CXX) $< `root-config --libs --cflags` -o $@ $(CXXFLAGS)

# pulisce la directory
clean:
	@ -rm -rf *.d *.o *.tmp $(MAIN)
.PHONY: clean
