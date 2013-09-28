#!/bin/bash

# questa funzione serve per creare i grafici della varianza e della
# SDOM (assicurati di aver modificato opportunamente "analisi.cpp").
# Accetta come argomenti solo gli affissi e l'ordine di arresto.
function vs_plot {
	# controllo se il file temporaneo esiste cancello il contenuto
	if [[ -e ${OUT} ]]
	then
		# controllo se il ${MAIN} è più vecchio dell' ${OUT}
		if [[ `stat -c %Z ${MAIN}` < `stat -c %Z ${OUT}` ]]
		then
			echo "Il file '${OUT}' è più recente del file '${MAIN}'!"
			# in questo caso non ha senso ri-analizzare i dati, per 
			# cui esco dalla funzione
			return
		else
			# avverto l'utente che sto per sovrascrivere
			echo "################################################"
			echo "#"
			echo "# Trovato vecchio file '${OUT}': lo sovrascrivo."
			echo "#"
			echo "################################################"
			echo
		fi
	fi

	# Creo i titoli per le colonne
	echo -e "#ord\t\tvv_us\t\tv_us\t\tv_ss\t\ts_ss\t\tv_en\t\ts_en" > ${OUT}

	# salvo i risultati in un file
	for (( k = 1; k <= ${1}; k ++ ))
	do
		echo "Creazione del file '${OUT}': riga ${k} / ${1}"
		./analisi ${MAIN} ${k} >> var_sdom.dat
		
		# controllo se il programma ha creato il grafico
		# dell'autocorrelatore
		if [[ -e ac.dat ]]
		then
			# modifico il nome del file contenente l'autocorrelatore che 
			# viene prodotto dal programma "./analisi"
			mv --verbose --update ac.dat ac_${k}.bak.dat
			# creo un file con l'autocorrelatore troncato a 10
			head --lines=11 ac_${k}.bak.dat > ac_${k}.dat
			# interpolo i grafici degli autocorrelatori
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
	head --lines=2 ${DIR}ac_times.dat >> tempi_auto-corr.dat
	
	# aggiungo le informazioni nei file
	echo "# B = ${B}; N = ${N}; M = ${M} sweep ${SWEEP}" >> ${OUT}
}

# argomenti c.s.
function move {
	echo
	echo "##########################################################"
	echo "Sposto i file "
	echo "##########################################################"
	echo

	# controllo se ${DIR} esiste
	if [[ !( -d ${DIR} ) ]]
	then
		echo -e "\n##########################"
		mkdir --parents --verbose ${DIR}
		echo -e "##########################\n"
	fi

	# sposto il file
	mv --verbose --update {var,sdom}.jpeg data.dat ${OUT} ac_times.dat --target-directory=${DIR}

	# sposto dati e grafici degli autocorrelatori
	for (( i = 1; i <= 30; i ++ ))
	do
		for ext in jpeg dat
		do
			mv --verbose --update ac_${i}{,.bak}.${ext} --target-directory=${DIR}
		done
		# sposto i fit degli autocorrelatori
		mv --verbose --update ac_${i}_{en,ss,us}.{pdf,svg} --target-directory=${DIR}
	done
}

# crea il file "./global.h" con i parametri definiti qui
function create_global {
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

function main {
	#################################################################
	#
	#  ACQUISIZIONE DATI
	#
	#################################################################

	cp -av ${DIR}${MAIN} .
	# Creo il file header con i parametri inseriti qui
# 	create_global
	# Compilo il programma e lo eseguo
# 	make -j3 && ./reticolo_2d >> risultati.dat

	#################################################################
	#
	#  ANALISI DATI
	#
	#################################################################

	vs_plot 30
	gnuplot make_plot.plt
 	move

	# ritorna il valore d'uscita dell'ultimo comando
#	exit $?
}

#####################################################################
#
# PARAMETRI DEL SISTEMA
#
#####################################################################
B=1
J=1
N=32
M=256
SWEEP=2000000

# "radice" del percorso in cui spostare i file
root="/home/paolo/Pubblici/Dropbox/tesi/data/2d/"
# file da cui estrarre i dati
MAIN="data.dat"

# dichiaro il nome del file di output
OUT="var_sdom.dat"
#####################################################################


# Per N = 32 siti
for (( m = 1; m < 10; m ++))
do
	let M=$[$m*32]
	# percorso in cui spostare i file
	DIR=${root}"B${B}.N${N}.M${M}/"
	main
	echo ${DIR}
done

# Per N = 128 siti
let N=128

#####################################################################
let B=2
let M=16
# percorso in cui spostare i file
DIR=${root}"B${B}.N${N}.M${M}/"
main
#####################################################################
let B=4
let M=32
# percorso in cui spostare i file
DIR=${root}"B${B}.N${N}.M${M}/"
main
#####################################################################
let B=8
let M=64
# percorso in cui spostare i file
DIR=${root}"B${B}.N${N}.M${M}/"
main
#####################################################################
let B=16
for m in 16 32 64 128
do
	let M=$m
	# percorso in cui spostare i file
	DIR=${root}"B${B}.N${N}.M${M}/"
	main
done
