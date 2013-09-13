#include <cstdio>
#include <cstdlib>

/*
 * file header contenente i parametri del sistema quali:
 *  > numero di siti x N;
 *  > numero di siti y M;
 *  > 4 * numero di Trotter T;
 *  > inverso della temperatura B;
 *  > coefficiente si accoppiamento J;
 */
#include "./global.h"

#ifndef  reticolo_3d
#define  reticolo_3d

class Reticolo {
	public:
		Reticolo (void); /* ctor */
		virtual ~Reticolo (void); /* dtor */

		/* copre il reticolo di cluster e li inverte casualmente */
		void fill (void);

		/* restituisce lo spin nel sito (n,m) */
		bool get_spin (unsigned int n, unsigned int m, unsigned int t);
		/* restituisce il numero di loop */
		unsigned int get_lps (void);

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
		 * variabile ausiliaria utile per capire se un sito è bloccato
		 * o no. Si usa così: ad ogni sweep vale 'true' o 'false' a
		 * seconda che lo sweep sia rispettivamente pari o dispari.
		 * È utile per evitare di resettare i campi 'sito[][][].lckd'.
		 */
		bool lckd = 0;

		/* dichiaro il reticolo di spin */
		struct Sito {
			/* valore dello spin nel sito (n,m,t) */
			bool s;
			/* indica la transizione per placche di tipo OD */
			bool p;
			/* indica se il sito appartiene gia' ad un cluster */
			bool lckd = false;
			/* indica se il sito e' stato invertito o no */
			bool flip = false;
		} ***sito = NULL;

		/* posizione del loop */
		unsigned int x[3];
		/* vertice della placca su cui giace la posizione del loop */
		unsigned short int i_x;
		/* variabili ausiliarie per i vertici delle placche */
		unsigned int xp[4][3] = {
			{ 0, 0, 0 },
			{ 0, 0, 0 },
			{ 0, 0, 0 }
		};
		
		/* numero di loop per ogni sweep */
		unsigned short int lps = 0;
		
		/* contiene le grandezze misurate sul sistema */
		struct Measure {
			/* Indici:
			 * 0. suscettivita' uniforme;
			 * 1. suscettivita' "staggered";
			 * 3. densita' energia.
			 */
			float val[3];
			/* media */
			float mean[3] = { (float) 0, (float) 0, (float) 0 };
			/* deviazione standard della media */
			float sdom[3] = { (float) 0, (float) 0, (float) 0 };
			/* numero di misure effettuate ('s' = sweep) */
			unsigned int lenght = 0;
		} msr;
	
		/* restituisce lo spin del sito (n,m) prima del flip */
		bool spin (unsigned int n, unsigned int m, unsigned int t);
		/* inizializza un loop dal sito (n,m) */
		void loop (unsigned int n, unsigned int m, unsigned int t);
		/* inverte gli spin in un loop */
		void flip (unsigned int n, unsigned int m, unsigned int t, bool f);

		/* controlla la direzione del loop su un sito */
		float next_ene (unsigned int n, unsigned int m, unsigned int t);

		/* controlla qual e' il tipo della placchetta in (n,m) */
		unsigned short int type (void);
		/* assegna i vertici della placca interagente col sito (n,m,t) */
		void plaque (unsigned int n, unsigned int m, unsigned int t);

		/* restituisce lo spin del vertice 'i' */
		bool p_s ( signed short int i );
		/* assegnano gli opportuni incrementi al loop */
		bool forced_continuation (void);
		bool forced_transition (void);
		unsigned short int optional_decay (void);

		/* probabilita' di transizione nel decadimento opzionale */
		long double prob (void);

		/* stampa i valori arrotondati di 'msr.{sdom[i],mean[i]}' */
		void r (unsigned short int i);
};

#endif	/* ----- #ifndef reticolo_3d ----- */
