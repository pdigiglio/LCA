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
 *  			 calcola varianza e deviazione standard.
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
#include <string.h>

int
main ( int argc, char *argv[] ) {
	/* ordine della suddivisione */
	unsigned int ord = 1;
	if ( argc == 3 )
		ord = atoi( argv[argc - 1] );
	else
		fprintf( stdout, "Troppi argomenti!\n");

	/* valori delle misure */
	float **f = NULL;
	float temp;

	/* media ed errore */
	float mean[3] = { (float) 0, (float) 0, (float) 0};
	float err[3] =  { (float) 0, (float) 0, (float) 0};

	/* file contenente i dati */
	FILE *pFile = fopen(argv[1], "r");
	if ( pFile == NULL ) {
		fprintf(stderr, "Errore nell'apertura del file: %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	/* acquisisco i valori e calcolo la media */
	unsigned int l, i;
	for ( l = 0; !( feof(pFile) ); l ++ ) {	
		/* alloco la memoria per i valori in input */
		f = (float **) realloc( f, (l + 1) * sizeof(float *) );
		if ( f == NULL ) {
			fprintf ( stderr, "\ndynamic memory reallocation failed\n" );
			exit (EXIT_FAILURE);
		}

		f[l] = (float *) malloc( 3 * sizeof(float) );
		if ( f[l] == NULL ) {
			fprintf ( stderr, "\ndynamic memory allocation failed\n" );
			exit (EXIT_FAILURE);
		}
		
		/* azzero i valori di '**f' */
		for ( unsigned short int j = 0; j < 3; j ++ )
			f[l][j] = (float) 0;

		/* calcolo i cluster */
		for ( i = 0; i < ord && !( feof(pFile) ); i ++ ) {
			/* prima colonna */
			fscanf(pFile, "%f, ", &temp);
			f[l][0] += temp;

			/* seconda colonna */
			fscanf(pFile, "%f, ", &temp);
			f[l][1] += temp;

			/* terza colonna */
			fscanf(pFile, "%f\n", &temp);
			f[l][2] += temp;
		}
		
//		printf("%u: ", l);
		for ( unsigned short int j = 0; j < 3; j ++ ) {
			f[l][j] = (float) f[l][j] / i;
//			printf("%f\t", f[l][j]);
//			printf("%u\t%f\n", ord * l, f[l]);
			mean[j] += f[l][j];
			err[j] += pow( f[l][j], (float) 2);
		}
//		printf("\n");
	}
	/* chiudo il file */
	fclose(pFile);

	printf("%u\t", ord);
	for ( unsigned short int j = 0; j < 3; j ++ ) {
		/* normalizzo la media */
		mean[j] = (float) mean[j] / l;

		/* calcolo la varianza */
		err[j] = err[j] / l;
		err[j] = (float) sqrt( err[j] - pow( mean[j], (float) 2) );

		/* stampo a schermo varianza e sdom */
		printf( "%g\t%g\t", err[j], err[j] / sqrt(l) );
	}
	printf("\n");


	/* coefficienti di normalizzazione */
	float norm[3] = { (float) 1, (float) 1, (float) 1};

	/* apro un file di output (lo creo) */
	FILE *oFile = NULL;
	oFile = fopen( "ac.dat" , "w" );
	if ( pFile == NULL ) {
		fprintf(stderr, "Errore nell'apertura del file: %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	/* calcolo gli autocorrelatori */
	for ( unsigned short int t = 0; t < 30; t ++ ) {
			/* stampo la coordinata temporale */
			fprintf( oFile, "%hu\t", t);
			
			for ( unsigned short int j = 0; j < 3; j ++ ) {
			/* variabile temporanea */
			temp = (float) 0;

			/* calcolo l'i-esimo autocorrelatore */
			for ( unsigned int s = 0; s < (unsigned) l - t; s ++ )
				temp += f[s][j] * f[s + t][j];

			/* salvo il primo valore per normalizzare gli altri */
			if ( t == 0 )
				norm[j] = temp/(l - t) - pow(mean[j], 2);
			
			/* stampo i valori normalizzati */
			fprintf( oFile, "%g\t", (float) ( temp/(l - t) - pow(mean[j], 2) ) / norm[j] );
		}
		fprintf( oFile, "\n");
	}

	/* chiudo il file */
	fclose(oFile);
	exit(EXIT_SUCCESS);
}				/* ----------  end of function main  ---------- */
