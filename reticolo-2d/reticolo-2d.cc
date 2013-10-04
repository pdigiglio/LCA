#include "./reticolo-2d.h"

#include <cmath>
/* per l'operatore bitwise XOR (^) */
#include <ciso646>

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: Reticolo
 * Description: [ctor] Costruisco il reticolo di spin.
 * ------------------------------------------------------------------
 */
Reticolo::Reticolo ( void ) {
	/* costruisco il reticolo */
	sito = (Sito ***) malloc ( N * sizeof(Sito **) );
	if ( sito == NULL ) {
		fprintf ( stderr, "[ctor] (1) dynamic memory reallocation failed\n" );
		exit (EXIT_FAILURE);
	}

	/*
	 * parallelizzo solo i processi per cui il passo i-esimo non
	 * dipende dal passo (i-1)-esimo, altrimenti ottengo un
	 * risultato non corretto
	 */
	#pragma omp parallel for
	for ( unsigned short int n = 0; n < N; n ++ ) {
		sito[n] = (Sito **) malloc ( M * sizeof(Sito *) );
		if ( sito[n] == NULL ) {
			fprintf ( stderr, "[ctor] (2) dynamic memory allocation failed\n" );
			exit (EXIT_FAILURE);
		}

		for ( unsigned short int m = 0; m < M; m ++ ) {
			sito[n][m] = (Sito *) malloc ( T * sizeof(Sito) );
			if ( sito[n][m] == NULL ) {
				fprintf ( stderr, "[ctor] (3) dynamic memory reallocation failed\n" );
				exit (EXIT_FAILURE);
			}

			/* Configurazioni iniziali:
			 *  > ( n + m + t ) % 2. trans. forzata;
			 *  > 1 (o 0). cont. forzata;
			 */
			for ( unsigned short int t = 0; t < T; t ++ ) {
				sito[n][m][t].s = (bool) ( ( m + n + t ) % 2);
				sito[n][m][t].ckd = lckd;
			}
		}
	}
} /* -----  end of method Reticolo::Reticolo (ctor)  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: ~Reticolo
 * Description: destructor
 * ------------------------------------------------------------------
 */
Reticolo::~Reticolo ( void ) {
} /* -----  end of method Reticolo::~Reticolo (dtor)  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: fill
 * Description: Resetta i valori (temporanei) delle misure nello sweep
 * 				attuale, copre il reticolo di loop disgiunti ed ag-
 * 				giorna le medie delle misure. Eventualmente calcola 
 * 				anche i loop generati nello sweep corrente.
 * ------------------------------------------------------------------
 */
void
Reticolo::fill ( void ) {
	/* azzero il numero di loop/dec. temporali nello sweep corrente */
	lps = 0; tds = 0;

	/*
	 * aggiorno il numero di misure (e sweep) e calcolo il valore
	 * (booleano) della variabile 'lckd'
	 */
	lckd = ( ++ msr.lenght ) % 2;

	/* inizializzo a zero le suscettività ed energie (temporanee) */
	for ( unsigned short int i = 0; i < 3; i ++ )
		msr.val[i] = (double) 0;

	/* copro l'intero reticolo con i cluster */
	for ( unsigned short int n = 0; n < N ; n ++ )
		for ( unsigned short int m = 0; m < M ; m ++ )
			for ( unsigned short int t = 0; t < T ; t ++ ) {
				/*
				 * controllo se il sito (n, m, t) è già contenuto in
				 * qualche cluster, nel qual caso salto le istruzioni
				 * seguenti e vado al prossimo passo nel ciclo
				 */
				if ( sito[n][m][t].lckd == lckd ) continue;
				
				/* 
				 * se non è già contenuto in altri cluster aggiorno
				 * il numero loop nello sweep corrente
				 */
				lps ++;

				/* faccio partire un loop dal sito (n, m, t) */
				loop(n, m, t);
			}

	/* aggiorno medie e SDM (valori calcolati in 'Reticolo::loop()') */
	for ( unsigned short int i = 0; i < 3; i ++ ) {
		msr.mean[i] += (long double) msr.val[i];
		msr.sdom[i] += (long double) pow( msr.val[i], 2 );
	}
} /* -----  end of method Reticolo::fill  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: spin
 * Description: Restituisce lo spin nel sito (n, m, t) prima del flip.
 * 				Funziona solo se lo spin è stato invertito nel loop
 * 				corrente, non in quelli precedenti.
 * ------------------------------------------------------------------
 */
bool
Reticolo::spin ( unsigned int n, unsigned int m, unsigned int t ) {
	/* se il sito è stato invertito */
	if ( flp && ( sito[n][m][t].lckd == lckd ) )
		return !( sito[n][m][t].s );

	/* valore di default */
	return sito[n][m][t].s;
} /* -----  end of method Reticolo::spin  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: loop
 * Description: Faccio partire il loop dal sito (n, m, t) e decido se
 * 				invertire i suoi spin in maniera casuale. 
 * ------------------------------------------------------------------
 */
void
Reticolo::loop ( unsigned int n, unsigned int m, unsigned int t ) {
	/* 
	 * Estraggo a caso un valore per la variabile (globale) che con-
	 * trolla l'inversione degli spin sul percorso attuale: la q.tà
	 * 'rand() % 2' vale '0' oppure '1' quindi è un booleano casuale.
	 */
 	flp = rand() % 2;

	/* assegno il punto di partenza del loop */
	x[0] = n; x[1] = m; x[2] = t;

	/* magnetizzazione uniforme */
	double cu = (double) 0;
	/* magnetizzazione alternata inizializzata a zero */
	unsigned long int cs = 0;

	/* faccio evolvere il loop finche' non torno al p.to di partenza */
	do {
		/* 
		 * la magnetizzazione uniforme è invariante per traslazione
		 * temporale quindi la calcolo solo a t = 0
		 */
		if ( !x[2] )
			cu += Reticolo::spin( x[0], x[1], x[2] ) - (double) 1/2;
	
		/* aggiorno la magnetizzazione alternata */
		cs ++;

		/* faccio evolvere le coordinate 'x[]' e misuro l'energia */
		msr.val[2] += Reticolo::next_ene( x[0], x[1], x[2] );

		/* inverto gli spin e controllo sovrapposizioni di loop */
		Reticolo::flip( x[0], x[1], x[2] );
	} while ( x[0] != n || x[1] != m || x[2] != t);

	/* aggiorno il valore della suscettività uniforme */
	msr.val[0] += cu * cu;
	
	/* aggiorno la suscettività alternata */
	msr.val[1] += (double) cs * cs;
} /* -----  end of method Reticolo::loop  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: flip
 * Description: inverte lo spin del sito (m, n, t) e lo "blocca".
 * ------------------------------------------------------------------
 */
void
Reticolo::flip ( unsigned int n, unsigned int m, unsigned int t ) {
	/* controllo eventuali sovrapposizioni di loop */
	if ( sito[n][m][t].lckd == lckd ) {
		fprintf( stderr, "[flip] due cluster passano per il sito (%u, %u, %u)\n", n, m, t);
		exit(EXIT_FAILURE);
	}
	/* blocco lo spin appena visitato */
	sito[n][m][t].lckd = lckd;
	
	/*
	 * inverto lo spin a seconda del valore della variabile globale 
	 * booleana 'flp' estratta a caso ad ogni loop
	 */
	sito[n][m][t].s = sito[n][m][t].s ^ flp;
} /* -----  end of method Reticolo::flip  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: get_spin
 * Description: restituisce lo spin del sito (n, m, t).
 * ------------------------------------------------------------------
 */
bool
Reticolo::get_spin (unsigned int n, unsigned int m, unsigned int t) {
	return sito[n % N][m % M][t % T].s;
} /* -----  end of method Reticolo::get_spin  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: get_lps
 * Description: restituisce il numero di loop di cui e' stato coperto
 * 				il reticolo nel corrente sweep. Bisogna "attivare" la
 * 				misurasione dei loop in 'Reticolo::fill()'.
 * ------------------------------------------------------------------
 */
unsigned int
Reticolo::get_lps ( void ) {
	return lps;
} /* -----  end of method Reticolo::get_lps  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: get_time_decays
 * Description: 
 * ------------------------------------------------------------------
 */
unsigned int
Reticolo::get_time_decay ( void ) {
	return tds;
} /* -----  end of method Reticolo::get_time_decays  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: get_msr
 * Description: misura i-esima al tempo markoviano corrente
 * ------------------------------------------------------------------
 */
double
Reticolo::get_msr (unsigned short int i = 0) {
	return msr.val[i % 3];
} /* -----  end of method Reticolo::get_msr  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: check
 * Description: controlla il sito (n, m, t) e decide in quale dire-
 * 				zione evolverà il loop quando vi arriva in base al
 * 				tipo di placca con cui ingeragisce il sito.
 * ------------------------------------------------------------------
 */
double
Reticolo::next_ene (unsigned int n, unsigned int m, unsigned int t) {
	/* calcolo le coordinate dei vertici della placca */
	Reticolo::plaque(n, m, t);

	/* controllo se l'evoluzione nel sito corrente è già stabilita */
	if ( sito[n][m][t].ckd == lckd ) {
		/* se la transizione è temporale */
		if ( sito[n][m][t].p )
			Reticolo::tt();
		else /* se transizione spaziale */
			Reticolo::st();

		/* non assegno energia alla placca */
		return (float) 0;
	} else {
		/* variabile ausiliaria per l'esponente */
		double c = (double) 4 * B * J / T;
		/* 
		 * controllo il tipo di placca, determino l'evoluzione e res-
		 * tituisco l'intera energia della placca
		 */
		switch ( Reticolo::type() ) {
			/* continuazione forzata */
			case 1: {
//				printf("CF: (%u, %u, %u)\n", xp[0][0], xp[0][1], xp[0][2]);
				Reticolo::forced_continuation();
				return (double) 1;
			}

			/* transizione forzata */
			case 2: {
//				printf("TF: (%u, %u, %u)\n", xp[0][0], xp[0][1], xp[0][2]);
				Reticolo::forced_transition();
				return ( 1. + 3. * exp(c) ) / ( 1. - exp(c) );
			}

			/* decadimento opzionale */
			case 3: {
//				printf("OD: (%u, %u, %u) ", xp[0][0], xp[0][1], xp[0][2]);
				Reticolo::optional_decay();
				return ( 1. - 3 * exp(c) ) / ( 1. + exp(c) );
			}
		}
	}

	/* solo in caso di placca non valida: stampa errore ed esce */
	fprintf( stderr, "[next_ene] Placca anomala: (%u, %u, %u)\n", xp[0][0], xp[0][1], xp[0][2]);
	exit(EXIT_FAILURE);

	/* evita errori di compilazione ma non viene _mai_ eseguito */
	return (double) 0;
} /* -----  end of method Reticolo::next_ene  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: type
 * Description: decide qual è il tipo di una placchetta. I possibili
 * 				risultati sono quattro:
 * 				1. continuazione forzata;
 * 				2. transizione forzata;
 * 				3. deadimento opzionale;
 * 				0. nessuna delle precedenti ( -> errore).
 * ------------------------------------------------------------------
 */
unsigned short int
Reticolo::type ( void ) {
	/* identifico le placchette con i valori di 'a' e 'b' */
	bool a = Reticolo::ps( 0 ) ^ Reticolo::ps( 1 );
	bool b = Reticolo::ps( 1 ) ^ Reticolo::ps( 2 );
	/* 
	 * 'a' e 'b' bastano ad identificare la placca: ma uso 'c' come 
	 * test di controllo in fase di debugging
	 */
//	bool c = Reticolo::ps( 0 ) ^ Reticolo::ps( 3 );

	if ( ( !a && !b ) /* && !c */ ) return 1; /* cont. forz. */
	else if ( ( a && !b ) /* && !c */ ) return 2; /* tran. forz. */
	else if ( ( a && b ) /* && c */ ) return 3; /* dec. op. */

	/* il valore di default è "non riconosciuta" */
	return (unsigned short) 0;
} /* -----  end of method Reticolo::type  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: plaque
 * Description: Calcolo le coordinate dei vertici della placca e il 
 * 				numero del vertice al quale corrisponde la posizione
 * 				attuale del loop. 
 * ------------------------------------------------------------------
 */
void
Reticolo::plaque ( unsigned int n, unsigned int m, unsigned int t) {
	/* assegno le coordinate al vertice in basso a dx della placca */
	xp[0][0] = n; xp[0][1] = m; xp[0][2] = t;
	/* in questo caso, il loop si trova sul vertice basso sx */
	i_x = 0;
	/* la placca è (ancora) orizzontale */
	v = false;

	/* 
	 * TODO cercare di comprimere il riconoscimento della placca 
	 *		includendo tutti i casi in uno soltanto (sftuttando il
	 *		valore dello spin)
	 */

	/* variabile ausiliaria */
	bool a;
	/* se lo spin era positivo prima del flip */
	if ( Reticolo::spin( n, m, t ) ) {
		/* se sono in una delle prime due "fette" temporali */
		if ( t % 4 == 0 || t % 4 == 1 ) {
			a = ( n + t ) % 2;
			/* (eventualmente) sposto indietro la coordinata 'x' */
			xp[0][0] = (unsigned) ( N - (signed) a + n ) % N;
			/* posizione loop sulla placca */
			i_x += (unsigned short) a;
		} else { /* if ( t % 4 == 2 || t % 4 == 3 ) */
			/* la placca diventa verticale */
			v = true;
			a = ( m % 2 ) ^ ( t % 2 );
			/* (eventualmente) modifico la coordinata 'y' */
			xp[0][1] = (unsigned) ( M - (signed) a + m ) % M;
			i_x += (unsigned short) a;
		}
	} else {
		/* torno indietro di una fetta temporale */
		xp[0][2] = ( T + t - 1 ) % T;
		/* il loop si trova a destra della placca */
		i_x = 2;

		/* seconda e terza "fette" temporali */
		if ( t % 4 == 1 || t % 4 == 2 ) {
			a = !( ( n + t ) % 2 );
			/* (eventualmente) sposto indietro la coordinata 'x' */
			xp[0][0] = ( N + n - (signed) a ) % N;
			/* posizione loop sulla placca */
			i_x += (unsigned short) a;
		} else { /* if ( t % 4 == 3 || t % 4 == 0 ) */
			/* la placca diventa verticale */
			v = true;
			a = (bool) ( m % 2 ) ^ !( t % 2 );
			/* (eventualmente) modifico la coordinata 'y' */
			xp[0][1] = (unsigned) ( M - (signed) a + m ) % M;
			i_x += (unsigned short) a;
		}
	}

	/* assegno le coordinate dei vertici della placca */
	for ( unsigned short int i = 0; i < 4; i ++ ) {
		xp[i][v] = (unsigned int) ( xp[0][v] + ( i % 2 ) ) % p[v];
		/* conserva la stessa coordinata x/y del vertice sotto a dx */
		xp[i][!v] = xp[0][!v];
		/* tempo */
		xp[i][2] = (unsigned int) ( xp[0][2] + (i / 2) ) % T;
	}
} /* -----  end of method Reticolo::plaque  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: pv
 * Description: Supponendo che i vertici del reticolo siano numerati
 * 				(partendo da '0' per quello in basso a sx, '1' per
 * 				quello in alto a sx, '2' per quello in basso a dx e 
 * 				'3' per il restante), questa funzione restituisce un 
 * 				puntatore al vertice 'i'.
 * ------------------------------------------------------------------
 */
Reticolo::Sito *
Reticolo::pv ( signed short int i ) {
	/* controllo che l'indice sia corretto */
	if ( i < 0 ) {
		fprintf( stderr, "[pv] indice negativo!\n" );
		exit(EXIT_FAILURE);
	} else if ( i > 3 ) {
		fprintf( stderr, "[pv] indice troppo alto!\n" );
		exit(EXIT_FAILURE);
	}

	/* restituisco il valore dello spin */
	return &sito[ xp[i][0] ][ xp[i][1] ][ xp[i][2] ];
} /* -----  end of method Reticolo::pv  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: ps
 * Description: Supponendo che i vertici del reticolo siano numerati
 * 				come sopra, questa funzione restituisce un lo spin
 * 				del vertice 'i'.
 * ------------------------------------------------------------------
 */
bool
Reticolo::ps ( signed short int i ) {
	/* controllo che l'indice sia corretto */
	if ( i < 0 ) {
		fprintf( stderr, "[ps] indice negativo!\n" );
		exit(EXIT_FAILURE);
	} else if ( i > 3 ) {
		fprintf( stderr, "[ps] indice troppo alto!\n" );
		exit(EXIT_FAILURE);
	}

	/* restituisco il valore dello spin */
	return Reticolo::spin( xp[i][0], xp[i][1], xp[i][2] );
} /* -----  end of method Reticolo::ps  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: tt
 * Description: modifica il segnaposto del loop per una transizione 
 * 				di tipo tempo.
 * ------------------------------------------------------------------
 */
void
Reticolo::tt ( void ) {
	/* aggiorno il numero di transizioni temporali */
	++ tds;
	x[2] = ( T + x[2] + ( 2 * Reticolo::spin( x[0], x[1], x[2] ) - 1 ) ) % T;
} /* -----  end of method Reticolo::tt  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: st
 * Description: modifica il segnaposto del loop per una transizione
 * 				di tipo spazio.
 * ------------------------------------------------------------------
 */
void
Reticolo::st ( void ) {
	x[v] = ( p[v] + x[v] + ( 2 * !( i_x % 2 ) - 1 ) ) % p[v];
} /* -----  end of method Reticolo::st  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: forced_continuation
 * Description: aggiorna le coordinate del loop compatibilmente con
 * 				una transizione di tipo "continuazione forzata".
 * ------------------------------------------------------------------
 */
void
Reticolo::forced_continuation ( void ) {
	/* 
	 * variabile ausiliaria:
	 * se '0 <= i < 2' fa zero; se '2 <= i < 4' fa due.
	 */
	unsigned short int i0 = 2 * ( i_x / 2 );
	/* assegno la transizione al sito non (ancora) visitato */
	for ( unsigned short int i = 0; i < 2; i ++ ) {
		/* registro che il sito è stato controllato */
		( * Reticolo::pv( i0 + i ) ).ckd = lckd;
		/* registro che la transizione è temporale */
		( * Reticolo::pv( i0 + i ) ).p = true;
	}

	/* faccio evolvere il loop */
	Reticolo::tt();
} /* -----  end of method Reticolo::forced_continuation  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: forced_transition
 * Description: aggiorna le coordinate del loop compatibilmente con
 * 				una transizione di tipo "transizione forzata".
 * ------------------------------------------------------------------
 */
void
Reticolo::forced_transition ( void ) {
	/* assegno le transizioni ai siti */
	for ( unsigned short int i = 0; i < 3; i += 2 ) {
		/* registro che il sito è stato controllato */
		( * Reticolo::pv( ( i_x + i ) % 4 ) ).ckd = lckd;
		/* registro che la transizione è temporale */
		( * Reticolo::pv( ( i_x + i ) % 4 ) ).p = false;
	}

	/* faccio evolvere il loop */
	Reticolo::st();
} /* -----  end of method Reticolo::forced_transition  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: optional_decay
 * Description: Aggiorna le variabili contenenti la posizione attuale
 * 				sul reticolo e restituisce i valori:
 * 				0. Non visitata e trans. spaziale;
 * 				1. Non isitata e trans. temporale;
 * 				2. Visitata e trans. spaziale;
 * 				3. Visitata e trans. temporale.
 * ------------------------------------------------------------------
 */
void
Reticolo::optional_decay ( void ) {
	/* estraggo la transizione */
	bool trans = ( (long double) rand() / RAND_MAX < Reticolo::prob() );

	/* variabile ausiliaria */
	unsigned short int i0 = 2 * ( ( i_x % 2 ) ^ Reticolo::ps( i_x ) ) + 1;
	for ( unsigned short int i = 0; i < 2; i ++ ) {
		/* registro che il sito è stato controllato */
		( * Reticolo::pv( ( i0 + i ) % 4 ) ).ckd = lckd;
		/* registro che la transizione è temporale */
		( * Reticolo::pv( ( i0 + i ) % 4 ) ).p = trans;
	}

	/* aggiorno la posizione del loop */
	if ( trans ) Reticolo::tt();
	else Reticolo::st();
} /* -----  end of method Reticolo::optional_decay  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: prob
 * Description: ritorna la probabilità di transizione temporale nella
 * 				placca di tipo "decadimento opzionale".
 * ------------------------------------------------------------------
 */
long double
Reticolo::prob ( void ) {
	return (long double) 2/( 1 + expl( (long double) 4 * B * J / T ) );
} /* -----  end of method Reticolo::prob  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: r(ound)
 * Description: arrotonda media ed errore alle stesse cifre signifi-
 * 				cative (in modo che abbiano senso statisticamente) e
 * 				li stampa a schermo.
 * 				NOTA: 'msr.sdom[i]' è sempre positivo.
 * ------------------------------------------------------------------
 */
void
Reticolo::r ( unsigned short int i ) {
	/*
	 * incremento dell'esponente: se 'msr.sdom[i] >= 10', cerco tra
	 * gli esponenti più alti di 0
	 */
	signed short int j = 2 * ( msr.sdom[i] >= 10 ) - 1;

	signed short int e = 0;
	/* calcolo le cifre significative */
	while ( ( pow(10, 1 + e) < msr.sdom[i] ) ^ ( msr.sdom[i] <= pow(10, e) ) )
		e += j;

	/* controllo se tenere una o due cifre decimali */
	if ( msr.sdom[i] / pow(10, e) < 3 )
		e += j;

	/* arrotondo (le copie di) errore e media */
//	double k = floorf(msr.sdom[i] / pow(10., e) + .5) * pow(10., e);
//	double g = floorf(msr.mean[i] / pow(10., e) + .5) * pow(10., e);

	/* stampo a schermo */
	printf( "%f\t%f\t",
			floorf(msr.mean[i] / pow(10., e) + .5) * pow(10., e),
			floorf(msr.sdom[i] / pow(10., e) + .5) * pow(10., e)
	);
} /* -----  end of method Reticolo::r  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: mean
 * Description: normalizza le medie e calcola gli errori delle misure
 * ------------------------------------------------------------------
 */
void
Reticolo::mean ( void ) {
	/* normalizzo le medie e calcolo gli errori */
	for ( unsigned short int i = 0; i < 3; i ++ ) {	
		msr.mean[i] = (long double) msr.mean[i] / msr.lenght;
		msr.sdom[i] = (long double) msr.sdom[i] / msr.lenght;
		/* uso la correzione di Bessel */
		msr.sdom[i] = sqrtl( ( long double) ( msr.sdom[i] - pow( msr.mean[i], 2 ) ) / ( msr.lenght - 1 ) );
	}
} /* -----  end of method Reticolo::mean  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: print_results
 * Description: sistemo i coefficienti e stampo le misure. 
 * ------------------------------------------------------------------
 */
void
Reticolo::print_results ( void ) {
	/* suscettività uniforme */
	msr.mean[0] = ((long double) B / ( N * M )) * msr.mean[0];
	msr.sdom[0] = ((long double) B / ( N * M )) * msr.sdom[0];

	/* suscettività alternata */
	msr.mean[1] = ((long double) B / ( 4 * N * M * T * T )) * msr.mean[1];
	msr.sdom[1] = ((long double) B / ( 4 * N * M * T * T )) * msr.sdom[1];

	/* energia */
	msr.mean[2] = ((long double) J / ( M * N * T )) * msr.mean[2];
	msr.sdom[2] = ((long double) J / ( M * N * T )) * msr.sdom[2];

	/* stampo i risultati sotto forma di tabella */	
	printf( "# RISULTATI (N. sweep: %u)\n"
			"#J\tB\tN\tM\tT\tUS\t\terr\t\tSS\t\terr\t\tEN\t\terr\n"
			"%u\t%u\t%u\t%u\t%u\t", msr.lenght, J, B, N, M, T );
	for ( unsigned int i = 0; i < 3; i++ )
		Reticolo::r(i);

	printf("\n");
} /* -----  end of method Reticolo::print_results  ----- */
