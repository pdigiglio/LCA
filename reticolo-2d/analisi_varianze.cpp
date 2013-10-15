/*
 * ==================================================================
 *
 *       Filename:  analisi_varianze.cpp
 *
 *    Description:  Programma per confrontare l'errore sull'estimatore
 *    				classico e migliorato in funzione del numero medio
 *    				di loop per reticolo. Si aspetta un file d'ingresso
 *    				della forma "%u\t%f\t%f\t%f\t%f\n", dove i campi
 *    				sono: 'lps', sus. un (classica), sus. stagg. (clas-
 *    				sica), sus. un. (improved), sus. stagg. (imp.).
 *
 *
 *        Version:  1.0
 *        Created:  08/10/2013 10:59:19
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
#include <string.h>
#include <errno.h>
#include <math.h>

int
main ( int argc, char *argv[] ) {
	/* nome file in input */
	char *in_file_name; // = "./tmp.dat";
	if ( argc == 2 )
		in_file_name = argv[1];

	/* apro il file */
	FILE *in = fopen( in_file_name, "r");
	if ( in == NULL ) {
		fprintf ( stderr, "couldn't open file '%s'; %s\n",
				in_file_name, strerror(errno) );
		exit (EXIT_FAILURE);
	}

	/* numero di bin */
	unsigned int l;
	/* variabile per contenere il numero di loop */
	unsigned int a;
	/* lunghezza dei bin in cui suddivido le misure */
	unsigned int len = 10;

	/* variabili */
	float var[4], m[4] = {}, ml = 0, v[4] = {}, vl = 0;
	float temp[4] = {};
	float vv[4] = {};

	/* faccio andare a vuoto le prime 50 righe */
	for ( unsigned int i = 0; i < 50 ; i ++ )
			fscanf( in, "%u\t%f\t%f\t%f\t%f\n", &a, &v[0], &v[1], &v[2], &v[3] );

	/* numero totale di misure */
	unsigned int number = 0;
	/* leggo i dati dal file */
	for ( l = 0; !( feof(in) ); l ++ ) {
		/* azzero le variabili temporanee per media e varianze */
		for ( unsigned int j = 0; j < 4; j ++ ) {
			m[j] = (float) 0;
			var[j] = (float) 0;
		}

		unsigned int i;
		for ( i = 0; i < len && !( feof(in) ); i ++ ) {
			/* aggiorno il numero di misure */
			number ++;

			/* acquisisco i dati */
			fscanf( in, "%u\t%f\t%f\t%f\t%f\n", &a, &v[0], &v[1], &v[2], &v[3] );

			/* calcolo le medie della lunghezza */
			ml += (float) a;
			vl += (float) a * a;

			/* calcolo le altre medie */
			for ( unsigned int j = 0; j < 4; j ++ ) {
				m[j] += v[j];
				var[j] += v[j] * v[j];
			}
		}

		for ( unsigned int j = 0; j < 4; j ++ ) {
			/* normalizzo le medie */
			m[j] = (float) m[j] / i;
			/* calcolo le varianze */
			var[j] = sqrt( var[j] / i - m[j] * m[j] );
			/* calcolo le medie delle varianze */
			temp[j] += var[j];
			vv[j] += var[j] * var[j];
		}
	}

	fprintf( stderr, "l: %u\n", l) ;
	/* normalizzo il numero di loop */
	ml = (float) ml / number;
	vl = sqrt( ( (float) vl / number - ml * ml ) / ( number - 1) );

	/* normalizzo le medie delle varianze */
	for ( unsigned j = 0; j < 4; j ++ ) {
		temp[j] = (float) temp[j] / l;
		vv[j] = sqrt( ( vv[j] / l - temp[j] * temp[j] ) / ( l - 1 ) );
	}

	/* stampo a schermo i risultati */
	printf( "%g\t%g\t%g\t%g\t%g\t%g\n", ml, vl, temp[2] / temp[0],
			( temp[2] / temp[0] ) * ( vv[0] / temp[0] + vv[2] / temp[2] ),
			temp[3] / temp[1], ( temp[3] / temp[1] ) * ( vv[1] / temp[1] + vv[3] / temp[3] )
	);

	if( fclose(in) == EOF ) { /* close input file */
		fprintf ( stderr, "couldn't close file '%s'; %s\n",
				in_file_name, strerror(errno) );
		exit (EXIT_FAILURE);
	}

		exit(EXIT_SUCCESS);
}				/* ----------  end of function main  ---------- */
