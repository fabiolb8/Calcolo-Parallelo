#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define m 4
#define n 10

void printMat(float**, int, int, int);
void printVett(int*, int, int);

int main(int argc, char *argv[])
{
	int myrank, numprocs;
	int namelen;
	int i, j;
	float k;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	float** matrice=NULL;
	float** sotto_matrice=NULL;
	float *data_matrice,*data_sotto_matrice;
	int *displs = NULL, *dimRecvCol = NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Get_processor_name(processor_name, &namelen);
	MPI_Status info;


	//Controlli di robustezza delegati al processo 0
	if (myrank == 0) {

		if (numprocs < 0) {
			printf("Errore! Il numero dei processi deve essere positivo\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		if (n <= 0 || m <= 0) {
			printf("Errore! La dimensione (di riga e colonna) della matrice deve essere positiva\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		if (n < numprocs) {
			printf("Errore! La dimensione (di riga e colonna) della matrice è minore del numero di processori\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

	}
	MPI_Barrier(MPI_COMM_WORLD);


	printf("Ciao sono il processo %d e mi chiamo %s\n", myrank, processor_name);
	matrice = (float**)malloc(m * sizeof(float*));

	if (myrank == 0) {

		//Allocazione matrice da distribuire
		data_matrice = (float*)malloc(m*n * sizeof(float));
		for (i = 0; i < m; i++) {
			matrice[i] = &(data_matrice[i*n]);
		}

		//Inizializzazione matrice da distribuire
		k = 1.0;
		for (i = 0; i < m; i++) {
			for (j = 0; j < n; j++) {

				matrice[i][j] = k++;
			}
		}

		printMat(matrice, m, n, myrank);

	}


	MPI_Barrier(MPI_COMM_WORLD);


	//Allocazione parametri per scatterv
	dimRecvCol = (int*)calloc(numprocs, sizeof(int));
	displs = (int*)calloc(numprocs, sizeof(int));

	//Inizializzazione parametri per scatterv delegati al processo 0
	if (myrank == 0) {

		//Inizializzazione dimRecvCol

		for (i = 0; i < numprocs; i++) {

			if (i < (n%numprocs)) {
				//Devo ricevere i+1 elementi
				dimRecvCol[i] = (int)(n / numprocs + 1);
			}
			else {
				//Devo ricevere i elementi
				dimRecvCol[i] = (int)(n / numprocs);
			}
		}


		//Inizializzazione displs

		displs[0] = 0;
		for (i = 1; i < numprocs; i++) {

			displs[i] = displs[i - 1] + dimRecvCol[i - 1];
		}
	}


	
	//Broadcast
	MPI_Bcast(dimRecvCol, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	

	//Definizione nuovi tipi per la colonna da inviare
	MPI_Datatype acol,acoltype;
	MPI_Type_vector(m,1,n, MPI_FLOAT, &acol);
	MPI_Type_commit(&acol);
	MPI_Type_create_resized(acol, 0, 1 * sizeof(float), &acoltype);
	MPI_Type_commit(&acoltype);

	//Definizione nuovi tipi per la colonna da ricevere
	MPI_Datatype ccol,ccoltype;
	MPI_Type_vector(m, 1, dimRecvCol[myrank], MPI_FLOAT, &ccol);
	MPI_Type_commit(&ccol);
	MPI_Type_create_resized(ccol, 0, 1 * sizeof(float), &ccoltype);
	MPI_Type_commit(&ccoltype);

	//Allocazione sotto_matrice in cui ricevere in tutti i processi
	data_sotto_matrice = (float*)malloc(dimRecvCol[myrank] * m*sizeof(float));
	sotto_matrice = (float**)malloc(m * sizeof(float*));
	for (i = 0; i < m; i++) {
		sotto_matrice[i] = &(data_sotto_matrice[i*(dimRecvCol[myrank])]);
	}

	//Scatterv
	MPI_Scatterv(&matrice[0][0], dimRecvCol, displs, acoltype, &sotto_matrice[0][0], dimRecvCol[myrank]*m, ccoltype, 0, MPI_COMM_WORLD);

	//Stampa in ordine
	for (i = 0; i < numprocs; i++) {

		if (myrank == i) {

			printf("Sono il processo %d e devo stampare la matrice di %d righe e %d colonne\n", i, m, dimRecvCol[myrank]);
			printMat(sotto_matrice, m, dimRecvCol[myrank], i);
			printf("Processo %d: ho finito la stampa\n", i);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}


	//Libero la memoria nel processo root
	if (myrank == 0) {
		free(matrice);
		matrice = NULL;
		free(data_matrice);
		data_matrice = NULL;
	}


	//Libero la memoria in tutti i processi
	free(sotto_matrice);
	sotto_matrice = NULL;
	free(data_sotto_matrice);
	data_sotto_matrice = NULL;
	free(displs);
	displs = NULL;
	free(dimRecvCol);
	dimRecvCol = NULL;
	
	MPI_Type_free(&acol);
	MPI_Type_free(&acoltype);
	MPI_Type_free(&ccol);
	MPI_Type_free(&ccoltype);

	MPI_Finalize();
	return 0;
}



void printMat(float** data, int r, int c, int rank) {

	int i = 0, j = 0;

	printf("Sono il processo %d e stampo matrice r=%d x c=%d: \n", rank, r, c);

	for (i = 0; i < r; i++) {

		printf("Riga %d: ", i);

		for (j = 0; j < c; j++) {

			printf("%f \t", data[i][j]);
		}
		printf("\n");
	}
}


void printVett(int* vett, int dim, int myrank){

	int i;

	printf("Sono il processo %d , stampo il vettore \n",myrank);

	for (i = 0;i < dim;i++) {
		
		printf("%d \t", vett[i]);
		
	}

	printf("\n");
}
