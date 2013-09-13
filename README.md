Description
===========================

This repository contains my C++ source code for simulating an Anti-Ferromagnetic quantum spin lattice in one dimension (i.e. a spin chain) at low temperatures.
<!--
under the supervision of professor [M. Pepe] [1].
-->
The aim of the project is to evaluate staggered susceptivity, uniform susceptivity and linear energy density of the spin chain.


The main routine for generating data (measures) is stored in *reticolo_2d.** files, which needs also *global.h* header to be included.
A *Makefile* is also given, which allows to compile the main program (please, set your own compile flags since specific options such as `-march=core2` are selected).
The `main()` function is located in *reticolo_2d.cpp*; class `Reticolo` is defined and implemented in *reticolo_2d.h* and *reticolo_2d.cc*, respectively.
The header *global.h* contains chain parameters like inverse temperature *B*,
exchange coupling coefficient *J*, number of spin sites *N* and doubled Trotter number *M*.
This file is supposed to be dinamically generated by *analisi.sh*, a bash-script that automatizes data generation, analysis and plotting (with [gnuplot] [2]).


Data analysis
---------

In the *.cc* file some functions such as `Reticolo::media()` are defined, that allows to calculate mean and Standard Deviation of Mean (SDM).
However, the file *analisi.cc* contains tools for binning data obtained from "reticolo_2d" in cluster of variable size d and study them as function of d.


[1]: http://fisica.mib.infn.it/pages/it/chi-siamo/persone/who.php?user=pepe&lang=IT "Go to personal homepage"
[2]: http://www.gnuplot.info/ "Go to gnuplot homepage"
