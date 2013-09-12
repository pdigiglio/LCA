/*
 * ==================================================================
 *
 *       Filename:  reticolo_2d.cpp
 *
 *    Description:  [usage] Il programma si richiama nella sintassi 
 *    				"reticolo_2d {J} {B} {N} {M}" e in questo modo
 *    				verrà inizializzato un reticolo con i parametri 
 *    				indicati dalla linea di comando.
 *
 *        Version:  1.0
 *        Created:  03/08/2013 17:44:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  P. Di Giglio (), p.digiglio91@gmail.com
 *   Organization:  
 *
 * ==================================================================
 */

#include "./reticolo_2d.cc"
#include <ctime>

int
main ( void ) {
	/* inizializzo in modo casuale il seme per 'rand()' */
	srand(time(NULL));

	/* dichiaro un reticolo */
	Reticolo reticolo;

//	reticolo.print();

	FILE *oFile = NULL;
	/* apro un file per la scrittura */
	oFile = fopen ( "data.dat", "w");
	if ( oFile == NULL ) {
		fprintf(stderr, "Errore nell'apertura del file: %s\n", "data.dat");
		exit(EXIT_FAILURE);
	}

	/* faccio evolvere il sistema e effettuo le misure */
	for ( unsigned int i = 0; i < 100000; i++ ) {
		reticolo.fill();
		fprintf( oFile, "%f, ", (float) B * reticolo.get_msr( 0 ) / ( (float) N ) );
		fprintf( oFile, "%f, ", (float) B * reticolo.get_msr( 1 ) / ( (float) 4 * N * M ) );
		fprintf( oFile, "%f\n", ( (float) J / ( 4 * M * N ) ) * reticolo.get_msr( 2 ) );
//		reticolo.measure();
	}

	/* chiudo il file */
	fclose(oFile);

//	reticolo.print();
	reticolo.mean();
	reticolo.print_results();

	exit(EXIT_SUCCESS);
} /* ----------  end of function main  ---------- */
