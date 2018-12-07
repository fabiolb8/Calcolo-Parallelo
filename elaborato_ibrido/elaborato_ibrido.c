#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define m 500000
#define n 1000
#define dim_vett 1000

void printVettF(double*, int, int);

int main(int argc, char *argv[])
{
	int myrank, numprocs;
	int namelen;
	int i, j;
	double k;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	double* data_matrice = NULL;
	double* data_sotto_matrice = NULL;
	double** matrice = NULL;
	double** sotto_matrice = NULL;
	double* vettore = NULL;
	double* risultato = NULL;
	double* risultato_parziale = NULL;
	int *displs = NULL, *dimRecv = NULL;
	int *displs_risultato = NULL, *dimRecv_risultato = NULL;
	double t1, t2, temp=0.0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Get_processor_name(processor_name, &namelen);
	MPI_Status info;

	//Controlli di robustezza delegati al processo 0
	if (myrank == 0) {

		if (atoi(getenv("OMP_NUM_THREADS")) <= 0) {
			printf("Errore! Il numero dei threads deve essere positivo\n");
			exit(1);
		}

		if (numprocs <= 0) {
			printf("Errore! Il numero dei processi deve essere positivo\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		if ((floor((double)m) != (double)m) || (floor((double)n) != (double)n) || (floor((double)dim_vett) != (double)dim_vett)) {
			printf("Errore! Le dimensioni della matrice e del vettore devono essere valori interi\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		if (m <= 0 || n <= 0 || dim_vett <= 0) {
			printf("Errore! Le dimensioni dei dati di input non sono valori positivi.\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		if (m < numprocs) {
			printf("Errore! La dimensione di riga della matrice è inferiore al numero di processori.\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		if (n != dim_vett) {
			printf("Errore! Le dimensioni non sono coerenti\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

	}
	MPI_Barrier(MPI_COMM_WORLD);

	printf("Ciao sono il processo %d e mi chiamo %s\n", myrank, processor_name);
	printf("[Thread %d] Il programma sarà eseguito con %d threads per processo\n", omp_get_thread_num(), atoi(getenv("OMP_NUM_THREADS")));

	matrice = (double**)malloc((int)m * sizeof(double*));
	vettore = (double*)malloc((int)n * sizeof(double));

	if (myrank == 0) {

		//Inizializzazione matrice
		data_matrice = (double*)malloc((int)m*(int)n * sizeof(double));
		for (i = 0; i < m; i++) {

			matrice[i] = &(data_matrice[i*n]);
		}

		for (i = 0; i < m; i++) {
			for (j = 0; j < n; j++) {

				matrice[i][j] = (double)i + 1.0;
			}
		}

		//Inizializzazione vettore
		for (i = 0; i < n; i++) {

			vettore[i] = M_PI;
		}

		//Allocazione risultato nel processo root
		risultato = (double*)malloc((int)m * sizeof(double));

	}

	//Allocazione parametri per scatterv e gatherv
	dimRecv = (int*)calloc(numprocs, sizeof(int));
	displs = (int*)calloc(numprocs, sizeof(int));
	dimRecv_risultato = (int*)calloc(numprocs, sizeof(int));
	displs_risultato = (int*)calloc(numprocs, sizeof(int));


	//Inizializzazione parametri per scatterv e gatherv delegati al processo 0
	if (myrank == 0) {


		//Inizializzazione dimRecv per scatterv

		for (i = 0; i < numprocs; i++) {

			if (i < ((int)m%numprocs)) {
				//Devo ricevere i+1 elementi
				dimRecv[i] = (int)(m / numprocs + 1)*n;
			}
			else {
				//Devo ricevere i elementi
				dimRecv[i] = (int)(m / numprocs)*n;
			}
		}

		//Inizializzazione dimRecv per gatherv

		for (i = 0; i < numprocs; i++) {

			if (i < ((int)m%numprocs)) {
				//Devo ricevere i+1 elementi
				dimRecv_risultato[i] = (int)(m / numprocs + 1);
			}
			else {
				//Devo ricevere i elementi
				dimRecv_risultato[i] = (int)(m / numprocs);
			}
		}

		//Inizializzazione displs per scatterv

		displs[0] = 0;
		for (i = 1; i < numprocs; i++) {

			displs[i] = displs[i - 1] + dimRecv[i - 1];
		}


		//Inizializzazione displs_risultato per gatherv

		displs_risultato[0] = 0;
		for (i = 1; i < numprocs; i++) {

			displs_risultato[i] = displs_risultato[i - 1] + dimRecv_risultato[i - 1];
		}
	}


	//Broadcast
	MPI_Bcast(dimRecv, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(dimRecv_risultato, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs_risultato, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(vettore, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);


	//Allocazione sottovettore risultato parziale in tutti i processi
	risultato_parziale = (double*)malloc(dimRecv_risultato[myrank] * sizeof(double));


	//Allocazione sotto matrice in cui ricevere in tutti i processi
	data_sotto_matrice = (double*)malloc(dimRecv[myrank] * sizeof(double));
	sotto_matrice = (double**)malloc(dimRecv[myrank] / n * sizeof(double*));

	for (i = 0; i < dimRecv[myrank] / n; i++) {

		sotto_matrice[i] = &(data_sotto_matrice[i*n]);
	}


	//Scatterv
	MPI_Scatterv(&matrice[0][0], dimRecv, displs, MPI_DOUBLE, &sotto_matrice[0][0], dimRecv[myrank], MPI_DOUBLE, 0, MPI_COMM_WORLD);


	if (myrank == 0) {
		//Inizio tempo
		t1 = MPI_Wtime();
	}

	//Prodotto matrice vettore
	//prodottoMatriceVettore(sotto_matrice, dimRecv[myrank] / n, n, vettore, risultato_parziale);
	#pragma omp parallel for firstprivate(temp) private(i,j) shared (sotto_matrice, vettore, risultato)
	for (i = 0; i < dimRecv[myrank] / n; i++) {
		for (j = 0; j < n; j++) {
			temp += sotto_matrice[i][j] * vettore[j];
		}
		risultato_parziale[i] = temp; //ATTENZIONE : FALSE SHARING A LIVELLO DI RIGA !!!!! (?)
		temp=0.0;
	}

	//Gatherv del risultato
	MPI_Gatherv(risultato_parziale, dimRecv_risultato[myrank], MPI_DOUBLE, risultato, dimRecv_risultato, displs_risultato, MPI_DOUBLE, 0, MPI_COMM_WORLD);


	//Stampa risultato finale
	if (myrank == 0) {

		//Fine tempo
		t2 = MPI_Wtime();

		printf("Sono il processo %d. Il vettore risultato di %d righe è :\n", myrank, m);
		//printVettF(risultato, m, myrank);
		printf("\nSono il processo %d: tempo di esecuzione totale di %.16lf secondi.\n", myrank, t2 - t1);
	}



	if (myrank == 0) {

		//Libero la memoria nel processo root
		free(matrice);
		matrice = NULL;
		free(data_matrice);
		data_matrice = NULL;
		free(risultato);
		risultato = NULL;
	}


	//Libero memoria di tutti i processori
	free(vettore);
	vettore = NULL;
	free(risultato_parziale);
	risultato_parziale = NULL;
	free(sotto_matrice);
	sotto_matrice = NULL;
	free(data_sotto_matrice);
	data_sotto_matrice = NULL;
	free(displs);
	displs = NULL;
	free(dimRecv);
	dimRecv = NULL;
	free(displs_risultato);
	displs_risultato = NULL;
	free(dimRecv_risultato);
	dimRecv_risultato = NULL;


	MPI_Finalize();
	return 0;
}


void printVettF(double* array, int c, int rank) {

	int i;

	printf("Stampa array del processo %d: ", rank);

	for (i = 0; i < c; i++) {

		printf("\n%.16lf ", array[i]);
	}
	printf("\n");
}
