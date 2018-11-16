#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define numero_processori 4
#define dim 10000
#define dim_x dim
#define dim_y dim

int main(int argc, char *argv[])
{
	int myrank, numprocs;
	int namelen;
	int i, j;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	double *x = NULL, *y = NULL, *part_x = NULL, *part_y = NULL;
	double risultato = 0, part_risultato = 0;
	double t1, t2;
	int *displs = NULL, *dimRecv = NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Get_processor_name(processor_name, &namelen);
	MPI_Status info;

	//Controlli di robustezza delegati al processo 0
	if (myrank == 0) {

		if (numprocs != numero_processori) {
			printf("Errore! Il numero dei processi deve essere pari a %d\n", numero_processori);
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		
		if (dim <= 0) {
			printf("Errore! La dimensione dei vettori non è positiva.\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		
		if (dim_x != dim_y) {
			printf("Errore! Le dimensioni dei due vettori sono diverse\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		if (dim < numprocs) {
			printf("Errore! La dimensione non è coerente con il numero di processori.\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
	}////Fine controlli di robustezza
	MPI_Barrier(MPI_COMM_WORLD);


	printf("Ciao sono il processo %d e mi chiamo %s\n", myrank, processor_name);


	if (myrank == 0) {

		//Inizializzazione vettori da distribuire	
		x = (double*)calloc(dim_x, sizeof(double));
		y = (double*)calloc(dim_y, sizeof(double));

		for (i = 0;i < dim;i++) {
			x[i] = M_PI;
			y[i] = 1.0;
		}


	}

	//Allocazione vettori comuni a tutti i processori
	dimRecv = (int*)calloc(numprocs, sizeof(int));
	displs = (int*)calloc(numprocs, sizeof(int));

	//Inizializzazione parametri per scatterv delegati al processo 0
	if (myrank == 0) {

		//Inizializzazione dimRecv

		for (i = 0; i < numprocs; i++) {

			if (i < (dim%numprocs)) {
				//Devo ricevere i+1 elementi
				dimRecv[i] = (int)dim / numprocs + 1;
			}
			else {
				//Devo ricevere i elementi
				dimRecv[i] = (int)dim / numprocs;
			}
		}

		//Inizializzazione displs

		displs[0] = 0;
		for (i = 1; i < numprocs; i++) {

			displs[i] = displs[i - 1] + dimRecv[i - 1];
		}

	}//Fine inizializzazione parametri scatterv

	//Broadcast dei vettori dimRecv e displs
	MPI_Bcast(dimRecv, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs, numprocs, MPI_INT, 0, MPI_COMM_WORLD);

	//Allocazione vettori per la ricezione
	part_x = (double*)calloc(dimRecv[myrank], sizeof(double));
	part_y = (double*)calloc(dimRecv[myrank], sizeof(double));

	//Distribuzione vettori x e y
	MPI_Scatterv(x, dimRecv, displs, MPI_DOUBLE, part_x, dimRecv[myrank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatterv(y, dimRecv, displs, MPI_DOUBLE, part_y, dimRecv[myrank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (myrank == 0) {

		//Libero la memoria per i vettori x e y
		free(x);
		x = NULL;
		free(y);
		y = NULL;

		//Inizio tempo
		t1 = MPI_Wtime();
	}

	//Prodotto scalare parziale
	for (i = 0; i < dimRecv[myrank]; i++) {

		part_risultato = part_risultato + (part_x[i] * part_y[i]);
	}

	//Stampa in ordine del prodotto scalare i-esimo
	for (i = 0; i < numprocs; i++) {
		if (myrank == i) {
			printf("Sono il processo %d. Il mio prodotto scalare parziale è: %.16lf\n", i, part_risultato);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	//Reduce per la somma dei risultati parziali
	MPI_Reduce(&part_risultato, &risultato, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	//Stampa risultato finale
	if (myrank == 0) {

		//Fine tempo
		t2 = MPI_Wtime();

		printf("Sono il processo %d. Il prodotto scalare è pari a : %.16lf.\nSono il processo %d : tempo di esecuzione totale di %.16lf secondi.\n", myrank, risultato, myrank, t2 - t1);
	}

	//Libero la memoria in tutti i processi 
	free(part_x);
	part_x = NULL;
	free(part_y);
	part_y = NULL;
	free(dimRecv);
	dimRecv = NULL;
	free(displs);
	displs = NULL;


	MPI_Finalize();
	return EXIT_SUCCESS;
}
