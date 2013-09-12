Description
===========================

This repository contains my C++ source code for simulating an Anti-Ferromagnetic quantum spin lattice in one dimension (i.e. a spin chain) .
<!--
under the supervision of professor [M. Pepe] [1].
-->
The aim of the project is to evaulate staggered susceptivity, uniform susceptivity and linear energy density for the spin chain.


The main function is located in "reticolo_2d.cpp" while "global.h" contains chain parameters like inverse temperature B,
exchange coupling coefficient J, number of spin sites N and doubled Trotter number M.

Data analysis
---------

In the .cc some functions such as `Reticolo::media()` are defined, that allows to calculate mean and Standard Deviation of Mean (SDM).
However, the file *analisi.cc* contains tools for binning data obtained from "reticolo_2d" in cluster of variable size d and study them as function of d.


[1]: http://fisica.mib.infn.it/pages/it/chi-siamo/persone/who.php?user=pepe&lang=IT "Go to personal homepage"
