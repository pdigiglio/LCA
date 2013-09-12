# File da compilare
MAIN = reticolo_2d 
INCLUDE = global.h

# Librerie aggiuntive che prevedo di inserire
MODULES = 

# Compilatore c++
CXX = g++
# Opzioni cpu-dipendenti
MARCH = core2
MASM = intel
# Standard di linguaggio
STD = c++11
# Opzioni
CXXFLAGS = -Wall -O2 -Wextra -pedantic -march=$(MARCH) -std=$(STD) \
		   -masm=$(MASM) -mtune=$(MARCH) -fopenmp -lm

# % = $(MAIN); la funzione parte solo 
# se esistono $(MAIN).cpp e 'Makefile'
$(MAIN): %: %.cpp %.cc %.h Makefile ${INCLUDE}
	@ echo '#INFO'
	@ echo 'Architettura rilevata:\t\t' ` gcc -march=native -Q --help=target | grep --text march | cut -f3 `
	@ echo -e 'Architettura selezionata:\t' $(MARCH)
	@ echo
	$(CXX) $< -o $@ $(CXXFLAGS)
	
# pulisce la directory
clean:
	@ -rm -rf *.d *.o *.tmp $(MAIN)
.PHONY: clean
