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

#include "./reticolo-1d.cc"
#include <ctime>
#include <errno.h>
#include <string.h>

int
main ( void ) {
	/* inizializzo in modo casuale il seme per 'rand()' */
	srand(time(NULL));

	/* dichiaro un reticolo */
	Reticolo reticolo;

	FILE *oFile = NULL;
	/* apro un file per la scrittura */
	oFile = fopen ( "data.dat", "w");
	if ( oFile == NULL ) {
		fprintf(stderr, "Errore nell'apertura del file: %s\n", "data.dat");
		exit(EXIT_FAILURE);
	}

	/* n. loop, 1 / (n. loop), decadimenti temporali */
	double msr[3][2] = { {0., 0.}, {0., 0.}, {0., 0.}};
	/* faccio evolvere il sistema e effettuo le misure */
	for ( unsigned int i = 0; i < MSR; i++ ) {
		reticolo.fill();

		/* esporto i valori delle misure in un file di dati */
		fprintf( oFile, "%f, ", (float) B * reticolo.get_msr( 0 ) / ( (float) N ) );
		fprintf( oFile, "%f, ", (float) B * reticolo.get_msr( 1 ) / ( (float) 4 * N * M ) );
		fprintf( oFile, "%f\n", ( (float) J / ( 4 * M * N ) ) * reticolo.get_msr( 2 ) );
		
		/* faccio la media delle lunghezze dei loop */
		msr[0][0] += (double) reticolo.get_lps();
		msr[1][0] += (double) N * M / reticolo.get_lps();
		msr[2][0] += (double) reticolo.get_time_decay() / ( N * M );

		/* errori */
		msr[0][1] += pow( (double) reticolo.get_lps(), 2 );
		msr[1][1] += pow( (double) N * M / reticolo.get_lps(), 2 );
		msr[2][1] += pow( (double) reticolo.get_time_decay() / ( N * M), 2 );
//		reticolo.measure();
	}

	/* chiudo il file */
	fclose(oFile);


	/* file output */ 
	char outFile_file_name[] = "./lunghezze.dat";
	FILE *outFile = fopen( outFile_file_name, "a" );

	if ( outFile == NULL ) {
		fprintf ( stderr, "couldn't open file '%s'; %s\n",
				outFile_file_name, strerror(errno) );
		exit (EXIT_FAILURE);
	}

	fprintf( outFile, "#J\tB\tN\tM\t2JB/M\t\tlps\t\td(lps)\t\tlen\t\td(len)\t\tt-dec\t\td(t-dec)\n" );
	fprintf( outFile, "%u\t%u\t%u\t%u\t%g\t", J, B, N, M, (double) 2*B*J/M);
	for ( unsigned short j = 0; j < 3; j ++ ){
		msr[j][0] = msr[j][0] / MSR;
		msr[j][1] = sqrt( ( msr[j][1] / MSR - pow( msr[j][0], 2 ) ) / MSR );
		fprintf( outFile, "%g\t%g\t", msr[j][0], msr[j][1]);
	}
	fprintf( outFile, "\n");
	
	if( fclose(outFile) == EOF ) { /* close output file */
		fprintf ( stderr, "couldn't close file '%s'; %s\n",
				outFile_file_name, strerror(errno) );
		exit (EXIT_FAILURE);
	}


	reticolo.mean();
	reticolo.print_results();

	exit(EXIT_SUCCESS);
} /* ----------  end of function main  ---------- */
