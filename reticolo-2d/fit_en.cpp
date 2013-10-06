/*
 * ==================================================================
 *
 *       Filename:  fit_en.cpp
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

/* funzione per interpolare l'energia */
Double_t en ( Double_t *x, Double_t *par ) {
	/* variabile ausiliaria: par[0] = $\hbar c$ */
	Double_t l = TMath::Power( B * par[0] / x[0], 1./3. );
	
	/* 
	 * termine tra parentesi (ho sfruttato le funzioni note per non
	 * dover riscrivere un'altra funzione per calcolare $l d\beta/dl$)
	 */
	Double_t tmp = t_beta( l, 1. ) - 3. * beta( l, 1. );
	/* moltiplico per il coefficiente */
	tmp = tmp * l * l / ( par[1] * B );
	/* primo termine fuori dalle parentesi */
	tmp += t_beta( l, 0. );
	/* incremento di uno */
	tmp ++;

	/* sistemo il coefficiente all'esterno */
	tmp = tmp / ( 3. * B * x[0] * x[0] );

	/* 'par[2]' è la densità d'energia $e_0$ */
	return par[2] - tmp;
}

int
main ( int argc, char *argv[] ) {
	/* valore di default dei dati in input */
	TString input = "./en.dat";

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
	(*data).SetTitle( "Energy density" );
	(*data).SetMarkerStyle( 20 );
	(*data).SetMarkerSize( 0.5 );
	/* imposto i titoli degli assi */
	(*data).GetXaxis()->SetTitle( "Lattice side" );
	(*data).GetYaxis()->SetTitle( "Energy density" );
	(*data).GetXaxis()->CenterTitle();
	(*data).GetYaxis()->CenterTitle();

	TF1 *plot = new TF1( "en", en, 6., 20. );
	/* valore di $\hbar c$ */
	(*plot).SetParameter( 0, 1.68 );
	(*plot).SetParName( 0, "#hbar c");
	/* valore di $\rho_s$ */
	(*plot).SetParameter( 1, .186 );
	(*plot).SetParName( 1, "#rho_{s}");
	(*plot).SetParameter( 2, - 0.6693 );
	(*plot).SetParName( 2, "e_{0}");

	TCanvas *c = new TCanvas( "out", "Regressione" );
	(*c).SetGridx();
	(*c).SetGridy();
	
	/* interpolo: "Q" (secondo parametro) sta per "quiet" */
	(*data).Fit("en", "", "", 6., 20. );

	/* disegno il grafico */
	(*data).Draw("APC");
	/* stampo i grafici */
	(*c).Print( "en.svg", "svg");
	(*c).Print( "en.pdf", "pdf");

	exit(EXIT_SUCCESS);
}
