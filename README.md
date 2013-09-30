Description
===========================

This repository contains my C++ source code for simulating an Anti-Ferromagnetic quantum spin lattice in one and two dimensions at very low temperatures.
The algorithm is based on the Anti-Ferromagnetic Heisenberg Model, which is __sperimentally__ proved to describe the physical system.


This work is mostly based on the studies of [Wiese-Ying] [4], who first adapted the Loop Cluster Algorithm (LCA) to spin lattices.
Another useful reference is [Beard] [5] Ph.D. thesis, which exhaustively explains the algorithm details and extends it to continuum limit.


There are two different and equivalent implementations of LCA:
* __Single-cluster__
in which one loop at time is constructed and all the spin in it are flipped;
* __Multi-cluster__
in which the lattice is fully covered with loops that are randomly flipped with 50% of probability.

This project is build upon the latter.

File description
-------------------

There are two main source sub-directory which contains the code for generating data (measures).
Each sub-directory should contain its own README.md descriptor file.

A lot of extra tools are provided in this directory to easily manage and analyze data:
* *gcccpuopt.sh* is a shell script taken from [][] to detect cpu-based gcc flags you may want to activate;
* *analisi_common.sh* contains some useful function used in *./reticolo{1,2}-d/analisi.sh*.

[4]: http://arxiv.org/abs/hep-lat/9204015v1 "Blockspin Cluster Algorithms for Quantum Spin Systems"
[5]: http://dspace.mit.edu/handle/1721.1/38414#files-area "Improvements in Cluster Algorithms for Quantum Spin Systems"
