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

int
main ( int argc, char *argv[] ) {
	/* valore di default dei dati in input */
	TString input = "./ac.dat";

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
	
	/* tre grafici contenenti gli autocorrelatori */	
	TGraphErrors *gr[3] = {
		new TGraphErrors ( input, "%lg %lg %lg %*lg %*lg %*lg %*lg" ),
		new TGraphErrors ( input, "%lg %*lg %*lg %lg %lg %*lg %*lg" ),
		new TGraphErrors ( input, "%lg %*lg %*lg %*lg %*lg %lg %lg" )
	};

	/* funzione per interpolare gli autocorrelatori */
	TF1 *fit[3];

	/* titoli dei grafici e delle finestre */
	TString title[3] = {
		"Uniform Susceptivity",
		"Staggered Susceptivity",
		"Energy Density"
	};
	/* nomi dei file di output */
	TString out[3] = { "ac_us", "ac_ss", "ac_en" };
	/* "tavolozze" per stampare i grafici su file .pdf e .jpeg */
	TCanvas *c[3];

	/* creo i grafici */
	for ( unsigned short int i = 0; i < 3; i ++ ) {
		/* 3 = verde */
		(*gr[i]).SetMarkerColor(3);
		/* titolo del grafico */
		(*gr[i]).SetTitle( title[i] );
		(*gr[i]).SetLineStyle(1);
		(*gr[i]).SetMarkerStyle(20);
		(*gr[i]).SetMarkerSize(0.5);
		/* imposto i titoli degli assi */
		(*gr[i]).GetXaxis()->SetTitle("Sweep");
		(*gr[i]).GetYaxis()->SetTitle("Auto-correlator (normalized)");
		(*gr[i]).GetXaxis()->CenterTitle();
		(*gr[i]).GetYaxis()->CenterTitle();
	
		c[i] = new TCanvas ( out[i], title[i] );
		(*c[i]).SetGridx();
		(*c[i]).SetGridy();
		(*c[i]).SetCanvasSize( 800, 600 );

		/* 
		 * l'andamento esponenziale vale approssimativamente per un
		 * dominio abbastanza ristretto, quindi definisco la funzione
		 * solo da 0 a 15 (che darà già un chi quadro molto basso)
		 */
		fit[i] = new TF1("f1", "[1] + [2] * exp( - x / [0])", 0, 15);
		/* il tempo di autocorrelazione è dell'ordine di due */
		(*fit[i]).SetParameter(0, 2);
		/* la traslazione verticale dovrebbe essere nulla */
		(*fit[i]).SetParameter(1, 0);
		/* il coefficiente di dilatazione dovrebbe essere uno */
		(*fit[i]).SetParameter(2, 1);

		/* imposto il nome della costante 'p[0]' */
		(*fit[i]).SetParName(0, "Decay time");
		/* imposto il nome della costante 'p[0]' */
		(*fit[i]).SetParName(1, "Traslation");
		/* imposto il nome della costante 'p[0]' */
		(*fit[i]).SetParName(2, "Dilatation");

		/* colore linea: 2 = rosso */
		(*fit[i]).SetLineColor(2);
		/* interpolo: "Q" sta per "quiet" */
		(*gr[i]).Fit("f1", "Q", "", 0, 10);
		
		/*
		** (*x).Draw() va sempre dopo la dichiarazione
		** di Canvas, altrimenti il grafico non viene stampato
		*/
		
		/* Assi, Punti, (linea) Continua */
		(*gr[i]).Draw("APC");
		(*c[i]).Print( out[i] + ".svg", "svg");
		/* per un'eventuale inclusione nella tesi */
		(*c[i]).Print( out[i] + ".pdf" , "pdf");
	
		/* stampo i valori ottenuti */
		printf("%lf\t", (double) (*fit[i]).GetParameter( 0 ));
		printf("%lf\t", (double) (*fit[i]).GetParError( 0 ));
	}

	printf("\n");
	
	exit(EXIT_SUCCESS);
}		/* -----  end of function fit  ----- */
