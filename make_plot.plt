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


#####################################################################
#
# AUTOCORRELATORI
#
#####################################################################

set ylabel "Autocorrelatori"
set xlabel "Tempo markoviano"

do for [t=1:30] {
	outfile = sprintf('ac_%d.jpeg', t)
	set output outfile

	titlefile = sprintf('Dimensione cluster: %d', t)
	set title titlefile

	plot "./ac_".t.".dat" using 1:2 with linespoint title "suscettività uniforme",\
		 "./ac_".t.".dat" using 1:2:3 with yerrorbars title "SDM s.u.",\
		 "./ac_".t.".dat" using 1:4 with linespoint title "suscettività alternata",\
		 "./ac_".t.".dat" using 1:4:5 with yerrorbars title "SDM s.a.",\
		 "./ac_".t.".dat" using 1:6 with linespoint title "densità di energia",\
		 "./ac_".t.".dat" using 1:6:7 with yerrorbars title "SDM d.e."

	print " > Gnuplot : Creato il file ".outfile
}

do for [x=1:30] {
	outf = sprintf('ac_%d.bak.jpeg', x)
	set output outf

	titlef = sprintf('Dimensione cluster: %d', x)
	set title titlef

	plot "./ac_".x.".bak.dat" using 1:2 with linespoint title "suscettività uniforme",\
		 "./ac_".x.".bak.dat" using 1:2:3 with yerrorbars title "SDM s.u.",\
		 "./ac_".x.".bak.dat" using 1:4 with linespoint title "suscettività alternata",\
		 "./ac_".x.".bak.dat" using 1:4:5 with yerrorbars title "SDM s.a.",\
		 "./ac_".x.".bak.dat" using 1:6 with linespoint title "densità di energia",\
		 "./ac_".x.".bak.dat" using 1:6:7 with yerrorbars title "SDM d.e."

	print " > Gnuplot : Creato il file ".outf
}
# disavilito il titolo
unset title

#####################################################################
#
# VARIANZE
#
#####################################################################

set ylabel "Varianze"
set xlabel "Dimensione cluster"
	
outfile = sprintf('var.jpeg')
set output outfile

plot "./var_sdom.dat" using 1:2 with linespoint title "suscettività uniforme",\
	 "./var_sdom.dat" using 1:4 with linespoint title "suscettività magnetica",\
	 "./var_sdom.dat" using 1:6 with linespoint title "densità di energia"
	 
print " > Gnuplot : Creato il file ".outfile

#####################################################################
#
# SDOM
#
#####################################################################

set ylabel "SDOM"
set xlabel "Dimensione cluster"

outfile = sprintf('sdom.jpeg')
set output outfile

plot "./var_sdom.dat" using 1:3 with linespoint title "suscettività uniforme",\
	 "./var_sdom.dat" using 1:5 with linespoint title "suscettività magnetica",\
	 "./var_sdom.dat" using 1:7 with linespoint title "densità di energia"
	 
print " > Gnuplot : Creato il file ".outfile
