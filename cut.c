/*
 * ==================================================================
 *
 *       Filename:  cut.c
 *
 *    Description:  Programmino per approssimare automaticamente i 
 *    				dati contenuti nei file prodotti da esecuzioni di
 *    				altri programmi. Il programma è pensato per essere
 *    				riscritto a seconda del tipo di tabella che bi-
 *    				arrotondare sogna
 *
 *        Version:  1.0
 *        Created:  19/09/2013 08:35:38
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
#include <errno.h>

/* 
 * ===  FUNCTION  ===================================================
 *         Name:  round
 *  Description: Arrotonda e stampa una coppia di numeri alla stessa
 *  			 cifra decimale: quelle significative dell'errore.
 * ==================================================================
 */
void
round ( float v, float err ) {
	/* dichiaro una variabile per l'esponente */
	short int exp = (short) log10( fabs( err ) );

	/* controllo che l'approssimazione sia corretta */
	while ( !( err / pow( 10, exp ) >= 1 && err / pow( 10, exp ) < 10 ) ) {
		if ( err / pow( 10, exp ) <= 1 ) exp --;
		else exp ++;
	}

	/* controllo le cifre decimali da tenere */
	if ( err / pow( 10, exp ) < 3 )
		exp --;

	printf( "%f\t%f\t",
			floorf( v / pow(10., exp) + 0.5) * pow(10., exp),
			floorf( err / pow(10., exp) + 0.5) * pow(10., exp)
		);
}		/* -----  end of function round  ----- */

int
main ( int argc, char *argv[] ) {
	/* nome del file input */
	char *in_file_name;
	
	/* controllo i parametri da riga di comando */
	if ( argc == 2 )
		in_file_name = argv[1];
	else { /* se sono sbagliati */
		if ( argc > 2 )
			fprintf( stderr, " > Troppi argomenti!\n");
		else if ( argc < 2 )
			fprintf( stderr, " > Manca il nome del file!\n");

		/* esco */
		exit( EXIT_FAILURE );
	}

	/* puntatore al file dati in input */
	FILE *in = fopen( in_file_name, "r" );
	if ( in == NULL ) {
		fprintf ( stderr, "Impossibile aprire il file '%s'; %s\n",
				in_file_name, strerror(errno) );
		exit (EXIT_FAILURE);
	}

	/*---------------------------------------------------------------
	 *  XXX Questa parte va adattata al formato del file input
	 *-------------------------------------------------------------*/

	/* prime colonne di interi */
	unsigned short int a;
	/* altre colonne: valore e errore */
	float val, err;
	/* finché il file non finisce */
	while ( !( feof( in ) ) ) {
		/* stampo le prime colonne di interi */
		for ( unsigned short int t = 0; t < 1; t ++ ) {
			fscanf( in, "%hu\t", &a );
			printf( "%hu\t", a );
		}

//		fscanf( in, "%f\t", &val );
//		printf( "%f\t", val);

		/* arrotondo le altre tre colonne */
		for ( unsigned short int t = 0; t < 1; t ++ ) {
			fscanf( in, "%f\t%f\n", &val, &err );
			round( (float) val / (a * a), (float) err / (a * a) );
		}

		/* le ultime due colonne */
//		fscanf( in, "%f\t%f\n", &val, &err );
//		round( val, err );

		/* vado a capo */
		printf("\n");
	}

	if( fclose(in) == EOF ) { /* close input file */
		fprintf ( stderr, "Impossibile chiudere il file '%s'; %s\n",
				in_file_name, strerror(errno) );
		exit (EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}				/* ----------  end of function main  ---------- */
