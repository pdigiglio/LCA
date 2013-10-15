/*
 * ==================================================================
 *
 *       Filename:  fit_cu.cpp
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

/* funzione per interpolare la scuscettività uniforme */
Double_t cu ( Double_t *x, Double_t *par ) {
	/* 
	 * imposto i parametri per vedere quanto si adattano bene i miei
	 * dati ai risultati trovari da Wiese-Ying
	 */
//	par[0] = 1.68; par[1] = .186;
	/* variabile ausiliaria: par[0] = $\hbar c$ */
	Double_t l = TMath::Power( B * par[0] / x[0], 1./3. );

	/* parte tra parentesi */
	Double_t tmp = t_beta( l, 1. );
	/* ne faccio il quadrato e sistemo i coefficienti */
	tmp = - tmp * tmp / 3.;
	/* aggiungo l'altro termine */
	tmp += t_beta( l, 2. );
	/* sottraggo $\psi$ */
	tmp -= 6. * psi( l );
	/* moltiplico per il coefficiente davanti: par[1] = $\rho_s$ */
	tmp = tmp * TMath::Power( l * l / ( B * par[1] ), 2. ); 

	/* primo termine in beta */
	tmp += t_beta ( l, 1. ) * l * l / ( B * par[1] );
	/* divido per tre */
	tmp = tmp / 3.;
	/* aggiungo uno */
	tmp += 1.;

	/* moltiplico per il primo coefficiente e restituisco */
	return tmp * 2. * par[1] / ( 3. * par[0] * par[0] );
}

/* funzione approssimata per interpolare la scuscettività uniforme */
Double_t CU ( Double_t *x, Double_t *par ) {
	/* variabile ausiliaria: par[0] = $\hbar c$ */
	Double_t l = TMath::Power( B * par[0] / x[0], 1./3. );

	/* parte tra parentesi */
	Double_t tmp = tb1( l );
	/* ne faccio il quadrato e sistemo i coefficienti */
	tmp = - tmp * tmp / 3.;
	/* aggiungo l'altro termine */
	tmp += tb2( l );
	/* sottraggo $\psi$ */
	tmp -= 6. * PSI( l );
	/* moltiplico per il coefficiente davanti: par[1] = $\rho_s$ */
	tmp = tmp * TMath::Power( l * l / ( B * par[1] ), 2. ); 

	/* primo termine in beta */
	tmp += tb1 ( l ) * l * l / ( B * par[1] );
	/* divido per tre */
	tmp = tmp / 3.;
	/* aggiungo uno */
	tmp += 1.;

	/* moltiplico per il primo coefficiente e restituisco */
	return tmp * 2. * par[1] / ( 3. * par[0] * par[0] );
}

int
main ( int argc, char *argv[] ) {
	/* valore di default dei dati in input */
	TString input = "./cu.dat";

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
	(*data).SetTitle( "Uniform Susceptibility" );
	(*data).SetMarkerStyle( 20 );
	(*data).SetMarkerSize( 0.8 );
	/* imposto i titoli degli assi */
	(*data).GetXaxis()->SetTitle( "Lattice side" );
//	(*data).GetYaxis()->SetTitle( "Susceptivity" );
	(*data).GetXaxis()->CenterTitle();
	(*data).GetYaxis()->CenterTitle();

	/* XXX Interpolo sugli stessi valori utilizzati da W-Y */
	TF1 *plot = new TF1( "cu", cu, 6., 20., 2 );
	/* colore linea: 2 = rosso */
	(*plot).SetLineColor(2);
	/* valore di $\hbar c$ */
	(*plot).SetParameter( 0, 1.68 );
	(*plot).SetParName( 0, "#hbar c");
	/* valore di $\rho_s$ */
	(*plot).SetParameter( 1, .186 );
	(*plot).SetParName( 1, "#rho_{s}");

	TCanvas *c = new TCanvas( "out", "Regressione" );
	(*c).SetGridx();
	(*c).SetGridy();
	
	/* interpolo: "Q" (secondo parametro) sta per "quiet" */
//	(*data).Fit("cu", "Q", "", 12., 20. );

	/* disegno il grafico */
	(*data).Draw("APC");
	(*plot).Draw("SAME");

	/* stampo informazioni fit */
	printf( "#hbar c\terr\t\trho_{s}\terr\n" );
	for ( unsigned short int i = 0; i < 2; i ++ )
		printf( "%g\t%g\t", (*plot).GetParameter( i ), (*plot).GetParError( i ) );

	printf( "\n" );
	/* stampo i grafici */
	(*c).Print( "cu.svg", "svg");
	(*c).Print( "cu.pdf", "pdf");

	exit(EXIT_SUCCESS);
}
