/*
 * ==================================================================
 *
 *       Filename:  fit_cs.cpp
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

/* funzioni matematiche per l'interpolazione */
#include "./fit_functions.cc"

/* 
 * funzione per interpolare la suscettività alternata
 * XXX questa funzione interpola la suscettività normalizzata all'area
 * del reticolo spaziale
 */
Double_t cs ( Double_t *x, Double_t *par ) {
	/* variabile ausiliaria: par[0] = $\hbar c$ */
	Double_t l = TMath::Power( B * par[0] / x[0], 1./3. );
	
	/* termine tra parentesi */
	Double_t tmp = 3. * beta( l, 2. );
	tmp += TMath::Power( beta( l, 1. ), 2. );
	/* moltiplico per il coefficiente davanti: par[1] = $\rho_s$ */
	tmp = tmp * TMath::Power( l * l / ( B * par[1] ), 2. ); 

	/* termini fuori dalla parentesi interna */
	tmp += 2. * beta( l, 1. ) * l * l / ( B * par[1] );
	/* incremento di uno */
	++ tmp;

	/* il parametro 'par[2]' è la magnetizzazione staggered */
	return B * par[2] * par[2] * tmp / 3.;
}

int
main ( int argc, char *argv[] ) {
	/* valore di default dei dati in input */
	TString input = "./cs.dat";

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

	/* file di dati */
	TGraphErrors *data = new TGraphErrors( input );
	/* 9 = blu */
	(*data).SetMarkerColor( 9 );
	/* titolo del grafico */
	(*data).SetTitle( "Staggered Susceptivity" );
	(*data).SetMarkerStyle( 20 );
	(*data).SetMarkerSize( 0.5 );
	/* imposto i titoli degli assi */
	(*data).GetXaxis()->SetTitle( "Lattice side" );
	(*data).GetYaxis()->SetTitle( "Susceptivity" );
	(*data).GetXaxis()->CenterTitle();
	(*data).GetYaxis()->CenterTitle();

	TF1 *plot = new TF1( "cs", cs, 6., 20., 3 );
	/* valore di $\hbar c$ */
	(*plot).SetParameter( 0, 1.68 );
	(*plot).SetParName( 0, "#hbar c");
	/* valore di $\rho_s$ */
	(*plot).SetParameter( 1, .186 );
	(*plot).SetParName( 1, "#rho_{s}");
	/* valore di $M_s$ */
	(*plot).SetParameter( 2, .3074 );
	(*plot).SetParName( 2, "M_{s}");

	TCanvas *c = new TCanvas( "out", "Regressione" );
	(*c).SetGridx();
	(*c).SetGridy();
	
	/* interpolo: "Q" (secondo parametro) sta per "quiet" */
	(*data).Fit("cs", "", "", 6., 20. );

	/* disegno il grafico */
	(*data).Draw("APC");
	/* stampo i grafici */
	(*c).Print( "cs.svg", "svg");
	(*c).Print( "cs.pdf", "pdf");

	exit(EXIT_SUCCESS);
}
