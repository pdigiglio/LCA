#####################################################################
#
# OPZIONI GENERALI
#
#####################################################################

# abilita la legenda
set key

# abilita il bordo destro e inferiore
set border 3
# disegna la griglia
set grid

# opzioni per l'output
set terminal jpeg large font "Helvetica,12" size 800,600 

# Font asse y
set ytics font "Helvetica,12"
# Font asse x
set xtics 5 font "Helvetica,12"

set ylabel "Osservabili"
set xlabel "Lato reticolo"

outfile = "risultati_en.jpeg"
set output outfile

plot "./risultati.dat" using 3:10 with linespoint title "densità di energia",\
	 "./risultati.dat" using 3:10:11 with yerrorbars title "SDM d.e."

outfile = "risultati_us.jpeg"
set output outfile

plot "./risultati.dat" using 3:6 with linespoint title "suscettività uniforme",\
	 "./risultati.dat" using 3:6:7 with yerrorbars title "SDM s.u."


outfile = "risultati_ss.jpeg"
set output outfile

plot "./risultati.dat" using 3:8 with linespoint title "suscettività alternata",\
	 "./risultati.dat" using 3:8:9 with yerrorbars title "SDM s.a."

#print " > Gnuplot : Creato il file ".outfile
