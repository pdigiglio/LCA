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
	
	/* variabile ausiliaria per l'interfaccia utente */
	unsigned int percentage = MSR / 100;

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

	/* genero le misure e le scrivo nel file */
	for ( unsigned int i = 0; i < MSR; i ++ ) {
		if ( ! ( i % percentage ) ) {
			fprintf( stderr, "Sweep n. %u: %u %% complete\r", i, i/percentage );
			fflush( stderr );
		}

		reticolo.fill();

		fprintf( oFile, "%f, ", ((double) B / (N * M)) * reticolo.get_msr(0));
		fprintf( oFile, "%f, ", ((double) B / (4 * N * M * T)) * reticolo.get_msr(1));
		fprintf( oFile, "%f\n", ((double) J / (N * M * T)) * reticolo.get_msr(2));
	}

	/* chiudo il file output */
	if( fclose(oFile) == EOF ) {
		fprintf ( stderr, " > Impossibile chiudere il file '%s': %s\n",
				oFile_file_name, strerror(errno) );
		exit (EXIT_FAILURE);
	}

	reticolo.mean();
	reticolo.print_results();

	exit(EXIT_SUCCESS);
} /* ----------  end of function main  ---------- */
