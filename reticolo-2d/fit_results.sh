#!/bin/bash 
#===============================================================================
#
#          FILE: fit.sh
# 
#         USAGE: ./fit.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Paolo Di Gigilio (p.digiglio@gmail.com), 
#  ORGANIZATION: 
#       CREATED: 03/10/2013 23:26
#      REVISION:  ---
#===============================================================================

# Treat unset variables as an error
set -o nounset

# creo la funzione contenente i parametri d'interpolazione
function create_header () {
	clear
	echo "Creo un nuovo header con parametri:"
	echo "B ${B} STOP ${STOP}"

	# creo l'header
	echo "/* definisco alcuni parametri d'interpolazione */
#ifndef  pars_INC
#define  pars_INC

/* ordine di arresto nel calcolo della somma 'S' */
#define STOP ${STOP}
/* fisso il valore di $\beta$ */
#define B ${B}
/* valore del coefficiente $\gamma_E$ della funzione beta_0 */
#define G 1.287142

#endif   /* ----- #ifndef pars_INC  ----- */" > ./fit_pars.h
}

#####################################################################
#
# Definisco i parametri
#
STOP=40
#B=
#####################################################################

for (( B = 5; B < 16; B += 5 )); do
	# Creo un header con i parametri corretti
	create_header
	# Compilo i sorgenti
	make fit
	for i in cu cs en; do
		# interpolo i tre grafici
		echo "#Osservabile: ${i} B: ${B}" >> fit_pars.dat
		./fit_${i} ./results/${i}_B${B}.dat >> fit_pars.dat

		# sposto i risultati per evitare sovrapposizioni
		for ext in svg pdf; do
			mv --update --verbose ${i}.${ext} ./results/${i}_B${B}.${STOP}.${ext}
		done
	done
	# Pulisco la cartella dai file oggetto
	make clean
done

exit $?
