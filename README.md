Description
===========================

This repository contains my C++ source code for simulating an Anti-Ferromagnetic quantum spin lattice in one and two dimensions at very low temperatures.
The algorithm is based on the Anti-Ferromagnetic Heisenberg Model, which is __sperimentally__ proved to describe the physical system.


This work is mostly based on the [studies] [4] of Wiese and Ying, who first adapted the Loop Cluster Algorithm (LCA) to spin lattices.
Another useful reference is Beard [Ph.D. thesis] [5], which exhaustively explains the algorithm details and extends it to continuum limit.


LCA is a very efficient and ergodic algorithm which essentially consists in creating disjoint and non-intersecting paths --- according to specific rules --- over lattice.
There are two different and equivalent implementations of LCA:
* __Single-cluster__
in which one loop at time is constructed and all the spin in it are flipped;
* __Multi-cluster__
in which the lattice is fully covered with loops that are randomly flipped with 50% of probability.

This project is build upon the latter.

File description
-------------------

There are two main source sub-directories which contain the code for generating data (measures).
Each sub-directory should contain its own README.md descriptor file too.

A lot of extra tools are provided in this directory to easily manage and analyze data:
* **gcccpuopt.sh** is a shell script taken from [here][6] to detect cpu-based gcc flags you may want to activate;
* **analisi_common.sh** contains some useful function used in *./reticolo{1,2}-d/analisi.sh*;
* **cut.c** is a very simple C program to round data files generated by other routines.
You have to manually adapt its content to the file format.

Simple Makefiles are given for each directory to simplify compiling sources.
Please, set your own compile flags since specific options such as `-march=core2` are selected

Data analysis
---------

In all the *.cc* files stored in *reticolo-{1,2}d* directories some functions such as `Reticolo::mean()` are defined, that allows to calculate mean and Standard Deviation of Mean (SDM).
However, *analisi.cpp* contains tools for binning data contained in *data.dat* (created by execution of *./reticolo-1d*) in bin of variable size _d_ and study them as function of _d_.
This routine is written to generate also auto-correlator plot points for each observable to be measured (i.e. _staggered_ and _uniform susceptivity_ and _energy density_).
Every line of *data.dat* is expected to be formatted as `"%f, %f, %f\n"` to be correctly read by *analisi*.


A [ROOT] [3]-based routine for fitting auto-correlator graphs generated by _analisi.cpp_ is provided in _fit.cpp_ source code.
To compile it you need to use the following syntax (i.e. to link ROOT library in it):

	g++ fit.cpp -o fit `root-config --libs --cflags` [...]

You will also need the ROOT suite installed on your locale to have the compile process work.


[4]: http://arxiv.org/abs/hep-lat/9204015v1 "Blockspin Cluster Algorithms for Quantum Spin Systems"
[5]: http://dspace.mit.edu/handle/1721.1/38414#files-area "Improvements in Cluster Algorithms for Quantum Spin Systems"
[6]: https://github.com/pixelb/scripts/commits/master/scripts/gcccpuopt "Pádraig Brady"
