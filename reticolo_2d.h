#include <cstdio>
#include <cstdlib>

/*
 * file header contenente i parametri del sistema quali:
 *  > numero di siti N;
 *  > 2 * numero di trotter M;
 *  > inverso della temperatura B;
 *  > coefficiente si accoppiamento J;
 */
#include "./global.h"

#ifndef  reticolo_2d
#define  reticolo_2d

class Reticolo {
	public:
		Reticolo (void); /* ctor */
		virtual ~Reticolo (void); /* dtor */

		/* copre il reticolo di cluster e li inverte casualmente */
		void fill (void);

		/* effettua le misure per la configurazione corrente */
//		void measure (void);

		/* "suggestiva" rappresentazione a schermo del reticolo */
		void print (void);

		/* restituisce lo spin nel sito (n, m) */
		bool get_spin (unsigned int n, unsigned int m);
		/* restituisce il numero di loop */
		unsigned int get_lps (void);
		/* restituisce il numero di decadimenti temporali */
		unsigned int get_time_decay (void);
		/* restituisce la misura 'i' (non normalizzata) */
		float get_msr (unsigned short int i);

		/* calcola medie ed incertezze (non normalizzate) */
		void mean (void);
		/* stampa a schermo medie ed incertezze normalizzate */
		void print_results (void);
private:
		/*
		 * parametri del reticolo: li metto in un vettore per poterli 
		 * gestire con facilita' piu' avanti.
		 */
		const unsigned short int p[2] = {N, M};

		/* 
		 * variabile ausiliaria utile per capire se un sito è bloccato
		 * o no. Si usa così: ad ogni sweep vale 'true' o 'false' a
		 * seconda che lo sweep sia rispettivamente pari o dispari.
		 * È utile per evitare di resettare i campi 'sito[][][].lckd'.
		 */
		bool lckd = 0;
	
		/* dichiaro il reticolo di spin */
		struct Sito {
			/* valore dello spin nel sito (n,m) */
			bool s;
			/* indica la transizione per placche di tipo OD */
			bool p;
			/* indica se il sito appartiene gia' ad un cluster */
			bool lckd = false;
			/* indica se il sito e' stato invertito o no */
			bool flip = false;
		} **sito = NULL;

		/* posizione del loop */
		unsigned short int x[2];
		/* variabili ausiliarie per i vertici delle placche */
		unsigned int np[4] = {0, 0, 0, 0};
		unsigned int mp[4] = {0, 0, 0, 0};
		
		/* numero di loop per ogni sweep */
		unsigned short int lps = 0;
		/* numero di decadimenti temporali ad ogni sweep */
		unsigned int td = 0;

		/* contiene le grandezze misurate sul sistema */
		struct Measure {
			/* Indici:
			 * 0. suscettivita' uniforme;
			 * 1. suscettivita' "staggered";
			 * 3. densita' energia.
			 */
			float val[3];
			/* media */
			double mean[3] = { (double) 0, (double) 0, (double) 0 };
			/* deviazione standard della media */
			double sdom[3] = { (double) 0, (double) 0, (double) 0 };
			/* numero di misure effettuate ('s' = sweep) */
			unsigned int lenght = 0;
		} msr;
	
		/* restituisce lo spin del sito (n, m) prima del flip */
		bool spin (unsigned int n, unsigned int m);
		/* inizializza un loop dal sito (n, m) */
		void loop (unsigned int n, unsigned int m);
		/* inverte gli spin in un loop */
		void flip (unsigned int n, unsigned int m, bool f);

		/* controlla la direzione del loop su un sito */
		float next_ene (unsigned int n, unsigned int m);

		/* controlla qual e' il tipo della placchetta in (n,m) */
		unsigned short int type (void);
		/*
		 * assegna alle variabili 'np[]' e 'mp[]' i vertici della
		 * placca che interagisce col sito (n,m)
		 */
		void plaque (signed int n, signed int m);

		/* assegnano gli opportuni incrementi al loop */
		bool forced_continuation (void);
		bool forced_transition (void);
		unsigned short int optional_decay (void);

		/* probabilita' di transizione nel decadimento opzionale */
		long double prob (void);

		/* stampa i valori arrotondati di msr.{var,sdom} */
		void r (unsigned short int i);
};

#endif	/* ----- #ifndef reticolo_2d ----- */
