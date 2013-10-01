General
=====================

In this sub-directory are stored source code files for the *bi-dimensional* version of [mono-dimensional spin chain] [1].


Since bi-dimensional lattice numerical simluation requires higher computational time, the plaque detecting function of this version is _slightly_ better in relation to mono-dimensional one.

<!---
All the functions are nearly the same as in mono-dimensional implementation except for plaque detecting.
Indeed, spin chain simulation has been written in such a way to allow bi-dimensional lattice generalization quite as simply as possible.
-->

There are also two extra files:
* __fit_risultati.cpp__ for fitting lattice observables;
* __make_res_plot.plt__ a [GNUplot] [2] script to plot observables data file called _risultati.dat_;

[1]: https://github.com/pdigiglio/LCA/tree/master/reticolo-1d "Mono-dimensional version of this project"
[2]: http://www.gnuplot.info/ "Go to gnuplot homepage"
