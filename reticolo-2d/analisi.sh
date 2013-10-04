#!/bin/bash - 
#===============================================================================
#
#          FILE: analisi.sh
# 
#         USAGE: ./analisi.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Paolo Di Giglio (p.digiglio@gmail.com), 
#  ORGANIZATION: 
#       CREATED: 29/09/2013 08:58
#      REVISION:  ---
#===============================================================================

# Treat unset variables as an error
set -o nounset

# Common functions which work either with 1-d and 2-d lattice
source ../analisi_common.sh

# Create "./global.h" file containing the user-chosen parameters in
# the section below
function create_global {
	clear
	echo "# Creo nuovo header!"
	echo "# PARAMETRI: B ${B} J ${J} N ${N} M ${M} T ${T} SWEEP ${SWEEP}"

	# creo l'header
	echo "/* File header contenente i parametri del sistema */

#ifndef  global_INC
#define  global_INC

#define N ${N}	/* numero atomi asse x */
#define M ${M}	/* numero atomi asse y */
#define T ${T}	/* 4 * numero di Trotter */

#define B ${B}
#define J ${J}

#define MSR ${SWEEP}

#endif   /* ----- #ifndef global_INC  ----- */" > ./global.h
}

# questa funzione serve per creare i grafici della varianza e della
# SDOM (assicurati di aver modificato opportunamente "analisi.cpp").
# Accetta come argomenti solo gli affissi e l'ordine di arresto.
function vs_plot {
	# Creo i titoli per le colonne
	echo -e "#ord\t\tvv_us\t\tv_us\t\tv_ss\t\ts_ss\t\tv_en\t\ts_en" > ${vs}

	echo -e "# Sweep: ${SWEEP}" >> ${R}
	echo -e "#J\tB\tN\tM\tT\tUS\t\terr\t\tSS\t\terr\t\tEN\t\terr" >> ${R}

	# salvo i risultati in un file
	for (( k = 1; k <= ${1}; k ++ ))
	do
		echo "Creazione del file '${tmp}': riga ${k} / ${1}"
		echo -e "${J}\t${B}\t${N}\t${M}\t${T}\t`./analisi ${MAIN} ${k}`" >> ${tmp}
		
		# controllo se il programma ha creato il grafico
		# dell'autocorrelatore
		if [[ -e ac.dat ]]
		then
			# modifico il nome del file contenente l'autocorrelatore che 
			# viene prodotto dal programma "./analisi"
			mv --verbose --update ac.dat ac_${k}.bak.dat
			# creo un file con l'autocorrelatore troncato a 10
			head --lines=11 ac_${k}.bak.dat > ac_${k}.dat

			echo -e "#ord\tJ\tB\tN\tM\tT\t##parametri fit" >> ac_times.dat
			echo  -e "${k}\t${J}\t${B}\t${N}\t${M}\t${T}\t`./fit ac_${k}.dat`" >> ac_times.dat
			
			# rinomino i file creati dall'interpolazione
			for ext in svg pdf
			do
				for t in en ss us
				do
					mv --verbose --update ac_${t}.${ext} ac_${k}_${t}.${ext}
				done
			done
		fi

		# controllo se il programma ha creato il grafico
		# delle misure in funzione della dimensione del cluster
		if [[ -e ms_cluster.dat ]]
		then
			# modifico il nome del file contenente l'autocorrelatore che 
			# viene prodotto dal programma "./analisi"
			mv --verbose --update ms_cluster.dat ms_cluster_${k}.dat
		fi
	done
	
	# Salvo i tempi di auto-correlazione
	head --lines=2 ac_times.dat >> ../tempi_auto-corr.dat
	# Creo i file dei risultati e i grafici degli errori
	cut --fields=1,2,3,4,5,13,14,15,16,17,18 ${tmp} | tail --lines=1 >> ${R}
	cut --fields=6,7,8,9,10,11,12 ${tmp} >> ${vs}	

	# aggiungo le informazioni nei file
	echo "# B = ${B}; N = ${N}; M = ${M}; T = ${T} sweep ${SWEEP}" >> ${vs}
}

#####################################################################
#
# SYSTEM PARAMETERS
#
#####################################################################
B=5
J=1
N=1
T=256

SWEEP=3000

# "root" of the path to move file in
root="/home/paolo/Pubblici/Dropbox/tesi/data/3d/"
#####################################################################

for B in 5 10 15; do
	for (( m = 0; m < 10; m ++ )); do
		let M=$[ $m * 2 + 4 ]
		let N=${M}
		let T=$[ 256 * ${B} / 5 ]
		# percorso in cui spostare i file
		DIR=${root}"B${B}.N${N}.M${M}.T${T}/"
		main
	done
done
