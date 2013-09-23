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

#include "./reticolo_3d.cc"
#include <ctime>

int
main ( void ) {
	/* inizializzo in modo casuale il seme per 'rand()' */
	srand(time(NULL));

	/* dichiaro un reticolo */
	Reticolo reticolo;

	for ( unsigned int i = 0; i < 20000; i ++ ) {
//		printf("Sweep: %u ################################\n", i);
		reticolo.fill();

		fprintf( stdout, "%f, ", ((double) B / (N * M)) * reticolo.get_msr(0));
		fprintf( stdout, "%f, ", ((double) B / (4 * N * M * T)) * reticolo.get_msr(1));
		fprintf( stdout, "%f\n", ((double) J / (N * M * T)) * reticolo.get_msr(2));
	}

//	reticolo.mean();
//	reticolo.print_results();

	exit(EXIT_SUCCESS);
} /* ----------  end of function main  ---------- */
