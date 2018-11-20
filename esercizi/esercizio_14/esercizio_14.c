#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define up		0
#define down	1
#define left	2
#define right	3
#define dim		2
#define num_vicini	4

int main(int argc, char *argv[])
{
	int myrank, numprocs;
	int namelen;
	int i, j;
	int somma, count;
	float media;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int mio_valore;
	int coords[dim];
	int dims[dim]={3,3};
	//int periods[dim]={1,1};		//Caso periodico
	int periods[dim]={0,0};			//Caso non periodico
	int vicini[num_vicini];
	float* valore_medie;

	MPI_Comm griglia;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	
	//Controlli di robustezza delegati al processo 0
	if (myrank == 0) {

		//Controllo numprocs coerente con definizione griglia
		;

		valore_medie = (float*) malloc (numprocs*sizeof(float));

	}
	MPI_Barrier(MPI_COMM_WORLD);
	
	MPI_Cart_create(MPI_COMM_WORLD, dim, dims, periods, 0, &griglia);
	MPI_Comm_rank(griglia, &myrank);
	MPI_Cart_coords(griglia, myrank, dim, coords);
	MPI_Get_processor_name(processor_name, &namelen);
	MPI_Status info;

	//Stampa in ordine dei processi
	for (i = 0; i < numprocs; i++) {

		if (myrank == i) {
			printf("Ciao sono il processo %d e mi chiamo %s. Le mie coordinate sono: (%d, %d)\n", i, processor_name, coords[0], coords[1]);
		}
		MPI_Barrier(griglia);
	}
	
	//Inizializzazione valore da distribuire
	mio_valore = myrank;

	//Get vicini
	MPI_Cart_shift(griglia, 0, 1, &vicini[up], &vicini[down]);
	MPI_Cart_shift(griglia, 1, 1, &vicini[left], &vicini[right]);
	
	//Stampa in ordine dei processi
	for (i = 0; i < numprocs; i++) {

		if (myrank == i) {
			printf("Ciao sono il processo %d. I rank dei miei vicini sono U:%d, D:%d, L:%d, R:%d\n", myrank, vicini[up], vicini[down], vicini[left], vicini[right]);
		}
		MPI_Barrier(griglia);
	}


	//Calcolo media rank vicini
	somma = mio_valore;
	count = 1;
	for (i=0; i<num_vicini; i++){
		
		if (vicini[i]>=0){
			somma+=vicini[i];	
			count++;
		}
	}
	media = (float) somma/count;

	//Stampa in ordine
	for (i = 0; i < numprocs; i++) {

		if (myrank == i) {
			printf("Ciao sono il processo %d. La media dei rank dei miei vicini e' %f\n", myrank, media);

		}
		MPI_Barrier(griglia);
	}
	MPI_Barrier(griglia);
	
	
	//Mando tutto a 0
	MPI_Gather(&media, 1, MPI_FLOAT, valore_medie, 1, MPI_FLOAT, 0, griglia);

	if (myrank==0){
		
		for (i = 0; i < numprocs; i++) {
			;//printf("Ciao sono il processo %d. La media i e' %f\n", myrank, valore_medie[i]);
		}
	}

	//Libero la memoria nel processo root

	//Libero la memoria in tutti i processi
	

	MPI_Finalize();
	return 0;
}
