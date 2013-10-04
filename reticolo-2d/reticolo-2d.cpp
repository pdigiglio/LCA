/*
 * ==================================================================
 *
 *       Filename:  reticolo_3d.cpp
 *
 *    Description:  Provo a studiare una catena monodimensionale
 *    				antiferromagnetica
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

#include "./reticolo-2d.cc"

#include <ctime>
#include <cerrno>
#include <cstring>

int
main ( void ) {
	/* inizializzo in modo casuale il seme per 'rand()' */
	srand(time(NULL));

	/* dichiaro un reticolo */
	Reticolo reticolo;

	/* nome file output per storico misure */
	char oFile_file_name[] = "./data.dat"; 
	/* file di output per lo storico delle misure */
	FILE *oFile = fopen( oFile_file_name, "w" );
	
	/* controllo che il file sia aperto correttamente */
	if ( oFile == NULL ) {
		fprintf ( stderr, " > Impossibile aprire il file '%s': %s\n",
				oFile_file_name, strerror(errno) );
		exit (EXIT_FAILURE);
	}

	/* variabile ausiliaria per l'interfaccia utente "percentage" */
	unsigned int pct = 0, k = 0;
	/* n. loop, 1 / (n. loop), decadimenti temporali */
	double msr[3][2] = {};
	/* genero le misure e le scrivo nel file */
	for ( unsigned int i = 0; i < MSR; i ++ ) {
		/* stampo la percentuale di completamento a schermo */
		if ( i == pct ) {
			/* stampo la percentuale */
			fprintf( stderr, "Sweep n. %u of %u. Completed %u %%\r", i, MSR, k );

			/* aggiorno le variabili di controllo */
			pct = ++ k * ( MSR / 100 );
		}

		/* genero una nuova configurazione del reticolo */
		reticolo.fill();

		/* stampo le misure correnti in un file di testo ausiliario */
		fprintf( oFile, "%f, ", ((double) B / (N * M)) * reticolo.get_msr(0));
		fprintf( oFile, "%f, ", ((double) B / (4 * N * M * T * T)) * reticolo.get_msr(1));
		fprintf( oFile, "%f\n", ((double) J / (N * M * T)) * reticolo.get_msr(2));

		/* faccio la media delle lunghezze dei loop */
		msr[0][0] += (double) reticolo.get_lps();
		msr[1][0] += (double) N * M * T / reticolo.get_lps();
		msr[2][0] += (double) reticolo.get_time_decay() / ( N * M * T );

		/* errori */
		msr[0][1] += pow( (double) reticolo.get_lps(), 2 );
		msr[1][1] += pow( (double) N * M * T / reticolo.get_lps(), 2 );
		msr[2][1] += pow( (double) reticolo.get_time_decay() / ( N * M * T ), 2 );
	}

	/* copro la percentuale di completamento */
	fprintf(stderr, "                                            \r");

	/* chiudo il file output per le misure */
	if( fclose(oFile) == EOF ) {
		fprintf ( stderr, " > Impossibile chiudere il file '%s': %s\n",
				oFile_file_name, strerror(errno) );
		exit (EXIT_FAILURE);
	}

	/* file output per proprietà reticolo */ 
	char outFile_file_name[] = "./lunghezze.dat";
	FILE *outFile = fopen( outFile_file_name, "a" );

	if ( outFile == NULL ) {
		fprintf ( stderr, "couldn't open file '%s'; %s\n",
				outFile_file_name, strerror(errno) );
		exit (EXIT_FAILURE);
	}

	fprintf( outFile, "#SWEEP %u\n", MSR);
	fprintf( outFile, "#J\tB\tN\tM\t4JB/T\t\tlps\t\td(lps)\t\tlen\t\td(len)\t\tt-dec\t\td(t-dec)\n" );
	fprintf( outFile, "%u\t%u\t%u\t%u\t%g\t", J, B, N, M, (double) 4*B*J/T );
	for ( unsigned short j = 0; j < 3; j ++ ){
		msr[j][0] = msr[j][0] / MSR;
		msr[j][1] = sqrt( ( msr[j][1] / MSR - pow( msr[j][0], 2 ) ) / MSR );
		fprintf( outFile, "%f\t%f\t", msr[j][0], msr[j][1]);
	}
	fprintf( outFile, "\n");
	
	/* chiudo file esterno per proprietà reticolo */
	if( fclose(outFile) == EOF ) {
		fprintf ( stderr, "couldn't close file '%s'; %s\n",
				outFile_file_name, strerror(errno) );
		exit (EXIT_FAILURE);
	}
	
	/* calcolo la media */
	reticolo.mean();
	/* la stampo a schermo */
	reticolo.print_results();

	exit(EXIT_SUCCESS);
} /* ----------  end of function main  ---------- */
