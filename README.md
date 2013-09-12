Description
===========================

This repository contains my C++ source code for simulating an Anti-Ferromagnetic quantum spin lattice in one dimension (i.e. a spin chain).

The main function is located in "reticolo_2d.cpp" while "global.h" contains chain parameters like inverse temperature B,
exchange coupling coefficient J, number of spin sites N and doubled Trotter number M.

Data analysis
---------

In the .cc some functions such as `Reticolo::media()` are defined, that allows to calculate mean and Standard Deviation of Mean (SDM).
However, the file *analisi.cc* contains tools for binning data obtained from "reticolo_2d" in cluster of variable size d and study them as function of d.
