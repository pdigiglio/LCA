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
#include "TLegend.h"

/* funzione per interpolare l'energia */
Double_t en ( Double_t *x, Double_t *par ) {
//	par[1] = .186;
//	par[0] = 1.68;
	/* variabile ausiliaria: par[0] = $\hbar c$ */
	Double_t l = TMath::Power( B * par[0] / x[0], 1./3. );
	
	/* 
	 * termine tra parentesi (ho sfruttato le funzioni note per non
	 * dover riscrivere un'altra funzione per calcolare $l d\beta/dl$)
	 */
	Double_t tmp = l * betad( l, 1. ) - beta( l, 1. );
	/* moltiplico per il coefficiente */
	tmp = tmp * l * l / ( par[1] * B );
	/* primo termine fuori dalle parentesi */
	tmp += l * betad( l, 0. );
	/* incremento di uno */
	tmp += 1.;

	/* 'par[2]' è la densità d'energia $e_0$ */
	return par[2] - tmp / ( 3. * B * x[0] * x[0] );
}

/* funzione per interpolare l'energia */
Double_t EN ( Double_t *x, Double_t *par ) {
	/* variabile ausiliaria: par[0] = $\hbar c$ */
	Double_t l = TMath::Power( B * par[0] / x[0], 1./3. );
	
	/* 
	 * termine tra parentesi (ho sfruttato le funzioni note per non
	 * dover riscrivere un'altra funzione per calcolare $l d\beta/dl$)
	 */
	Double_t tmp = tb1( l ) - 3. * b1( l );
	/* moltiplico per il coefficiente */
	tmp = tmp * l * l / ( par[1] * B );
	/* primo termine fuori dalle parentesi */
	tmp += tb0( l );
	/* incremento di uno */
	tmp += 1.;

	/* 'par[2]' è la densità d'energia $e_0$ */
	return par[2] - tmp / ( 3. * B * x[0] * x[0] );
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
	TGraphErrors
		*d = new TGraphErrors( "./results/en_B10_traslat.dat" ),
		*data = new TGraphErrors( input );

	/* 3/8 = verde vivo/spento */
	(*data).SetMarkerColor( 8 );
	/* 4 = blu vivo */
	(*d).SetMarkerColor( 4 );

	/* spessore linea minimo 1 - 10 */
	(*data).SetLineWidth( 1 );
	(*d).SetLineWidth( 1 );
	/* tratteggiata */
	(*data).SetLineStyle( 7 );
	(*d).SetLineStyle( 5 );
	/* 12/11 = grigio scuro/chiaro */
	(*d).SetLineColor( 11 );


	(*data).SetMarkerStyle( 20 );
	(*data).SetMarkerSize( 0.8 );

	(*d).SetMarkerStyle( 21 );
	(*d).SetMarkerSize( 0.5 );

	/* titolo del grafico */
	(*data).SetTitle( "Energy density" );
	/* imposto i titoli degli assi */
	(*data).GetXaxis()->SetTitle( "Lattice side" );
//	(*data).GetYaxis()->SetTitle( "Energy density" );
	(*data).GetXaxis()->CenterTitle();
	(*data).GetYaxis()->CenterTitle();

	TF1 *plot = new TF1( "en", en, 6., 20. , 3 );
	/* colore linea: 2 = rosso */
	(*plot).SetLineColor( 2 );
	/* valore di $\hbar c$ */
	(*plot).SetParameter( 0, 1.68 );
	(*plot).SetParName( 0, "#hbar c");
	/* valore di $\rho_s$ */
	(*plot).SetParameter( 1, .186 );
	(*plot).SetParName( 1, "#rho_{s}");
	(*plot).SetParameter( 2, - 0.6694 );
	(*plot).SetParName( 2, "e_{0}");

	TCanvas *c = new TCanvas( "out", "Regressione" );
	(*c).SetGridx();
	(*c).SetGridy();
	
	/* interpolo: "Q" (secondo parametro) sta per "quiet" */
//	(*d).Fit("en", "Q", "" , 6., 20. );

	/* disegno il grafico */
	(*data).Draw("APC");
	(*d).Draw( "PCSAME" );
	(*plot).Draw( "SAME" );

	/* stampo informazioni fit */
	printf( "#hbar c\terr\t\trho_{s}\terr\te_{0}\t\terr\n" );
	for ( unsigned short int i = 0; i < 3; i ++ )
		printf( "%g\t%g\t", (*plot).GetParameter( i ), (*plot).GetParError( i ) );

	printf( "\n" );

	/* stampo i grafici */
	(*c).Print( "en.svg", "svg");
	(*c).Print( "en.pdf", "pdf");

	exit(EXIT_SUCCESS);
}
