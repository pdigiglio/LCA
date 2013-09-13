#include "./reticolo_3d.h"

#include <cmath>
/* per l'operatore bitwise XOR (^) */
#include <ciso646>

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: Reticolo
 * Description: [ctor] costruisco il reticolo di spin.
 * ------------------------------------------------------------------
 */
Reticolo::Reticolo ( void ) {
	/* creo il reticolo */
	sito = (struct Sito ***) malloc ( N * sizeof(struct Sito **) );
	if ( sito == NULL ) {
		fprintf ( stderr, "[ctor] (0) dynamic memory reallocation failed\n" );
		exit (EXIT_FAILURE);
	}

	#pragma omp parallel for
	for ( unsigned short int n = 0; n < N; n ++ ) {
		sito[n] = (struct Sito **) malloc ( M * sizeof( struct Sito *) );
		if ( sito[n] == NULL ) {
			fprintf ( stderr, "[ctor] (1) dynamic memory allocation failed\n" );
			exit (EXIT_FAILURE);
		}
		
		for ( unsigned short int m = 0; m < M; m ++ ) {
			sito[n][m] = (struct Sito *) malloc ( T * sizeof(struct Sito) );
			if ( sito[n][m] == NULL ) {
				fprintf ( stderr, "[ctor] (2) dynamic memory allocation failed\n" );
				exit (EXIT_FAILURE);
			}

			/* inizializzo i valori */
			for ( unsigned short int t = 0; t < T; t ++ ) {
				sito[n][m][t].s = true;
				sito[n][m][t].lckd = lckd;
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
	/* azzero il numero di loop nello sweep corrente */
//	lps = 0;

	/* aggiorno il numero di misure (e sweep) */
	msr.lenght ++;
	/* calcolo il valore della variabile lckd */
	lckd = (bool) ( msr.lenght % 2 );

	/* inizializzo a zero le suscettività */
	for ( unsigned short int i = 0; i < 3; i ++ )
		msr.val[i] = (float) 0;

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
				 * se il sito (n, m, t) non appartiene ad alcun cluster:
				 * aggiorno il numero do loop
				 */
//				lps ++;

				/* faccio partire un loop dal sito (n, m, t) */
				loop(n, m, t);
			}

	/* aggiorno le medie (coi valori calcolati in Reticolo::loop()) */
	for ( unsigned short int i = 0; i < 3; i ++ ) {
		msr.mean[i] += msr.val[i];
		msr.sdom[i] += pow( msr.val[i], 2 );
	}
} /* -----  end of method Reticolo::fill  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: spin
 * Description: restituisce lo spin nel sito (n, m, t) prima del flip.
 * ------------------------------------------------------------------
 */
bool
Reticolo::spin ( unsigned int n, unsigned int m, unsigned int t ) {
	/* spin del sito */
	bool value = sito[n][m][t].s;
	/* se il sito è stato invertito */
	if ( sito[n][m][t].flip && ( sito[n][m][t].lckd == lckd ) )
		value = !value;

	return (bool) value;
} /* -----  end of method Reticolo::spin  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: loop
 * Description: faccio partire il loop dal sito (n, m, t) e decido se
 * 				invertire i suoi spin in maniera casuale. 
 * ------------------------------------------------------------------
 */
void
Reticolo::loop ( unsigned int n, unsigned int m, unsigned int t ) {
	/* 'rand() % 2' vale '0' oppure '1' quindi è un booleano casuale */
 	bool f = (bool) ( rand() % 2 );

	/* assegno il punto di partenza */
	x[0] = n; x[1] = m; x[2] = t;

	/* magnetizzazione uniforme */
	float cu = (float) 0;
	/* magnetizzazione alternata */
	unsigned int cs[T];
	for ( unsigned short int k = 0; k < T; k ++ )
		cs[k] = 0;

	/* faccio evolvere il loop finche' non torno al p.to di partenza */
	do {
		/* 
		 * la magnetizzazione uniforme è invariante per traslazione
		 * temporale quindi la calcolo solo a t = 0
		 */
		if ( x[2] == 0 )
			cu += Reticolo::spin(x[0], x[1], x[2]) - (float) 1/2;

		/* aggiorno la magnetizzazione alternata */
		cs[ x[2] ] ++;

		/* 
		 * (eventualmente) inverto gli spin. Questa operazione deve
		 * essere eseguita prima di 'Reticolo::next_ene()', altrimenti
		 * ci sono degli errori.
		 */
		Reticolo::flip(x[0], x[1], x[2], f);
		
		/*
		 * faccio evolvere le coordinate 'x[]' e misuro l'energia.
		 * Sommo direttamente nella variabile che contiene la media
		 * perché non devo eseguire potenze sull'energia.
		 */
		msr.val[2] += Reticolo::next_ene(x[0], x[1], x[2]);
	} while ( x[0] != n || x[1] != m || x[2] != t);

	/* aggiorno il valore della suscettività uniforme */
	msr.val[0] += (float) pow(cu, 2);
	
	/* aggiorno la suscettività alternata */
	for ( unsigned short int k = 0; k < T; k ++ )
		msr.val[1] += (float) pow(cs[k], 2);
} /* -----  end of method Reticolo::loop  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: flip
 * Description: inverte lo spin del sito (m,n,t) e registra l'inver-
 * 				sione nel campo 'sito[][][].flip'.
 * ------------------------------------------------------------------
 */
void
Reticolo::flip (unsigned int n, unsigned int m, unsigned int t, bool f) {
	/* inverto lo spin */
	if ( f )
		sito[n][m][t].s = !( sito[n][m][t].s );

	/* registro l'inversione (o meno) */
	sito[n][m][t].flip = f;
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
Reticolo::get_lps (void) {
	return lps;
} /* -----  end of method Reticolo::get_lps  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: check
 * Description: controlla il sito (n, m, t) e decide in quale dire-
 * 				zione evolverà il loop quando vi arriva in base al
 * 				tipo di placca con cui ingeragisce il sito.
 * ------------------------------------------------------------------
 */
float
Reticolo::next_ene (unsigned int n, unsigned int m, unsigned int t) {
	/* controllo eventuali sovrapposizioni di loop */
	if ( sito[n][m][t].lckd == lckd ) {
		fprintf( stderr, "Errore: due cluster passano per il sito (%u, %u, %u)\n", n, m, t);
		exit(EXIT_FAILURE);
	}

	/* blocco lo spin appena visitato */
	sito[n][m][t].lckd = lckd;

	/* calcolo le coordinate dei vertici della placca */
	Reticolo::plaque(n, m, t);

	/* esponente */
	float c = (float) 4 * B * J / T;
//	float energy = (float) 0;
	/* calcolo gli spostamenti e le energie delle placche */
	switch ( Reticolo::type() ) {
		/* continuazione forzata */
		case 1: {
//			printf("CF: (%u, %u, %u)\n", xp[0][0], xp[0][1], xp[0][2]);
			/* se gia' visitata */
			if ( Reticolo::forced_continuation() )
				return (1. + 3.*exp(c)) / (1. + exp(c));
			else /* se prima visita */
				return (1. - exp(c)) / (1. + exp(c));
			
//			printf("%f\n", energy);
//			return energy;
		}

		/* transizione forzata */
		case 2: {
//			printf("TF: (%u, %u, %u)\n", xp[0][0], xp[0][1], xp[0][2]);
			/* se gia' visitata */
			if ( Reticolo::forced_transition() ) 
				return (1. + 8.*exp(c) + 3.*exp(2.*c)) / (1. - exp(2.*c));
			else /* se prima visita */
				return (1. + 3.*exp(2.*c)) / (1. - exp(2.*c));

//			printf("%f\n", energy);
//			return energy;
		}

		/* decadimento opzionale */
		case 3: {
//			printf("OD: (%u, %u, %u)\n", xp[0][0], xp[0][1], xp[0][2]);
			switch ( Reticolo::optional_decay() ) {
				/* non visitata e trans. spaziale */
				case 0:
					return (1. + 3.*exp(2.*c)) / (1. - exp(2.*c));
//					break;

				/* non visitata e trans. temporale */
				case 1:
					return (1. - exp(c)) / (1. + exp(c));
//					break;

				/* visitata e trans. spaziale */
				case 2:
					return (1. - 8.*exp(c) + 3.*exp(2.*c)) / (1. - exp(2.*c));
//					break;

				/* visitata e trans. temporale */
				case 3:
					return (1. - 5.*exp(c)) / (1. + exp(c));
//					break;
			}
//			printf("%f\n", energy);
//			return energy;
		}
	}

	/* solo in caso di placca non valida: stampa errore ed esce */
	fprintf( stderr, "[next_ene] Trovata placca anomala! (%u, %u, %u)\n", xp[0][0], xp[0][1], xp[0][2]);
	/* stampo gli spin della placca */
	for ( unsigned short int j = 0; j < 4; j ++ )
		printf(
				"(%u, %u, %u): spin() %hu .s %hu\n",
				xp[j][0], xp[j][1], xp[j][2],
				Reticolo::spin(xp[j][0], xp[j][1], xp[j][2]),
				sito[ xp[j][0] ][ xp[j][1] ][ xp[j][2] ].s);
	/* esco */
	exit(EXIT_FAILURE);

	/* evita errori di compilazione ma non viene mai eseguito */
	return (float) 0;
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
Reticolo::type (void) {
	/* decido che tipo di placca ho */
	bool r_0 = Reticolo::p_s( 0 ); //xp[0][0], xp[0][1], xp[0][2]); /* basso sx */
	bool r_1 = Reticolo::p_s( 1 ); //spin(xp[1][0], xp[1][1], xp[1][2]); /* alto sx */
	bool r_2 = Reticolo::p_s( 2 ); //spin(xp[2][0], xp[2][1], xp[2][2]); /* basso dx */

	/* identifico le placchette con i valori di 'a' e 'b' */
	bool a = r_0 ^ r_1, b = r_1 ^ r_2;

	if ( !a && !b ) return 1; /* cont. forz. */
	else if ( a && !b ) return 2; /* tran. forz. */
	else if ( a && b ) return 3; /* dec. op. */

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
Reticolo::plaque (unsigned int n, unsigned int m, unsigned int t) {
	/* assegno le coordinate al vertice in basso a dx della placca */
	xp[0][0] = (unsigned) n;
	xp[0][1] = (unsigned) m;
	xp[0][2] = (unsigned) t;
	/* in questo caso, il loop si trova sul vertice basso dx */
	i_x = 0;

	/* 
	 * variabile ausiliaria utile per stabilire se la placca è oriz-
	 * zontale o verticale (inizializzata ad "orizzontale").
	 */
	bool v = false;
	/* variabile ausiliaria */
	bool a;

	/* 
	 * TODO cercare di comprimere il riconoscimento della placca 
	 *		includendo tutti i casi in uno soltanto (sftuttando il
	 *		valore dello spin)
	 */

	/* se lo spin era positivo prima del flip */
	if ( Reticolo::spin(n, m, t) ) {
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
			a = ( ( m % 2 ) ) ^ ( ( t % 4 ) % 2 );
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
			a = ! ( ( n + t ) % 2);
			/* (eventualmente) sposto indietro la coordinata 'x' */
			xp[0][0] = ( N + n - (signed) a ) % N;
			/* posizione loop sulla placca */
			i_x += (unsigned short) a;
		} else { /* if ( t % 4 == 3 || t % 4 == 0 ) */
			/* la placca diventa verticale */
			v = true;
			a = (bool) ( ( m % 2 ) ) ^ !( ( t % 4 ) % 2 );
			/* (eventualmente) modifico la coordinata 'y' */
			xp[0][1] = (unsigned) ( M - (signed) a + m ) % M;
			i_x += (unsigned short) a;
		}
	}

//	printf("%hu\n", i_x);

	/* assegno le coordinate dei vertici della placca */
	for ( unsigned short int i = 0; i < 4; i++ ) {
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
 *      Method: p_s
 * Description: Supponendo che i vertici del reticolo siano numerati
 * 				(partendo da '0' per quello in basso a sx, '1' per
 * 				quello in alto a sx, '2' per quello in basso a dx e 
 * 				'3' per il restante), questa funzione restituisce lo
 * 				spin del vertice 'i'.
 * ------------------------------------------------------------------
 */
bool
Reticolo::p_s ( signed short int i ) {
	/* controllo che l'indice sia corretto */
	if ( i < 0 ) {
		fprintf( stderr, "[p_s] indice negativo!\n" );
		exit(EXIT_FAILURE);
	} else if ( i > 3 ) {
		fprintf( stderr, "[p_s] indice troppo alto!\n" );
		exit(EXIT_FAILURE);
	}

	/* restituisco il valore dello spin */
	return Reticolo::spin( xp[i][0], xp[i][1], xp[i][2] );
} /* -----  end of method Reticolo::p_s  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: forced_continuation
 * Description: aggiorna le coordinate del loop compatibilmente con
 * 				una transizione di tipo "continuazione forzata".
 * ------------------------------------------------------------------
 */
bool
Reticolo::forced_continuation ( void ) {
	/* variabili ausiliarie */
	bool visited = true;
	bool s = Reticolo::p_s( i_x ); //spin(x[0], x[1], x[2]);

	/* controllo se la placca e' stata gia' visitata */
	unsigned short int i0 = 2 * !s;
	bool a = sito[ xp[i0][0] ][ xp[i0][1] ][ xp[i0][2] ].lckd;
	bool b = sito[ xp[i0 + 1][0] ][ xp[i0 + 1][1] ][ xp[i0 + 1][2] ].lckd;
	if ( a ^ b )
		visited = false;

	/* salvo l'incremento in 'i0' */
	i0 = 2 * s - 1;
	/* incremento la posizione lungo il tempo */
	x[2] = (unsigned) ( T + x[2] + (int) i0 ) % T;

	return visited;
} /* -----  end of method Reticolo::forced_continuation  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: forced_transition
 * Description: aggiorna le coordinate del loop compatibilmente con
 * 				una transizione di tipo "transizione forzata".
 * ------------------------------------------------------------------
 */
bool
Reticolo::forced_transition (void) {
	/* variabile ausiliaria */
	bool visited = true;

	/* controllo se la placca e' stata gia' visitata */
	short int ic = (short) 2 * Reticolo::spin( x[0], x[1], x[2] ) - 1;
	if ( sito[ x[0] ][ x[1] ][ x[2] ].lckd ^ sito[ x[0] ][ x[1] ][ ( T + x[2] + ic ) % T ].lckd )
		visited = false;

	/* controllo se la placca è orizzontale o verticale */
	bool v = (bool) ( xp[0][0] == xp[1][0] );
	/* incremento la posizione lungo lo spazio */
	ic = (short) 2 * Reticolo::p_s( 0 ) - 1;
	x[v] = (unsigned) ( p[v] + x[v] + (int) ic ) % p[v];

	return visited;
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
unsigned short int
Reticolo::optional_decay (void) {
	/* decido qual e' il vertice da controllare */
	unsigned short int i = 2 * Reticolo::spin(xp[0][0], xp[0][1], xp[0][2]) + 1;
	i += Reticolo::spin(x[0], x[1], x[2]) == Reticolo::spin(xp[1][0], xp[1][1], xp[1][2]);
	i = i % 4;

	/* variabili ausiliarie */
	bool t = Reticolo::spin(x[0], x[1], x[2]);
	/* se 'sito[][][].p = 1', transizinone temporale */
	bool a = true;

	bool visited = false;
	/* controllo se la placca e' stata visitata */
	if ( sito[ xp[i][0] ][ xp[i][1] ][ xp[i][2] ].lckd == lckd ) {
		/* se placca visitata e transizione spaziale */
		if ( !( sito[ xp[i][0] ][ xp[i][1] ][ xp[i][2] ].p ) ) {
			/* cambio la variabile ausiliaria per l'incremento */
			t = Reticolo::spin(x[0], x[1], x[2]) == Reticolo::spin(xp[0][0], xp[0][1], xp[0][2]);
			/* salvo la transizione */
			a = false;
		}
		/* modifico lo stato di "placca visitata" */
		visited = true;
	} else {
		/* se placca non visitata "estraggo" la transizione */
		if ( (long double) rand()/RAND_MAX > Reticolo::prob() ) {
			/* cambio la variabile ausiliaria per l'incremento */
			t = Reticolo::spin(x[0], x[1], x[2]) == Reticolo::spin(xp[0][0], xp[0][1], xp[0][2]);
			/* salvo la transizione */
			a = false;
		}
	}

	/* incremento */
	signed short int ic = (signed) 2 * t - 1;
	sito[ x[0] ][ x[1] ][ x[2] ].p = a;

	/* controllo se la placca è orizzontale o verticale */
	bool v = (bool) ( xp[0][0] == xp[1][0] );
	/*
	 * assegno l'indice della coordinata da aggiornare:
	 *  - (a = 1) -> i = 2;
	 *  - (a = 0) -> i = v;
	 */
	i = ( (short) 2 * a + (v && !a) ); 
	/* effettuo l'incremento */
	x[i] = (unsigned) ( p[i] + x[i] + (int) ic ) % p[i];

	return (unsigned short int) 2 * visited + a;
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
Reticolo::prob (void) {
	return (long double) 2/( 1 + expl( (long double) 4*B*J/T ) );
} /* -----  end of method Reticolo::prob  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: r(ound)
 * Description: arrotonda media ed errore alle stesse cifre signifi-
 * 				cative (in modo che abbiano senso statisticamente) e
 * 				li stampa a schermo.
 * ------------------------------------------------------------------
 */
void
Reticolo::r (unsigned short int i) {
	/* NOTA: 'msr.sdom[i]' è positivo */
	unsigned short int e = 0;
	/* calcolo le cifre significative */
	while ( ( pow(10, 1 - e) < msr.sdom[i] ) ^ ( msr.sdom[i] <= pow(10, -e) ) )
		e ++;

	/* controllo se tenere una o due cifre decimali */
	if ( msr.sdom[i] * pow(10, e) < 3 )
		e ++;

	/* arrotondo (le copie di) errore e media */
	float k = floorf(msr.sdom[i] * pow(10., e) + 0.5) / pow(10., e);
	float g = floorf(msr.mean[i] * pow(10., e) + 0.5) / pow(10., e);

	/* stampo a schermo */
	printf("%f\t%f\t", g, k);
} /* -----  end of method Reticolo::r  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: mean
 * Description: normalizza le medie e calcola gli errori delle misure
 * ------------------------------------------------------------------
 */
void
Reticolo::mean (void) {
	/* normalizzo le medie e calcolo gli errori */
	for ( unsigned short int i = 0; i < 3; i ++ ) {	
		msr.mean[i] = (float) msr.mean[i] / msr.lenght;
		msr.sdom[i] = (float) msr.sdom[i] / msr.lenght;
		msr.sdom[i] = sqrt( ( msr.sdom[i] - pow( msr.mean[i], 2 ) ) / msr.lenght );
	}
} /* -----  end of method Reticolo::mean  ----- */

///*
// * ------------------------------------------------------------------
// *       Class: Reticolo
// *      Method: auto_corr
// * Description: 
// * ------------------------------------------------------------------
// */
//void
//Reticolo::auto_corr ( short int i = 0, unsigned short int nMin = 0, unsigned short int nMax = 100 ) {
//	/* controllo che si sia selezionata una grandezza valida */
//	if ( i < 0 || i > 2 ) {
//		fprintf(stderr, "Autocorrelator: \'i = %u\' out of range, switching to 0!\n", i);	
//		i = 0;
//	}
//
//	/* controllo che il numero di sweep sia sufficiente */
//	if ( (signed int) msr.lenght - nMax < 0 ) {
//		fprintf(stderr, "Autocorrelator: too few sweeps, changing domain!\n");
//		nMax = msr.lenght - 1;
//	}
//
//	/* creo l'autocorrelatore */
//	float temp, norm = (float) 1;
//	for ( unsigned int t = nMin; t < nMax; t ++ ) {
//		temp = (float) 0;
//		for ( unsigned int s = 0; s < msr.lenght - t; s ++ )
//			temp += msr.val[s][i] * msr.val[s + t][i];
//
//		/* salvo il primo valore per normalizzare gli altri */
//		if ( t == 0 )
//			norm = temp/(msr.lenght - t) - pow(msr.mean[i], 2);
//		
//		/* stampo i valori normalizzati */
//		printf("%u\t%f\n", t, (float) ( temp/(msr.lenght - t) - pow(msr.mean[i], 2) ) / norm);
//	}
//} /* -----  end of method Reticolo::auto_corr  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: print_results
 * Description: sistemo i coefficienti e stampo le misure. 
 * ------------------------------------------------------------------
 */
void
Reticolo::print_results (void) {
	/* suscettività uniforme */
	msr.mean[0] = (float) B * msr.mean[0]/((float) N * M );
	msr.sdom[0] = (float) B * msr.sdom[0]/((float) N * M );

	/* suscettività alternata */
	msr.mean[1] = (float) B * msr.mean[1]/((float) 4 * N * M * T );
	msr.sdom[1] = (float) B * msr.sdom[1]/((float) 4 * N * M * T );

	/* energia */
	msr.mean[2] = ((float) J / (2 * M * N * T )) * msr.mean[2];
	msr.sdom[2] = ((float) J / (2 * M * N * T )) * msr.sdom[2];

	/* stampo i risultati sotto forma di tabella */	
	printf( "# RISULTATI (N. sweep: %u)\n"
			"#J\tB\tN\tM\tT\tUS\t\terr\t\tSS\t\terr\t\tEN\t\terr\n"
			"%u\t%u\t%u\t%u\t%u\t", msr.lenght, J, B, N, M, T );

	for ( unsigned int i = 0; i < 3; i++ )
		Reticolo::r(i);

	printf("\n");
} /* -----  end of method Reticolo::print_results  ----- */
