#include "TF1.h"
/* serve per settare il titolo degli assi */
#include "TH1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TString.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMath.h"

/* parametri d'interpolazione */
#include "./fit_pars.h"

/* somme infinite troncate all'ordine 'STOP' */
Double_t S ( Double_t x ) {
	/* inizializzo la somma a zero */
	Double_t sum = (Double_t) 0;
	/* calcolo la somma in inversa */
	for ( unsigned short int n = STOP; n > 0; n -- )
		sum += TMath::Exp( - TMath::Pi() * x * n * n );

	/* sistemo i coefficienti */
	sum = 2. * sum;
	/* incremento ancora di uno la somma e ritorno */
	++ sum;
	return sum;
}

/* funzione (ausiliaria) integranda per ottenere 'h_alpha()') */
Double_t integranda ( Double_t *x, Double_t *par ) {
	/* calcolo $S(1/l^2 t)$ */
	Double_t tmp = S( 1. / ( x[0] * par[1] * par[1] ) );
	/* ne faccio il quadrato */
	tmp = tmp * tmp;
	/* moltiplico per l'altro termine */
	tmp = tmp * S( TMath::Power( par[1], 4. ) / x[0] );
	/* sottraggo uno */
	tmp --;

	/*
	 * Il valore del parametro 'par[0]' corrisponde all'intero "r" del
	 * lavoro di Beard. Dev'essere impostato dall'esterno nella
	 * funzione 'h_alpha()' che richiama questa
	 */
	return TMath::Power( x[0], par[0] - 1. ) * tmp;
}

/* calcolo l'integrale (funzione $\hat\alpha_r(l)$ in tesi Beard) */
Double_t h_alpha ( Double_t x, Double_t r ) {
	/* funzione integranda */
	TF1 *func = new TF1 ( "integranda", integranda, 0., 1., 2);

	/* parametro 'r' */
	(*func).SetParameter( 0, r );
	(*func).SetParName( 0, "r" );
	
	/* parametro 'l' */
	(*func).SetParameter( 1, x );
	(*func).SetParName( 1, "l" );

	/* restituisco il valore della funzione */
	return (*func).Integral( 0., 1. );
}

/* funzione $\alpha$, combinazione lineare degli $\hat\alpha$ */
Double_t alpha ( Double_t x, Double_t p ) {
	return h_alpha( x, p - 1.5 ) + h_alpha( 1. / x, - p );
}

/* le funzioni $\beta_n(l)$ con $n$ diverso da zero*/
Double_t beta ( Double_t x, Double_t n ) {
	/* termine tra parentesi */
	Double_t tmp = alpha( x, n ) - 3. / ( n * ( 2. * n - 3. ) );
	/* sistemo il coefficiente */
	return TMath::Power( - 1. / ( 4. * TMath::Pi() ), n ) * tmp;
}

/* funzione $\beta_0(l)$ */
Double_t beta_0 ( Double_t x ) {
	Double_t tmp = alpha( x, 0. );
	/* aggiungo il fattore costante */
	tmp += G;

	/* restituisco il risultato */
	return tmp;
}

/* funzione ausiliaria per la derivata */
Double_t a_beta ( Double_t *x, Double_t *par ) {
	Double_t tmp = beta( x[0], par[0] );
	/* controllo se 'par[0] == 0' */
	if ( par[0] == 0 ) tmp = beta_0( x[0] );

	return TMath::Power( x[0], 2. * par[0] ) * tmp;
}

/* funzioni $\tilde \beta_n(l)$ */
Double_t t_beta ( Double_t x, Double_t n ) {
	/* 
	 * funzione derivanda
	 * XXX possibile problema con gli estremi della funzione
	 */
	TF1 *deriv = new TF1 ( "derivanda", a_beta, -100., 100., 1);
	/* sistemo il parametro */
	(*deriv).SetParameter( 0, n );

	/* restituisco la funzione */
	return (*deriv).Derivative( x ) * TMath::Power( x, 1. - 2 * n );
}

/* funzione $\psi(l)$ */
Double_t psi ( Double_t x ) {
	Double_t tmp = beta( x, 2. );
	tmp += TMath::Power( beta( x, 1 ), 2 );

	/* sistemo i co)fficienti */
	return - tmp / 3.;
}
