#!/bin/bash

# sposta i file generati nelle cartelle appropriate
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

	# sposto i file
	# aggiungere 'data.dat'!
	mv --verbose --update {var,sdom}.jpeg ${vs} data.dat ac_times.dat --target-directory=${DIR}

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


function main {
	#################################################################
	#
	#  ACQUISIZIONE DATI
	#
	#################################################################

	################################################################
	# XXX Da aggiornare! sicuramente non funziona! 
	#
	# Creo il file header con i parametri inseriti qui
	create_global
	# Compilo il programma e lo eseguo
	make -j3 && ./reticolo-2d >> risultati_MAIN.dat
	################################################################

	# creo una directory di lavoro temporanea e mi ci sposto
	mkdir --parents --verbose ./temp
	# copio i binari per l'analisi dei dati nella cartella temporanea
	cp --update --verbose --archive ../{analisi,fit,make_plot.plt} \
		--target-directory=./temp

	# mi sposto nella cartella temporanea
	cd ./temp
	echo -e "\n > Mi sposto in '`pwd`'\n"

	# copio il file di dati (dev'essere stato generato)
	cp --archive --verbose --update ../data.dat .  # ${DIR}${MAIN} .
# 	head --lines=1000 ${DIR}${MAIN} > ./data.dat


	#################################################################
	#
	#  ANALISI DATI
	#
	#################################################################

	vs_plot 30
	gnuplot make_plot.plt
 	move

	# elimino la cartella temporanea
	cd .. && rm --recursive --verbose --force ./temp
}


# file da cui estrarre i dati
MAIN="data.dat"
# file dei risultati
R="../risultati.dat"
# file temporaneo
tmp="./tmp.dat"

# dichiaro il nome del file di output
vs="./var_sdom.dat"
#####################################################################
