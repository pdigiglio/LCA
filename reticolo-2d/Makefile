# Main file to compile
MAIN = reticolo-1d
# header to include
INCLUDE = global.h

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
$(MAIN): %: %.cpp %.cc %.h Makefile $(INCLUDE)
	@ echo '#INFO'
	@ echo 'Architettura rilevata:\t\t' ` gcc -march=native -Q --help=target | grep --text march | cut -f3 `
	@ echo -e 'Architettura selezionata:\t' $(MARCH)
	@ echo
	$(CXX) $< -o $@ $(CXXFLAGS)
	@ echo

# remove $(MAIN) binary file
clean:
	@ rm --recursive --verbose $(MAIN)
.PHONY: clean
