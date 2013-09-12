#include "./reticolo_2d.h"

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
	/* costruisco il reticolo */
	sito = (struct Sito **) malloc ( N * sizeof(struct Sito *) );
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
		sito[n] = (struct Sito *) malloc ( M * sizeof(struct Sito) );
		if ( sito[n] == NULL ) {
			fprintf ( stderr, "[ctor] (1) dynamic memory allocation failed\n" );
			exit (EXIT_FAILURE);
		}

		for ( unsigned short int m = 0; m < M; m ++ ) {
			sito[n][m].s = (bool) ( ( m + n ) % 2);
			sito[n][m].lckd = lckd;
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
Reticolo::~Reticolo (void) {
} /* -----  end of method Reticolo::~Reticolo (dtor)  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: fill
 * Description: Copre il reticolo di loop disgiunti.
 * ------------------------------------------------------------------
 */
void
Reticolo::fill (void) {
	/* azzero il numero di loop nello sweep corrente */
//	lps = 0;

	/* aggiorno il numero di misure */
	msr.lenght ++;
	/* calcolo il valore del campo 'lckd' dei siti */
	lckd = msr.lenght % 2;

	/* resetto le variabili */
	for ( unsigned short int i = 0; i < 3; i ++ )
		msr.val[i] = (float) 0;

	/* copro l'intero reticolo con i cluster */
	for ( unsigned int n = 0; n < N ; n ++ )
		for ( unsigned int m = 0; m < M ; m ++ ) {
			/*
			 * controllo se il sito (n,m) e' gia' contenuto in qual-
			 * che cluster, nel qual caso salto le istruzioni seguen-
			 * ti e vado al prossimo passo nel ciclo
			 */
			if ( sito[n][m].lckd == lckd ) continue;

			/*
			 * se il sito (n,m) non appartiene ad alcun cluster:
			 * aggiorno il numero do loop
			 */
//			lps++;

			/* faccio partire un loop dal sito (n,m) */
			loop(n, m);
		}

	/* aggiorno le medie e le sdom */
	for ( unsigned short int i = 0; i < 3; i ++ ) {
		msr.mean[i] += msr.val[i];
		msr.sdom[i] += pow( msr.val[i], 2 );
	}
} /* -----  end of method Reticolo::fill  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: measure
 * Description: Misuro la suscettività uniforme ed alternata tramite
 * 				le definizioni "classiche" (non migliorate).
 * ------------------------------------------------------------------
 */
//void
//Reticolo::measure (void) {
//	float temp = (float) 0;
//	float temp1 = (float) 0;
//
//	for ( unsigned int t = 0; t < M; t ++ ) {
//		temp = (float) 0;
////		temp1 = (float) 0;
//		for ( unsigned int x = 0; x < N; x ++ ) {
//			temp += sito[x][t].s - (float) 1/2;
////			temp += pow(-1, x)*(sito[x][t].s - (float) 1/2);
//		}
////		msr.val[msr.lenght - 1][0] += pow(temp, 2)/4;
////		msr.val[msr.lenght - 1][1] += B*pow(temp1, 2)/(N*M);
//		temp1 += B*pow(temp, 2)/(N*M);
//	}
//
//	printf("%u\t%f\n", msr.lenght -1, temp1);
//	
////	msr.mean[0] += msr.val[msr.lenght - 1][0];
////	msr.mean[1] += msr.val[msr.lenght - 1][1];
//} /* -----  end of method Reticolo::measure  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: print
 * Description: stampa a schermo una rappresentazione stilizzata del 
 * 				reticolo usando '1' e '0' per identificare gli spin
 * 				su e giu', rispettivamente. Il tempo scorre in dire-
 * 				zione orizzontale.
 * ------------------------------------------------------------------
 */
void
Reticolo::print (void) {
	for ( signed int n = N - 1; n >= 0; n-- ) {
		printf("n: %u\t", n);
		for ( unsigned int t = 0; t < M; t++ )
			printf("%hu ", (unsigned short) sito[n][t].s);

		/* vado a capo alla fine della riga temporale */
		printf("\n");
	}
} /* -----  end of method Reticolo::print  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: spin
 * Description: restituisce lo spin nel sito (n,m) prima del flip.
 * ------------------------------------------------------------------
 */
bool
Reticolo::spin (unsigned int n, unsigned int m) {
	/* controllo che 'n' e 'm' non siano fuori range */
	if ( N < n ) {
		fprintf(stdout, "[spin] Variabile n = %u fuori range!\n", n);
		n = n % N;
	} else if ( M < m ) {
		fprintf(stdout, "[spin] Variabile m = %u fuori range!\n", m);
		m = m % N;
	}

	/* spin del sito */
	bool value = sito[n][m].s;
	/* se il sito è stato invertito */
	if ( sito[n][m].flip && ( sito[n][m].lckd == lckd ) )
		value = !value;

	return (bool) value;
} /* -----  end of method Reticolo::spin  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: loop
 * Description: faccio partire il loop da un punto a caso. 
 * ------------------------------------------------------------------
 */
void
Reticolo::loop (unsigned int n, unsigned int m) {
	/* assegno il punto di partenza */
	x[0] = n; x[1] = m;

	/* NOTA: rand() % 2 = 0, 1 quindi è un booleano casuale */
	bool f = (bool) ( rand() % 2 );

	/* magnetizzazione uniforme */
	float cu = (float) 0;
	/* magnetizzazione alternata */
	unsigned int cs[M];
	for ( unsigned int t = 0; t < M; t ++ )
		cs[t] = 0;

	/* faccio evolvere il loop finche' non torno al p.to di partenza */
	do {
		/* (eventualmente) inverto gli spin */
		Reticolo::flip(x[0], x[1], f );

		/* 
		 * la magnetizzazione uniforme è invariante per traslazione
		 * temporale quindi la calcolo solo a t = 0
		 */
		if ( x[1] == 0 )
			cu += Reticolo::spin(x[0], x[1]) - (float) 1/2;

		/* calcolo la magnetizzazione alternata */
		cs[ x[1] ] ++;
		/* faccio evolvere le coordinate 'x[]' e misuro l'energia */
		msr.val[2] += Reticolo::next_ene( x[0], x[1] );
	} while ( x[0] != n || x[1] != m );

	/* aggiorno i valori delle misre */
	if ( cu /* != 0 */ )
		msr.val[0] += (float) pow(cu, 2);
	
	for ( unsigned int t = 0; t < M; t ++ )
		msr.val[1] += (float) pow( cs[t], 2. );
} /* -----  end of method Reticolo::loop  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: flip
 * Description: inverte lo spin del sito (m,n).
 * ------------------------------------------------------------------
 */
void
Reticolo::flip (unsigned int n, unsigned int m, bool f) {
	/* inverto lo spin */
	if ( f )
		sito[n][m].s = !( sito[n][m].s );

	/* registro l'inversione (o meno) */
	sito[n][m].flip = f;
} /* -----  end of method Reticolo::flip  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: get_spin
 * Description: restituisce lo spin del sito (n,m).
 * ------------------------------------------------------------------
 */
bool
Reticolo::get_spin (unsigned int n, unsigned int m) {
	return sito[n % N][m % M].s;
} /* -----  end of method Reticolo::get_spin  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: get_lps
 * Description: restituisce il numero di loop di cui e' stato coperto
 * 				il reticolo nel corrente sweep.
 * ------------------------------------------------------------------
 */
unsigned int
Reticolo::get_lps (void) {
	return lps;
} /* -----  end of method Reticolo::get_lps  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: get_msr
 * Description: misura i-esima al tempo markoviano corrente
 * ------------------------------------------------------------------
 */
float
Reticolo::get_msr (unsigned short int i = 0) {
	return msr.val[i % 3];
} /* -----  end of method Reticolo::get_msr  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: check
 * Description: calcola il punto successivo del loop e restituisce la
 * 				energia della placca interagente con il sito (n, m).
 * ------------------------------------------------------------------
 */
float
Reticolo::next_ene (unsigned int n, unsigned int m) {
	/* controllo eventuali sovrapposizioni di loop */
	if ( sito[n][m].lckd == lckd ) {
		fprintf(stderr, "Errore: due cluster passano per il sito (%u, %u)\n", n, m);
		exit(EXIT_FAILURE);
	}

	/* blocco lo spin appena visitato */
	sito[n][m].lckd = lckd;

	/* calcolo le coordinate dei vertici della placca */
	Reticolo::plaque(n, m);

	/* esponente */
	float t = (float) 2*B*J/M, energy = (float) 0;
	/* calcolo gli spostamenti e le energie delle placche */
	switch ( Reticolo::type() ) {
		/* continuazione forzata */
		case 1: {
//			printf("CF: (%u, %u)\n", np[0], mp[0]);
			/* se gia' visitata */
			if ( Reticolo::forced_continuation() )
				energy = (1. + 3.*exp(t)) / (1. + exp(t));
			else /* se prima visita */
				energy = (1. - exp(t)) / (1. + exp(t));
			
//			printf("%f\n", energy);
			return energy;
		}

		/* transizione forzata */
		case 2: {
//			printf("TF: (%u, %u)\n", np[0], mp[0]);
			/* se gia' visitata */
			if ( Reticolo::forced_transition() ) 
				energy = (1. + 8.*exp(t) + 3.*exp(2.*t)) / (1. - exp(2.*t));
			else /* se prima visita */
				energy = (1. + 3.*exp(2.*t)) / (1. - exp(2.*t));

//			printf("%f\n", energy);
			return energy;
		}

		/* decadimento opzionale */
		case 3: {
//			printf("OD: (%u, %u)\n", np[0], mp[0]);
			switch ( Reticolo::optional_decay() ) {
				/* non visitata e trans. spaziale */
				case 0:
					energy = (1. + 3.*exp(2.*t)) / (1. - exp(2.*t));
					break;

				/* non visitata e trans. temporale */
				case 1:
					energy = (1. - exp(t)) / (1. + exp(t));
					break;

				/* visitata e trans. spaziale */
				case 2:
					energy = (1. - 8.*exp(t) + 3.*exp(2.*t)) / (1. - exp(2.*t));
					break;

				/* visitata e trans. temporale */
				case 3:
					energy = (1. - 5.*exp(t)) / (1. + exp(t));
					break;
			}
//			printf("%f\n", energy);
			return energy;
		}
	}

	/* solo in caso di placca non valida: stampa errore ed esce */
	fprintf(stderr,"\nLoop check: Trovata placca anomala! (%u, %u)\n", n, m);
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
	/* se i siti 0 e 1 sono uguali, continuazione forzata */
	if ( Reticolo::spin(np[0], mp[0]) == Reticolo::spin(np[1], mp[1]) )
		return (unsigned short) 1;
	/* altrimenti controllo i siti sulla diagonale */
	else {
		/* se i siti 1 e 2 sono uguali, transizione forzata */
		if ( Reticolo::spin(np[1], mp[1]) == Reticolo::spin(np[2], mp[2]) )
			return (unsigned short) 2;
		else /* se i siti 1 e 2 sono diversi, dec. opzionale */
			return (unsigned int) 3;
	}

	/* il valore di default e' "placca non riconosciuta" */
	return 0;
} /* -----  end of method Reticolo::type  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: plaque
 * Description: Calcolo le coordinate dei vertici della placca. 
 * ------------------------------------------------------------------
 */
void
Reticolo::plaque (signed int n, signed int m) {
	/* assegnamento iniziale */
	np[0] = (unsigned int) n;
	mp[0] = (unsigned int) m;

	/* se lo spin era positivo prima del flip */
	if ( Reticolo::spin(n, m) ) {
		/* 
		 * se sono in una posizione di "norma" dispari considedo
		 * interazione con placca inferiore
		 */
		if ( (m + n) % 2 == 1 )
			np[0] = (N + np[0] - 1) % N;
	/* se lo spin era negativo */
	} else {
		/* se sono in posizione di norma pari */
		if ( (m + n) % 2 == 0 ) {
			np[0] = (N + np[0] - 1) % N;
			mp[0] = (M + mp[0] - 1) % M;
		/* posizione di norma dispari */
		} else
			mp[0] = (M + mp[0] - 1) % M;
	}

	/* assegno le coordinate dei vertici della placca */
	for ( unsigned int i = 0; i < 4; i ++ ) {
		np[i] = (unsigned int) ( np[0] + (i % 2) ) % N;
		mp[i] = (unsigned int) ( mp[0] + (i / 2) ) % M;
	}
} /* -----  end of method Reticolo::plaque  ----- */


/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: forced_continuation
 * Description: calcola il prossimo punto del loop
 * ------------------------------------------------------------------
 */
bool
Reticolo::forced_continuation (void) {
	/* variabili ausiliarie */
	bool visited = true, s = Reticolo::spin(x[0], x[1]);

	unsigned short int i0 = 2 * !s;
	/* controllo se la placca e' stata gia' visitata */
	if ( sito[ np[i0] ][ mp[i0] ].lckd ^ sito[ np[i0 + 1] ][ mp[i0 + 1] ].lckd )
		visited = false;

	short int ic = 2 * s - 1;
	/* incremento la posizione lungo il tempo */
	x[1] = (unsigned) ( M + x[1] + (int) ic ) % M;

	return visited;
} /* -----  end of method Reticolo::forced_continuation  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: forced_transition
 * Description: c.s.
 * ------------------------------------------------------------------
 */
bool
Reticolo::forced_transition (void) {
	bool visited = true;
	/* controllo se la placca e' stata gia' visitata */
	short int ic = (short) 2 * Reticolo::spin(x[0], x[1]) - 1;
	if ( sito[ x[0] ][ x[1] ].lckd ^ sito[ x[0] ][ x[1] + ic ].lckd )
		visited = false;

	ic = (short) 2 * Reticolo::spin(np[0], mp[0]) - 1;
	/* incremento la posizione lungo lo spazio */
	x[0] = (unsigned) ( N + x[0] + (int) ic ) % N;

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
	bool visited = false;

	/* decido qual e' il vertice da controllare */
	unsigned short int i0 = 2 * Reticolo::spin(np[0], mp[0]) + 1;
	unsigned short int i = (i0 + ( Reticolo::spin(x[0], x[1]) == Reticolo::spin(np[1], mp[1]) )) % 4;

	/* variabili ausiliarie */
	bool t = Reticolo::spin(x[0], x[1]);
	unsigned short int a = 1;

	/* controllo se la placca e' stata visitata */
	if ( sito[ np[i] ][ mp[i] ].lckd == lckd ) {
		/* se placca visitata e transizione spaziale */
		if ( !(sito[ np[i] ][ mp[i] ].p) ) {
			t = Reticolo::spin(x[0], x[1]) == Reticolo::spin(np[0], mp[0]);
			a = 0;
		}
		visited = true;
	} else {
		/* se placca non visitata "estraggo" la transizione */
		if ( (long double) rand()/RAND_MAX > Reticolo::prob() ) {
			t = Reticolo::spin(x[0], x[1]) == Reticolo::spin(np[0], mp[0]);
			a = 0;
		}
	}

	/* incremento */
	signed short int ic = (signed) 2 * t - 1;
	/* salvo la transizione */
	sito[ x[0] ][ x[1] ].p = a;
	/* effettuo l'incremento */
	x[a] = (unsigned) ( p[a] + x[a] + (int) ic) % p[a];

	return (unsigned short int) 2 * visited + a;
} /* -----  end of method Reticolo::optional_decay  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: prob
 * Description: 
 * ------------------------------------------------------------------
 */
long double
Reticolo::prob (void) {
	return (long double) 2/( 1 + expl( (long double) 2*B*J/M ) );
} /* -----  end of method Reticolo::prob  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: r(ound)
 * Description: arrotonda k e ritorna le cifre significative
 * ------------------------------------------------------------------
 */
void
Reticolo::r (unsigned short int i) {
	/* NOTA: 'msr.sdom[i]' è positivo */
	if ( msr.sdom[i] < 0 ) {
		fprintf( stdout, "La sdom \'%hu\' è negativa!\n", i);
		exit(EXIT_FAILURE);
	} else if ( msr.sdom[i] >= 10 ) {
		fprintf( stdout, "La sdom \'%hu\' è troppo alta (> 10)!\n", i);
		exit(EXIT_FAILURE);
	}

	unsigned short int e;
	/* calcolo le cifre significative */
	for ( e = 0; ( pow(10, 1 - e) < msr.sdom[i] ) ^ ( msr.sdom[i] <= pow(10, -e) ); e ++ )
		;

	if ( msr.sdom[i] * pow(10, e) < 3 )
		e ++;

	/* arrotondo */
	float k = floorf(msr.sdom[i] * pow(10., e) + 0.5) / pow(10., e);
	float g = floorf(msr.mean[i] * pow(10., e) + 0.5) / pow(10., e);

	printf("%f\t%f\t", g, k);
} /* -----  end of method Reticolo::r  ----- */

/*
 * ------------------------------------------------------------------
 *       Class: Reticolo
 *      Method: mean
 * Description: normalizzo le medie e calcolo la sdom. 
 * ------------------------------------------------------------------
 */
void
Reticolo::mean (void) {
	/* normalizzo le medie */
	for ( unsigned int i = 0; i < 3; i++ ) {
		msr.mean[i] = (float) msr.mean[i]/msr.lenght;
		msr.sdom[i] = msr.sdom[i] / msr.lenght;
		msr.sdom[i] = sqrt( ( msr.sdom[i] - pow( msr.mean[i], 2) ) / msr.lenght );
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
Reticolo::print_results (void) {
	/* suscettività uniforme */
	msr.mean[0] = (float) B * msr.mean[0]/((float) N );
	msr.sdom[0] = (float) B * msr.sdom[0]/((float) N );

	/* suscettività alternata */
	msr.mean[1] = (float) B * msr.mean[1]/((float) 4 * N * M );
	msr.sdom[1] = (float) B * msr.sdom[1]/((float) 4 * N * M );

	msr.mean[2] = ((float) J / (4 * M * N)) * msr.mean[2];
	msr.sdom[2] = ((float) J / (4 * M * N)) * msr.sdom[2];

	/* stampo i risultati sotto forma di tabella */	
	printf( "# RISULTATI (N. sweep: %u)\n"
			"#J\tB\tL\tM\tUS\t\terr\t\tSS\t\terr\t\tEN\t\terr\n"
			"%u\t%u\t%u\t%u\t", msr.lenght, J, B, N, M );

	for ( unsigned int i = 0; i < 3; i++ )
		Reticolo::r(i);

	printf("\n");
} /* -----  end of method Reticolo::print_results  ----- */
