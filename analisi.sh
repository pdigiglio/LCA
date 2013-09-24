#!/bin/bash

# questa funzione serve per creare i grafici della varianza e della
# SDOM (assicurati di aver modificato opportunamente "analisi.cpp").
# Accetta come argomenti solo gli affissi e l'ordine di arresto.
function vs_plot {
	# controllo se il file di output esiste
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
			# Se il file esiste e non è più vecchio del file ${MAIN}
			# contenente i dati, avverto l'utente della sovrascrittura
			echo "################################################"
			echo "#"
			echo "# Trovato vecchio file '${OUT}': lo sovrascrivo."
			echo "#"
			echo "################################################"
			echo
		fi
	fi

	# Creo i titoli per le colonne
	echo -e "#ord\t\tv_us\t\ts_us\t\tv_ss\t\ts_ss\t\tv_en\t\ts_en" > ${OUT}

	# salvo i risultati nel file ${OUT}
	for (( k = 1; k <= ${1}; k ++ ))
	do
		# creo il file di coordinate dei grafici degli errori
		echo "Creazione del file '${OUT}': riga ${k} / ${1}"
		./analisi.a ${MAIN} ${k} >> var_sdom.dat
		
		# controllo se esiste il grafico dell'autocorrelatore
		if [[ -e ac.dat ]]
		then
			# modifico il nome del file contenente l'autocorrelatore 
			# (prodotto da "./analisi.a") per evitare riscritture
			mv --verbose --update ac.dat ac_${k}.bak.dat

			# creo un file con l'autocorrelatore troncato a 10
			head --lines=11 ac_${k}.bak.dat > ac_${k}.dat

# 			# aggiungo le informazioni nei file degli a-c
#			# XXX ROOT potrebbe dare problemi con "formattazioni" strane
# 			echo "# B = ${B}; N = ${N}; M = ${M} sweep ${SWEEP}" >> ac_${k}{.bak,}.dat

			# interpolo i grafici degli autocorrelatori e salvo i tempi
			# di auto-correlazione (in funzione della dimensione dei
			# bin in cui sono suddivise le misure) in un file
			echo -e "#ord\tJ\tB\tN\tM\t##tempi a-c (us, ss, en)" >> ac_times.dat
			echo -e "${k}\t${J}\t${B}\t${N}\t${M}\t`./fit.a ac_${k}.dat`" >> ac_times.dat
			
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
			# viene prodotto dal programma "./analisi.a"
			mv --verbose --update ms_cluster.dat ms_cluster_${k}.dat
		else
			echo "File 'ms_cluster.dat' non esistente: proseguo."
		fi
	done
	
	# Salvo i tempi di auto-correlazione (con dimensione bin = 1)
	head --lines=2 ${DIR}ac_times.dat >> tempi_auto-corr.dat
	
	# aggiungo le informazioni nel file degli errori
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
		echo -e "\n###################################################"
		mkdir --parents --verbose ${DIR}
		echo -e "####################################################\n"
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

function main {
	#################################################################
	#
	#  ACQUISIZIONE DATI
	#
	#################################################################

# 	cp -av ${DIR}${MAIN} .
	clear
	# Creo il file header con i parametri inseriti qui
	create_global
	# Compilo il programma e lo eseguo
	make -j3 && ./reticolo-2d.a >> risultati.dat

	#################################################################
	#
	#  ANALISI DATI
	#
	#################################################################

	# analisi dati
	vs_plot 30
	# creo i grafici
	gnuplot make_plot.plt
	# sposto i file nelle cartelle appropriate
 	move
}

#####################################################################
#
# PARAMETRI DEL SISTEMA
#
#####################################################################
B=5
J=1
N=6
M=6
T=256
SWEEP=500000

# "radice" del percorso in cui spostare i file
root="/home/paolo/Pubblici/Dropbox/tesi/data/3d/"
# file da cui estrarre i dati
MAIN="data.dat"

# dichiaro il nome del file di output
OUT="var_sdom.dat"
#####################################################################


# compilo le routine per analisi errori e interpolazione
make fit
make analisi

# imposto il ciclo per la generazione dei dati
for (( i = 1; i < 5; i ++ ))
do
	# calcolo il valore di $\beta$
	let B=$[ ${i} * 5 ]
	# calcolo il numero di fette temporali
	let T=$[ ${i} * 256 ]

	# faccio partire la generazione dei dati
	for (( m = 0; m < 10; m ++ ))
	do
		let N=$[ 2 * ${m} + 4 ]
		let M=${N}
		# percorso in cui spostare i file
		DIR=${root}"B${B}.N${N}.M${M}.T${T}/"

		# avvio la generazione ed analisi dei dati
		main
	done
done

# Serve per eliminare i commenti dal file produrre una tabella dati
# leggibile per ROOT
# sed -e '/^#/d' ./risultati.dat >> ?
