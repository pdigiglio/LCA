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
	echo "# PARAMETRI: B ${B} J ${J} N ${N} M ${M}"

	# creo l'header
	echo "/* File header contenente i parametri del sistema */

#ifndef  global_H
#define  global_H

#define N ${N}	/* numero atomi */
#define M ${M}	/* 2 * numero di Trotter */

#define J ${J}
#define B ${B}

#define MSR ${SWEEP}

#endif   /* ----- #ifndef global_H  ----- */" > ./global.h
}

# questa funzione serve per creare i grafici della varianza e della
# SDOM (assicurati di aver modificato opportunamente "analisi.cpp").
# Accetta come argomenti solo gli affissi e l'ordine di arresto.
function vs_plot {
	# Creo i titoli per le colonne
	echo -e "#ord\t\tvv_us\t\tv_us\t\tv_ss\t\ts_ss\t\tv_en\t\ts_en" > ${vs}

	echo -e "# Sweep: ${SWEEP}" >> ${R}
	echo -e "#J\tB\tN\tM\tUS\t\terr\t\tSS\t\terr\t\tEN\t\terr" >> ${R}

	# salvo i risultati in un file
	for (( k = 1; k <= ${1}; k ++ ))
	do
		echo "Creazione del file '${tmp}': riga ${k} / ${1}"
		echo -e "${J}\t${B}\t${N}\t${M}\t`./analisi ${MAIN} ${k}`" >> ${tmp}
		
		# controllo se il programma ha creato il grafico
		# dell'autocorrelatore
		if [[ -e ac.dat ]]
		then
			# modifico il nome del file contenente l'autocorrelatore che 
			# viene prodotto dal programma "./analisi"
			mv --verbose --update ac.dat ac_${k}.bak.dat
			# creo un file con l'autocorrelatore troncato a 10
			head --lines=11 ac_${k}.bak.dat > ac_${k}.dat

			echo -e "#ord\tJ\tB\tN\tM\t##parametri fit" >> ac_times.dat
			echo  -e "${k}\t${J}\t${B}\t${N}\t${M}\t`./fit ac_${k}.dat`" >> ac_times.dat
			
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
	cut --fields=1,2,3,4,12,13,14,15,16,17 ${tmp} | head --lines=1 >> ${R}
	cut --fields=5,6,7,8,9,10,11 ${tmp} >> ${vs}	

	# aggiungo le informazioni nei file
	echo "# B = ${B}; N = ${N}; M = ${M} sweep ${SWEEP}" >> ${vs}
}

#####################################################################
#
# SYSTEM PARAMETERS
#
#####################################################################
B=1
J=1
N=32
M=32

SWEEP=20000

# "root" of the path to move file in
root="/home/paolo/Pubblici/Dropbox/tesi/data/2d/"
#####################################################################

STOP=20
# genero il file contenente le lunghezze
for (( b = 1; b < 4; b ++ )); do
	# modifico il valore di $\beta$
	let B=${b}
	for (( m = 1; m <= ${STOP}; m ++ )); do
		let M=$[ 16 * ${m} * ${b} ]
		create_global
		make && ./reticolo-1d
	done
# 	tail --lines=$[ 2 * ${STOP} ] ./lunghezze.dat > ./lunghezze_B${B}.dat
# 	echo "SWEEP ${SWEEP}" >> lunghezze_B${B}.dat
done

# # Per N = 32 siti
# for (( m = 1; m < 10; m ++ ))
# do
# 	let M=$[ $m * 32 ]
# 	# percorso in cui spostare i file
# 	DIR=${root}"B${B}.N${N}.M${M}/"
# 	main
# done
# 
# # Per N = 128 siti
# let N=128
# 
# #####################################################################
# let B=2
# let M=16
# # percorso in cui spostare i file
# DIR=${root}"B${B}.N${N}.M${M}/"
# main
# #####################################################################
# let B=4
# let M=32
# # percorso in cui spostare i file
# DIR=${root}"B${B}.N${N}.M${M}/"
# main
# #####################################################################
# let B=8
# let M=64
# # percorso in cui spostare i file
# DIR=${root}"B${B}.N${N}.M${M}/"
# main
# #####################################################################
# let B=16
# for m in 16 32 64 128
# do
# 	let M=$m
# 	# percorso in cui spostare i file
# 	DIR=${root}"B${B}.N${N}.M${M}/"
# 	main
# done
