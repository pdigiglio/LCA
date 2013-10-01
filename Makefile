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

# uncertainly analyzer and auto-correlator data generator
analisi: analisi.cpp
	$(CXX) $< -o $@ $(CXXFLAGS)

# auto-correlator fit routine (requires ROOT)
fit: fit.cpp
	$(CXX) $< `root-config --libs --cflags` -o $@ $(CXXFLAGS)

# clean directory
clean:
	@ rm --recursive --verbose fit analisi
.PHONY: clean
