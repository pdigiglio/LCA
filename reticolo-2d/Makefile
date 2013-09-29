# File (principale) da compilare
MAIN = reticolo-2d
INCLUDE = global.h 

# Librerie aggiuntive che prevedo di inserire
#MODULES = 

# Compilatore c++
CXX = g++
# Opzioni cpu-dipendenti
MARCH = core2
MASM = intel
# Standard di linguaggio
STD = c++11
# Opzioni
CXXFLAGS = -Wall -O2 -Wextra -pedantic -march=$(MARCH) -std=$(STD) \
		   -masm=$(MASM) -mtune=$(MARCH) -lm -fopenmp

# % = $(MAIN); la funzione parte solo 
# se esistono $(MAIN).cpp e 'Makefile'
$(MAIN): %: %.cpp %.cc %.h Makefile $(INCLUDE)
	@ echo '#INFO'
	@ echo 'Architettura rilevata:\t\t' ` gcc -march=native -Q --help=target | grep --text march | cut -f3 `
	@ echo -e 'Architettura selezionata:\t' $(MARCH)
	@ echo
	$(CXX) $< -o $@ $(CXXFLAGS)
	@ echo
	
# compilo il programma per interpolare i grafici
fit: fit.cpp
	$(CXX) $< `root-config --libs --cflags` -o $@ $(CXXFLAGS)

# compilo il programma per interpolare i grafici
fit_risultati: fit_risultati.cpp
	$(CXX) $< `root-config --libs --cflags` -o $@ $(CXXFLAGS)

# compilo il programma per interpolare i grafici
analisi: analisi.cpp
	$(CXX) $< -o $@ $(CXXFLAGS)

# pulisce la directory
clean:
	@ rm --recursive --force --verbose *.o *.tmp
	@ rm --verbose --interactive=always risultati.dat lunghezze.dat
.PHONY: clean
	
