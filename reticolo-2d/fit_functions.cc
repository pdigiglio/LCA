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
	return ( ++ sum );
}

/* derivata della somma rispetto ad 'x' */
Double_t Sd ( Double_t x ) {
	/* inizializzo la somma a zero */
	Double_t sum = (Double_t) 0;
	/* calcolo la somma in inversa */
	for ( unsigned short int n = STOP; n > 0; n -- )
		sum += n * n * TMath::Exp( - TMath::Pi() * x * n * n );

	/* sistemo i coefficienti e ritorno */
	return ( - 2. * TMath::Pi() * sum );
}

/* funzione (ausiliaria) integranda per ottenere 'alpha()' */
Double_t integranda_1 ( Double_t *x, Double_t *p ) {
	/* calcolo $S(1/l^2 t)$ */
	Double_t tmp = S( 1. / ( x[0] * p[1] * p[1] ) );
	/* ne faccio il quadrato */
	tmp = tmp * tmp;
	/* moltiplico per l'altro termine $S(l^4/t)$ */
	tmp = tmp * S( p[1] * p[1] * p[1] * p[1] / x[0] );
	/* sottraggo uno */
	tmp --;

	
	/* calcolo $S(l^2/t)$ */
	Double_t tmp1 = S( p[1] * p[1] / x[0] );
	/* ne faccio il quadrato */
	tmp1 = tmp1 * tmp1;
	/* moltiplico per l'altro termine $S(1/l^4 t)$ */
	tmp1 = tmp1 * S( 1. / ( p[1] * p[1] * p[1] * p[1] * x[0] ) );
	/* sottraggo uno */
	tmp1 --;

	/*
	 * Il valore del parametro 'p[0]' corrisponde all'intero "p" del
	 * lavoro di Beard. Dev'essere impostato dall'esterno nella
	 * funzione 'alpha()' che richiama questa
	 */
	return TMath::Power( x[0] , p[0] - 2.5) * tmp + tmp1 / TMath::Power( x[0], p[0] + 1. );
}

/* funzione integranda per ottenere la derivata di 'alpha()' */
Double_t integranda_2 ( Double_t *x, Double_t *p ) {
	/* calcolo $S(1/l^2 t)$ */
	Double_t tmp1 = S( 1. / ( x[0] * p[1] * p[1] ) );

	/* calcolo l'integranda */
	Double_t tmp = p[1] * p[1] * p[1] * Sd( p[1] * p[1] * p[1] * p[1] / x[0] ) * tmp1;
	tmp -= Sd( 1. / ( x[0] * p[1] * p[1] ) ) * S( p[1] * p[1] * p[1] * p[1] / x[0] ) / ( p[1] * p[1] * p[1] );
	tmp = tmp * tmp1;

	/* calcolo $S(l^2 / t)$ */
	tmp1 = S( p[1] * p[1] / x[0] );

	/* calcolo l'integranda */
	Double_t tmp2 = p[1] * Sd( p[1] * p[1] / x[0] ) * S( 1. / ( p[1] * p[1] * p[1] * p[1] * x[0] ) );
	tmp2 -= Sd( 1. / ( x[0] * p[1] * p[1] * p[1] * p[1] ) ) * tmp1 / ( p[1] * p[1] * p[1] * p[1] * p[1] );
	tmp2 = tmp2 * tmp1;

	return 4. * ( TMath::Power( x[0], p[0] - 3.5 ) * tmp + tmp2 / TMath::Power( x[0], p[0] + 2. ) );
}

/* funzione $\alpha$ */
Double_t alpha ( Double_t x, Double_t p ) {
	/* funzione integranda */
	TF1 *func = new TF1 ( "integranda", integranda_1, 0., 1., 2);

	/* parametro 'p' */
	(*func).SetParameter( 0, p );
	(*func).SetParName( 0, "p" );
	
	/* parametro 'l' */
	(*func).SetParameter( 1, x );
	(*func).SetParName( 1, "l" );

	/* restituisco il valore della funzione */
	return (*func).Integral( 0., 1. );
}

/* funzione per la derivata di 'alpha()' */
Double_t alphad ( Double_t x, Double_t p ) {
	/* funzione integranda */
	TF1 *func = new TF1 ( "integranda", integranda_2, 0., 1., 2);

	/* parametro 'p' */
	(*func).SetParameter( 0, p );
	(*func).SetParName( 0, "p" );
	
	/* parametro 'l' */
	(*func).SetParameter( 1, x );
	(*func).SetParName( 1, "l" );

	/* restituisco il valore della funzione */
	return (*func).Integral( 0., 1. );
}

/* le funzioni $\beta_n(l)$ con $n$ diverso da zero*/
Double_t beta ( Double_t x, Double_t n ) {
	/* termine tra parentesi */
	Double_t tmp = alpha( x, n );
	/* se 'n != 0' aggiungo un termine */
	if ( n ) tmp += 3. / ( n * ( 2. * n - 3. ) );
	else /* if ( !n ) */ tmp += (Double_t) G;

	/* sistemo il coefficiente */
	return tmp / TMath::Power( - 4. * TMath::Pi(), n );
}

/* derivata delle funzioni $\beta$ */
double betad ( double x, double n ) {
	return alphad( x, n ) / TMath::Power( - 4. * TMath::Pi(), n );
}

/* funzioni $\tilde \beta_n(l)$ */
Double_t t_beta ( Double_t x, Double_t n ) {
	Double_t tmp = x * betad( x, n );
	tmp += 2. * n * beta( x, n );

	return tmp;
}

/* funzione $\psi(l)$ (approssimata) */
Double_t psi ( Double_t x ) {
	/* variabile temporanea */
	Double_t tmp = 3.900265 * TMath::Power( x, 5.) / ( 24. * TMath::Pi() );
	tmp -= .055 * x * x;
	tmp -= 4.78 * TMath::Exp( - 2. * TMath::Pi() * x * x * x );

	return tmp - TMath::Power( x, 8. ) / 120.;
}

/*-------------------------------------------------------------------
 *  XXX funzioni approssimate per l'interpolazione
 *-----------------------------------------------------------------*/
double b0 ( double x ) {
	return x * x * x * 1.437745 - 4. * TMath::Log( x );
}

double b1 ( double x ) {
	return x * ( 3.900265 / ( 4. * TMath::Pi() ) - x * x * x / 12. );
}

double b2 ( double x ) {
	return x * x * ( TMath::Power( x, 6. ) / 720. + 1.437745 / ( TMath::Power( 4. * TMath::Pi(), 2 ) * TMath::Pi() * x * x * x) );
}

double tb0 ( double x ) {
	return 3. * x * x * x * 1.437745 - 4.;
}

double tb1 ( double x ) {
	return 2. * b1( x ) + x * x * ( 3.900265 / ( 4. * TMath::Pi() ) - x * x * x / 3. );
}

double tb2 ( double x ) {
	return 4. * b2( x ) + TMath::Power( x, 8. ) / 90. - 1.437745 / ( 16. * TMath::Pi() * TMath::Pi() * x );
}

double PSI ( double x ) {
	return psi(x);
}
