/*
 * ==================================================================
 *
 *       Filename:  analisi.cpp
 *
 *  Description:  [sintassi] Il programma dev'essere lanciato con la
 *  			 sintassi "./analisi {file(.dat)} {ord}". Il {file}
 *  			 deve contenere tre colonne di cui le prime due sepa-
 *  			 rate da una virgola.
 *
 *  			  [output] Il programma suddivide i dati del file in
 *  			 cluster di dimensione {ord} e ne fa la media. Poi
 *  			 calcola varianza e deviazione standard stampandole a
 *  			 schermo. Nel frattempo crea un file contenente gli
 *  			 autocorrelatori.
 *
 *        Version:  1.0
 *        Created:  07/09/2013 11:02:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  P. Di Giglio (), p.digiglio91@gmail.com
 *   Organization:  
 *
 * ==================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>

int
main ( int argc, char *argv[] ) {
	/* ordine della suddivisione */
	unsigned short int ord = 1;
	
	/* controllo che argomenti da linea di comando */ 
	if ( argc == 3 )
		ord = atoi( argv[argc - 1] );
	else if ( argc == 2 )
		fprintf( stderr, " > Variabile 'ord' impostata di default a %hu\n", ord );
	else { /* se sono sbagliati */
		if ( argc > 3 )
			fprintf( stderr, " > Troppi argomenti!\n");
		else if ( argc < 2 )
			fprintf( stderr, " > Manca il nome del file!\n");

		/* esco dal programma */
		exit(EXIT_FAILURE);
	}

	/* (apro il) file contenente i dati */
	FILE *pFile = fopen( argv[1], "r" );
	if ( pFile == NULL ) {
		fprintf( stderr, " > Impossibile aprire il file '%s': %s\n", argv[1], strerror(errno) );
		exit(EXIT_FAILURE);
	}

	/* ---------------------------------------------------------------
	 *  CALCOLO MEDIE ED ERRORI
	 * -------------------------------------------------------------*/

	/* valori delle misure */
	double **f = NULL;
	double temp;

	/* media ed errore */
	double mean[3] = { (double) 0, (double) 0, (double) 0 };
	double err[3] =  { (double) 0, (double) 0, (double) 0 };

	/* acquisisco i valori e calcolo la media */
	unsigned int l, i;
	for ( l = 0; !( feof(pFile) ); l ++ ) {	
		/* alloco la memoria per i valori in input */
		f = (double **) realloc( f, (l + 1) * sizeof(double *) );
		if ( f == NULL ) {
			fprintf ( stderr, "\ndynamic memory reallocation failed\n" );
			exit (EXIT_FAILURE);
		}

		f[l] = (double *) malloc( 3 * sizeof(double) );
		if ( f[l] == NULL ) {
			fprintf ( stderr, "\ndynamic memory allocation failed\n" );
			exit (EXIT_FAILURE);
		}
		
		/* azzero i valori di '**f' */
		for ( unsigned short int j = 0; j < 3; j ++ )
			f[l][j] = (double) 0;

		/* calcolo i cluster */
		for ( i = 0; i < ord && !( feof(pFile) ); i ++ ) {
			/* prima colonna */
			fscanf(pFile, "%lf, ", &temp);
			f[l][0] += temp;

			/* seconda colonna */
			fscanf(pFile, "%lf, ", &temp);
			f[l][1] += temp;

			/* terza colonna */
			fscanf(pFile, "%lf\n", &temp);
			f[l][2] += temp;
		}
		
		/* normalizzo i cluster ed aggiorno le medie */
		for ( unsigned short int j = 0; j < 3; j ++ ) {
			f[l][j] = (double) f[l][j] / i;
//			printf("%lf\t", f[l][j]);
//			printf("%u\t%lf\n", ord * l, f[l]);
			mean[j] += f[l][j];
			err[j] += pow( f[l][j], (double) 2);
		}
//		printf("\n");
	}

	/* chiudo il file di input */
	if( fclose( pFile ) == EOF ) {
		fprintf ( stderr, " > Impossibile chiudere il file '%s'; %s\n",
				argv[1], strerror(errno) );
		exit (EXIT_FAILURE);
	}

	/* stampo ordine, varianza e SDM correttamente normalizzate */
	printf("%u\t", ord);
	for ( unsigned short int j = 0; j < 3; j ++ ) {
		/* normalizzo la media */
		mean[j] = (double) mean[j] / l;

		/* calcolo la varianza */
		err[j] = err[j] / l;
		err[j] = (double) sqrt( err[j] - pow( mean[j], (double) 2) );

		/* stampo a schermo varianza e sdom */
		printf( "%g\t%g\t", err[j], err[j] / sqrt(l) );
	}
	printf("\n");

	/*-----------------------------------------------------------------------------
	 *  AUTOCORRELATORI
	 *-----------------------------------------------------------------------------*/
	
	/* coefficienti di normalizzazione */
	double norm[3] = { (double) 1, (double) 1, (double) 1 };

	/* apro un file di output (lo creo) */
	FILE *oFile = fopen( "ac.dat" , "w" );
	if ( oFile == NULL ) {
		fprintf ( stderr, " > Impossibile aprire il file '%s'; %s\n",
				"ac.dat", strerror(errno) );
		exit (EXIT_FAILURE);
	}

	/* calcolo gli autocorrelatori */
	for ( unsigned short int t = 0; t < 30; t ++ ) {
			/* stampo la coordinata temporale */
			fprintf( oFile, "%hu\t", t );
			
			for ( unsigned short int j = 0; j < 3; j ++ ) {
			/* variabile temporanea */
			temp = (double) 0;
			/* riutilizzo la variabile per l'errore */
			err[j] = (double) 0;

			/* aggiorno il j-esimo autocorrelatore e il suo errore */
			for ( unsigned int s = 0; s < (unsigned) l - t; s ++ ) {
				temp += f[s][j] * f[s + t][j];
				err[j] += pow( f[s][j] * f[s + t][j], 2 );
			}

			/* normalizzo auto-correlatore e errore */
			temp = (double) temp / ( l - t );
			/* uso la correzione di Bessel */
			err[j] = sqrt( ( err[j] / (l - t) - pow (temp, 2) ) / (l - t - 1) );

			/* salvo il primo valore per normalizzare gli altri */
			if ( t == 0 )
				norm[j] = temp - pow(mean[j], 2);
			
			/* stampo i valori normalizzati */
			fprintf( oFile, "%g\t%g\t", (double) ( temp - pow(mean[j], 2) ) / norm[j], err[j] / norm[j] );
		}
		fprintf( oFile, "\n");
	}

	/* chiudo il file di output */
	if( fclose(oFile) == EOF ) {
		fprintf ( stderr, " > Impossibile chiudere '%s'; %s\n",
				"ac.dat", strerror(errno) );
		exit (EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}				/* ----------  end of function main  ---------- */
