#include <cstdio>
#include <cstdlib>

/*
 * file header contenente i parametri del sistema quali:
 *  > numero di siti x N;
 *  > numero di siti y M;
 *  > 4 * numero di Trotter T;
 *  > inverso della temperatura B;
 *  > coefficiente di accoppiamento J;
 */
#include "./global.h"

#ifndef  reticolo_3d
#define  reticolo_3d

class Reticolo {
	public:
		Reticolo (void); /* ctor */
		virtual ~Reticolo (void); /* dtor */

		/* copre il reticolo di cluster disgiunti */
		void fill (void);

		/* restituisce lo spin nel sito (n, m, t) */
		bool get_spin (unsigned int n, unsigned int m, unsigned int t);
		/* restituisce il numero di loop nello sweep corrente */
		unsigned int get_lps (void);
		/* restituisce il numero di decadimenti temporali */
		unsigned int get_time_decay (void);
		/* restituisce il valore attuale della misura 'i' */
		double get_msr (unsigned short int i);

		/* calcola medie ed incertezze (non normalizzate) */
		void mean (void);
		/* stampa a schermo medie ed incertezze normalizzate */
		void print_results (void);
	private:
		/*
		 * parametri del reticolo: sono gli stessi che compaiono
		 * nelle istruzioni '#define' ma li metto in un vettore
		 * per poterli gestire con facilita' piu' avanti
		 */
		const unsigned short int p[3] = { N, M, T };

		/* 
		 * Variabile ausiliaria utile per capire se un sito è bloccato
		 * o no. Si usa così: ad ogni sweep vale 'true' o 'false' a
		 * seconda che lo sweep sia rispettivamente pari o dispari.
		 * È utile per evitare di resettare i campi 'sito[][][].lckd'.
		 */
		bool lckd = false;
		/* 
		 * Altra variabile globale che mi dice se invertire o meno gli
		 * sweep situati all'interno di un cluster. Va estratta a caso
		 * ogni qualvolta si inizializza un loop.
		 */
		bool flp = false;

		/* definisco il record 'st' e il tipo 'Sito' */
		typedef struct st {
			/* valore spin, tipo transizione ('p = 1' trans. temp.) */
			bool s, p;
			/* indica se il sito appartiene gia' ad un cluster */
			bool lckd = false;
			/* 
			 * indica se la transizione che subisce il loop quando
			 * raggiunge questo sito è stata già determinata o no
			 */
			bool ckd = false;
		} Sito;
		/* dichiaro il reticolo di spin (da espandere) */
		Sito ***sito = NULL;

		/* segnaposto per il loop (inizializzato a zero) */
		unsigned int x[3];
		/* vertice della placca su cui giace la posizione del loop */
		unsigned short int i_x;
		/* variabili ausiliarie per i vertici delle placche */
		unsigned int xp[4][3];
		/* variabile che indica se la placca corrente è verticale */
		bool v;
		
		/* numero di loop: da azzerare ad ogni sweep */
		unsigned short int lps = 0;
		/* numero di "time decays", da azzerare ad ogni sweep */
		unsigned int tds = 0;
		
		/* contiene le grandezze misurate sul sistema */
		struct Measure {
			/* Indici:
			 * 0/1. suscettività uniforme/"staggered";
			 * 3. 	densità d'energia.
			 */
			double val[3];
			/* media, deviazione standard della media */
			long double mean[3] = {}, sdom[3] = {};
			/* numero di misure effettuate */
			unsigned int lenght = 0;
		} msr;
	
		/* restituisce lo spin del sito (n, m, t) prima del flip */
		bool spin (unsigned int n, unsigned int m, unsigned int t);
		/* inizializza un loop dal sito (n, m, t) */
		void loop (unsigned int n, unsigned int m, unsigned int t);
		/* (se 'flp == true') inverte gli spin in un loop */
		void flip (unsigned int n, unsigned int m, unsigned int t);

		/*
		 * assegna il prossimo punto del loop e restituisce l'energia
		 * della placca interagente col sito che rappresenta l'ultimo
		 * punto (quello più recente) del loop
		 */
		double next_ene (unsigned int n, unsigned int m, unsigned int t);

		/* 
		 * una volta determinati i vertici della placca, ne controlla
		 * il tipo e restituisce un numero tra 0 (errore) e 4 per i-
		 * dentificarlo (cfr. commento nel file .cc)
		 */
		unsigned short int type (void);
		/* assegna i vertici della placca interagente (n, m, t) */
		void plaque (unsigned int n, unsigned int m, unsigned int t);

		/*
		 * restituisce un puntatore al sito corrispondente al vertica
		 * i-esimo della placca. In questo modo si accede semplicemente
		 * a tutti i campi del record. Il nome della funzione sta per
		 * "plaque vertex (number i)"
		 */
		Sito * pv ( signed short int i );
		/* 
		 * restituisce lo spin del vertice 'i' della placca prima 
		 * dell'eventuale flip (usa la funzione 'Reticolo::spin()').
		 * Il nome sta per "plaque spin (number i)"
		 */
		bool ps ( signed short int i );

		/* 
		 * Modifica il segnaposto del loop per una transizione di tipo
		 * tempo: il nome sta per "temporal transition".
		 */
		void tt (void);
		/* 
		 * Modifica il segnaposto del loop per una transizione di tipo
		 * spazio: il nome sta per "spazial transition".
		 */
		void st (void);

		/* 
		 * Determinano le transizioni che avvengono nella placca e as-
		 * segnano il valore corretto ai campi '.p' del record 'Sito'.
		 * Assegnano l'incremento del loop tramite le funzioni qui 
		 * sopra 'Reticolo::{s,t}t()'
		 */
		void forced_continuation (void);
		void forced_transition (void);
		void optional_decay (void);

		/* 
		 * Restituisce la probabilità di transizione nelle placche di
		 * tipo "decadimento opzionale".
		 */
		long double prob (void);

		/* Stampa i valori arrotondati di 'msr.{sdom[i],mean[i]}'. */
		void r (unsigned short int i);
};

#endif	/* ----- #ifndef reticolo_3d ----- */
