#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define n 8

void printMat(float**, int, int, int);
void printVett(int*, int, int);
float **allocaMatrice(int, int);

int main(int argc, char *argv[])
{
	int myrank, numprocs;
	int namelen;
	int i, j;
	float k;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	float** matrice=NULL;
	float** sotto_matrice=NULL;
	float *data_matrice=NULL,*data_sotto_matrice=NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Get_processor_name(processor_name, &namelen);
	
	//Definizione nuovi tipi per la colonna
	MPI_Datatype acol;
	MPI_Type_vector(n, n/numprocs, n, MPI_FLOAT, &acol);
	MPI_Type_commit(&acol);
	MPI_Datatype acoltype;
	MPI_Type_create_resized(acol, 0, (n/numprocs)*sizeof(float), &acoltype);
	MPI_Type_commit(&acoltype);

	
	//Controlli di robustezza delegati al processo 0
	if (myrank == 0) {

		if (numprocs < 0) {
			printf("Errore! Il numero dei processi deve essere positivo\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		
		if (n <= 0) {
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
	matrice = (float**)malloc(n * sizeof(float*));	

	if (myrank == 0) {
		
		//Allocazione matrice da distribuire
		//matrice = allocaMatrice(n,n);
		
		//Allocazione matrice da distribuire
		data_matrice = (float*)malloc(n*n * sizeof(float));
		for (i = 0; i < n; i++) {
			matrice[i] = &(data_matrice[i*n]);
		}

		//Inizializzazione matrice da distribuire
		k = 1.0;
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {

				matrice[i][j] = k++;
			}
		}

		printMat(matrice, n, n, myrank);
	}

	
	MPI_Barrier(MPI_COMM_WORLD);


	//Allocazione sotto_matrice in cui ricevere in tutti i processi
	//sotto_matrice = allocaMatrice(n,n/numprocs);
	//Allocazione sotto_matrice in cui ricevere in tutti i processi
	data_sotto_matrice = (float*)malloc((n/numprocs) *n* sizeof(float));
	sotto_matrice = (float**)malloc(n * sizeof(float*));
	for (i = 0; i < n; i++) {
		sotto_matrice[i] = &(data_sotto_matrice[i*(n/numprocs)]);
	}
	
	//Scatter
	MPI_Scatter(&matrice[0][0], 1, acoltype, &sotto_matrice[0][0], n*(n/numprocs), MPI_FLOAT, 0, MPI_COMM_WORLD);

	
	//Stampa in ordine
	for (i = 0; i < numprocs; i++) {

		if (myrank == i) {

			printf("Sono il processo %d e devo stampare la matrice di %d righe e %d colonne\n", i, n, n / numprocs);
			printMat(sotto_matrice, n, n/numprocs, i);
			printf("Processo %d: ho finito la stampa\n", i);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}


	//Libero la memoria nel processo root
	if (myrank == 0) {
		free(matrice);
		matrice=NULL;
		free(data_matrice);
		data_matrice=NULL;
	}

	
	//Libero la memoria in tutti i processi
	free(sotto_matrice);
	sotto_matrice=NULL;
	free(data_sotto_matrice);
	data_sotto_matrice=NULL;
	
	MPI_Type_free(&acol);
	MPI_Type_free(&acoltype);

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


float **allocaMatrice(int r, int c){

  int i;

  float *data = (float*)malloc(r*c*sizeof(float));
  float **array = (float**)malloc(r*sizeof(float*));
  
  for (i=0; i<r; i++)
    array[i] = &(data[i*c]);

  return array;
}
