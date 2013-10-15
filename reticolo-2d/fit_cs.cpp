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

/* funzione per interpolare la suscettività alternata */
Double_t cs ( Double_t *x, Double_t *par ) {
	/* 
	 * imposto i parametri per vedere quanto si adattano bene i miei
	 * dati ai risultati trovari da Wiese-Ying
	 */
//	par[0] = 1.68; par[1] = .186; par[2] = .3074;
	/* variabile ausiliaria: par[0] = $\hbar c$ */
	Double_t l = TMath::Power( B * par[0] / x[0], 1./3. );
	
	/* termine tra parentesi */
	Double_t tmp = beta ( l, 1. );
	/* ne faccio il quadrato */
	tmp = tmp * tmp;
	/* aggiungo l'altro termine */
	tmp += 3. * beta( l, 2. );
	/* moltiplico per il coefficiente davanti: par[1] = $\rho_s$ */
	tmp = tmp * TMath::Power( l * l / ( B * par[1] ), 2. ); 

	/* termini fuori dalla parentesi interna */
	tmp += 2. * beta( l, 1. ) * l * l / ( B * par[1] );
	/* incremento di uno */
	++ tmp;

	/* il parametro 'par[2]' è la magnetizzazione staggered */
	return B * par[2] * par[2] * tmp * x[0] * x[0] / 3.;
}

/* funzione approssimata */
Double_t CS ( Double_t *x, Double_t *p ) {
	/* 
	 * imposto i parametri per vedere quanto si adattano bene i miei
	 * dati ai risultati trovari da Wiese-Ying
	 */
//	p[0] = 1.68; p[1] = .186; p[2] = .3074;
	/* variabile ausiliaria: par[0] = $\hbar c$ */
	Double_t l = TMath::Power( B * p[0] / x[0], 1./3. );
	
	/* termine tra parentesi */
	Double_t tmp = b1 ( l );
	/* ne faccio il quadrato */
	tmp = tmp * tmp;
	/* aggiungo l'altro termine */
	tmp += 3. * b2( l );
	/* moltiplico per il coefficiente davanti: par[1] = $\rho_s$ */
	tmp = tmp * TMath::Power( l * l / ( B * p[1] ), 2. ); 

	/* termini fuori dalla parentesi interna */
	tmp += 2. * b1( l ) * l * l / ( B * p[1] );
	/* incremento di uno */
	++ tmp;

	/* il parametro 'par[2]' è la magnetizzazione staggered */
	return B * p[2] * p[2] * tmp * x[0] * x[0] / 3.;
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
	/* 3/8 = verde vivo/spento */
	(*data).SetMarkerColor( 8 );
	/* spessore linea minimo 1 - 10 */
	(*data).SetLineWidth( 1 );
	/* tratteggiata */
	(*data).SetLineStyle( 7 );
	/* titolo del grafico */
	(*data).SetTitle( "Staggered Susceptibility" );
	(*data).SetMarkerStyle( 20 );
	(*data).SetMarkerSize( .8 );
	/* imposto i titoli degli assi */
	(*data).GetXaxis()->SetTitle( "Lattice side" );
//	(*data).GetYaxis()->SetTitle( "Staggered Susceptivity" );
	(*data).GetXaxis()->CenterTitle();
	(*data).GetYaxis()->CenterTitle();

	TF1 *plot = new TF1( "cs", cs, 6., 20., 3 );
	/* colore linea: 2 = rosso */
	(*plot).SetLineColor( 2 );
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
//	(*data).Fit("cs", "Q", "", 12., 20. );

	/* disegno il grafico */
	(*data).Draw("APC");
	(*plot).Draw("SAME");

	/* stampo informazioni fit */
	printf( "#hbar c\terr\t\trho_{s}\terr\tM_{s}\t\terr\n" );
	for ( unsigned short int i = 0; i < 3; i ++ )
		printf( "%g\t%g\t", (*plot).GetParameter( i ), (*plot).GetParError( i ) );

	printf( "\n" );
	
	printf( "\n\nChi quadro: %g %d\n", (*data).Chisquare( plot ), (*plot).GetNDF() );

	/* stampo i grafici */
	(*c).Print( "cs.svg", "svg");
	(*c).Print( "cs.pdf", "pdf");

	exit(EXIT_SUCCESS);
}
