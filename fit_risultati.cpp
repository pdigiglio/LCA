/*
 * ==================================================================
 *
 *       Filename:  fit_risultati.cpp
 *
 *    Description:  Programma che sfrutta la suite ROOT per interpola-
 *    				re i risultati dell'esecuzione della versione
 *    				bi-dimensionale del reticolo Anti-Ferromagnetico.
 *
 *        Version:  1.0
 *        Created:  22/09/2013 14:58:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  P. Di Giglio (), p.digiglio91@gmail.com
 *   Organization:  
 *
 * ==================================================================
 */

/* librerie standard C/C++ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "TF1.h"
/* serve per settare il titolo degli assi */
#include "TH1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TString.h"
#include "TGraph.h"
#include "TGraphErrors.h"

/* ordine di arresto nel calcolo della somma 'S' */
#define STOP 10
/* Pi greca */
#define PI 3.141592653589793

/* somme */
double S ( double x ) {
	/* inizializzo la somma a zero */
	double sum = (double) 0;
	/* calcolo la somma in inversa */
	for ( unsigned short int n = STOP; n > 0; n -- )
		sum += exp( - PI * x * pow( n, 2 ) );

	/* sistemo i coefficienti */
	sum = 2 * sum;
	/* incremento ancora di uno la somma e ritorno */
	return ++ sum;
}

/* funzione (ausiliaria) integranda per ottenere 'h_alpha()') */
double integranda ( double *x, double *par ) {
	/* calcolo $S(1/l^2 t)$ */
	double tmp = S( 1. / ( x[0] * pow( par[1], 2. ) ) );
	/* ne faccio il quadrato */
	tmp = pow( tmp, 2. );
	/* moltiplico per l'altro termine */
	tmp = tmp * S( pow( par[1], 4. ) / x[0] );
	/* sottraggo uno */
	tmp --;

	/*
	 * Il valore del parametro 'par[0]' corrisponde all'intero "r" del
	 * lavoro di Beard. Dev'essere impostato dall'esterno nella
	 * funzione 'h_alpha()' che richiama questa
	 */
	return pow( x[0], par[0] - 1 ) * tmp;
}

/* calcolo l'integrale (funzione $\hat\alpha_r(l)$ in tesi Beard) */
double h_alpha ( double x, double par ) {
	/* funzione integranda */
	TF1 *func = new TF1 ( "integranda", integranda, 0, 1, 2);

	/* parametro 'r' */
	(*func).SetParameter( 0, par );
	(*func).SetParName( 0, "r" );
	
	/* parametro 'l' */
	(*func).SetParameter( 1, x );
	(*func).SetParName( 1, "l" );

	/* restituisco il valore della funzione */
	return (*func).Integral( 0, 1 );
}

/* funzione $\alpha$, combinazione lineare degli $\hat\alpha$ */
double alpha ( double *x, double *par ) {
	return h_alpha( x[0], par[0] - 1.5) + h_alpha( 1 / x[0], - par[0]);
}

int
main ( int argc, char *argv[] ) {
	/* valore di default dei dati in input */
	TString input = "./risultati.dat";

	/* controllo che gli argomenti siano corretti */
	if ( argc == 2 )
		input = (TString) argv[1];
	else { /* se gli argomenti sono troppi o troppo pochi */
		if (argc > 2 ) {
			fprintf( stderr, "Too many arguments!\n");

			/* esco dal programma */
			exit(EXIT_FAILURE);
		} else
			fprintf( stderr, "Too few arguments, using default settings!\n");
	}
	
	/* imposto lo stile della tavolozza */
	(*gROOT).SetStyle("Plain");
	(*gStyle).SetPalette();
	(*gStyle).SetOptStat();
	(*gStyle).SetOptFit();
	
	TF1 *plot = new TF1( "c", alpha, 0, 1, 1 );
	(*plot).SetParameter(0, 1);

	TCanvas *c = new TCanvas ( "titolo", "nome" );
	/* Assi, Punti, (linea) Continua */
	(*plot).Draw("APC");
//	(*c).Print( "risultato.svg", "svg");

	exit(EXIT_SUCCESS);
}
