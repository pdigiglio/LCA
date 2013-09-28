#define STOP 10

double S ( double x ) {
	/* inizializzo la somma a zero */
	double sum = (double) 0;
	/* calcolo la somma in inversa */
	for ( unsigned short int n = STOP; n > 0; n -- )
		sum += TMath::Exp( - TMath::Pi() * x * n * n );

	/* sistemo i coefficienti */
	sum = 2. * sum;
	/* incremento ancora di uno la somma e ritorno */
	return ++ sum;
}

/* funzione (ausiliaria) integranda per ottenere 'h_alpha()') */
double integranda ( double *x, double *par ) {
	/* calcolo $S(1/l^2 t)$ */
	double tmp = S( 1. / ( x[0] * par[1] * par[1] ) );
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
double h_alpha ( double x, double r ) {
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
double alpha ( double x, double p ) {
	return h_alpha( x, p - 1.5 ) + h_alpha( 1. / x, - p );
}

/* le funzioni $\beta_n(l)$ */
double beta ( double x, double n ) {
	/* termine tra parentesi */
	double tmp = alpha( x, n ) - 3. / ( n * ( 2. * n - 3. ) );
	/* sistemo il coefficiente */
	return TMath::Power( - 1. / ( 4. * TMath::Pi() ), n ) * tmp;
}

/* funzione ausiliaria per la derivata */
double a_beta ( double *x, double *par ) {
	return TMath::Power( x[0], 2. * par[0] ) * beta( x[0], par[0] );
}

/* funzioni $\tilde \beta_n(l)$ */
double t_beta ( double x, double n ) {
	/* 
	 * funzione derivanda
	 * XXX possibile problema con gli estremi della funzione
	 */
	TF1 *deriv = new TF1 ( "derivanda", a_beta, -100., 100., 1);
	/* sistemo il parametro */
	(*deriv).SetParameter( 0, n );

	/* restituisco la funzione */
	return (*deriv).Derivative( x ) / TMath::Power( x, n + 1. );
}

/* funzione $\psi(l)$ */
double psi ( double x ) {
	double tmp = beta( x, 2. );
	tmp += TMath::Power( beta( x, 1 ), 2 );

	/* sistemo i co)fficienti */
	return - tmp / 3.;
}

/* funzione per interpolare la scuscettività uniforme */
double cu ( double *x, double *par ) {
	/* fisso il valore di $\beta$ */
	double B = (double) 5;
	/* variabile ausiliaria: par[0] = $\hbar c$ */
	double l = TMath::Power( B * par[0] / x[0], 1./3. );

	/* parte tra parentesi */
	double tmp = t_beta( l, 2 );
	tmp -= TMath::Power( t_beta( l, 1. ), 2. ) / 3.;
	tmp -= 6. * psi( l );
	/* moltiplico per il coefficiente davanti: par[1] = $\rho_s$ */
	tmp = tmp * TMath::Power( l * l / ( B * par[1] ), 2 ); 

	/* primo termine in beta */
	tmp += t_beta ( l, 1. ) * l * l / ( B * par[1] );
	/* divido per tre */
	tmp = tmp / 3.;
	/* aggiungo uno */
	++ tmp;

	/* moltiplico per il primo coefficiente */
	tmp = tmp * 2. * par[1] / ( 3. * par[0] * par[0] );

	/* restituisco il valore della funzione */
	return tmp;
}

void fit_risultati ( TString input = "./risultati.dat" ) {
	/* imposto lo stile della tavolozza */
	(*gROOT).SetStyle("Plain");
	(*gStyle).SetPalette();
	(*gStyle).SetOptStat();
	(*gStyle).SetOptFit();

	TF1 *plot = new TF1( "cu", cu, 4., 24., 2 );
	/* parametro "r" di $\hat\alpha(l)$ */
	(*plot).SetParameters(1.68, 0.186);

	TCanvas *c = new TCanvas( "out", "titolo" );
	
	(*plot).Draw();
}
