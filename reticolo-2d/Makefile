# Main file to compile
MAIN = reticolo-2d
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

# fit routines for analyzing results (requires ROOT)
fit: fit_cu fit_cs fit_en

fit_cu: fit_cu.o
	$(CXX) `root-config --libs --cflags` -o $@ $< $(CXXFLAGS)

fit_cu.o: fit_cu.cpp fit_functions.cc fit_pars.h
	$(CXX) `root-config --libs --cflags` -c $< $(CXXFLAGS)

fit_cs: fit_cs.o
	$(CXX) `root-config --libs --cflags` -o $@ $< $(CXXFLAGS)

fit_cs.o: fit_cs.cpp fit_functions.cc fit_pars.h
	$(CXX) `root-config --libs --cflags` -c $< $(CXXFLAGS)

fit_en: fit_en.o
	$(CXX) `root-config --libs --cflags` -o $@ $< $(CXXFLAGS)

fit_en.o: fit_en.cpp fit_functions.cc fit_pars.h
	$(CXX) `root-config --libs --cflags` -c $< $(CXXFLAGS)

# remove $(MAIN) binary file
clean:
	@ rm --recursive --verbose *.o #$(MAIN) fit_cu fit_en fit_cs
.PHONY: clean
