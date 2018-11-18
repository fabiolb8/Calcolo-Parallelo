#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define m 7
#define n 8
#define dim_vett 8

void printMat(double**, int, int, int);
void printVett(double*, int, int);
void prodottoMatriceVettore(double**, int, int, double*, double*);

int main(int argc, char *argv[])
{
	int myrank, numprocs;
	int namelen;
	int i, j;
	double k;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	double** matrice=NULL;
	double** sotto_matrice=NULL;
	double *data_matrice,*data_sotto_matrice;
	double *risultato, *risultato_parziale;
	double *vettore, *vettore_parziale;
	double t1, t2;
	int *displs = NULL, *dimRecvCol = NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Get_processor_name(processor_name, &namelen);
	MPI_Status info;


	//Controlli di robustezza delegati al processo 0
	if (myrank == 0) {

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

		if (n < numprocs) {
			printf("Errore! La dimensione di colonna della matrice è inferiore al numero di processori.\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		if (n != dim_vett) {
			printf("Errore! Le dimensioni non sono coerenti\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

	}
	MPI_Barrier(MPI_COMM_WORLD);


	printf("Ciao sono il processo %d e mi chiamo %s\n", myrank, processor_name);
	matrice = (double**)malloc(m * sizeof(double*));

	if (myrank == 0) {

		//Allocazione matrice da distribuire
		data_matrice = (double*)malloc(m*n * sizeof(double));
		for (i = 0; i < m; i++) {
			matrice[i] = &(data_matrice[i*(int)n]);
		}

		//Inizializzazione matrice da distribuire
		for (i = 0; i < m; i++) {
			for (j = 0; j < n; j++) {

				matrice[i][j] = (double) i +1.0;
			}
		}

		printMat(matrice, m, n, myrank);
		
		//Allocazione vettore
		vettore = (double*)malloc(dim_vett * sizeof(double));
		for (i = 0; i < dim_vett; i++) {
			vettore[i] = M_PI;
		}
		
		//Allocazione vettore risultato
		risultato = (double*)malloc(m * sizeof(double));

	}


	MPI_Barrier(MPI_COMM_WORLD);


	//Allocazione parametri per scatterv
	dimRecvCol = (int*)calloc(numprocs, sizeof(int));
	displs = (int*)calloc(numprocs, sizeof(int));

	//Inizializzazione parametri per scatterv delegati al processo 0
	if (myrank == 0) {

		//Inizializzazione dimRecvCol

		for (i = 0; i < numprocs; i++) {

			if (i < ((int)n%numprocs)) {
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
	
	//Allocazione vettori per tutti i processi
	vettore_parziale = (double*) malloc (dimRecvCol[myrank]*sizeof(double));
	risultato_parziale = (double*) malloc (m*sizeof(double));

	//Definizione nuovi tipi per la colonna da inviare
	MPI_Datatype acol,acoltype;
	MPI_Type_vector(m,1,n, MPI_DOUBLE, &acol);
	MPI_Type_commit(&acol);
	MPI_Type_create_resized(acol, 0, 1 * sizeof(double), &acoltype);
	MPI_Type_commit(&acoltype);

	//Definizione nuovi tipi per la colonna da ricevere
	MPI_Datatype ccol,ccoltype;
	MPI_Type_vector(m, 1, dimRecvCol[myrank], MPI_DOUBLE, &ccol);
	MPI_Type_commit(&ccol);
	MPI_Type_create_resized(ccol, 0, 1 * sizeof(double), &ccoltype);
	MPI_Type_commit(&ccoltype);

	//Allocazione sotto_matrice in cui ricevere in tutti i processi
	data_sotto_matrice = (double*)malloc(dimRecvCol[myrank] * m*sizeof(double));
	sotto_matrice = (double**)malloc(m * sizeof(double*));
	for (i = 0; i < m; i++) {
		sotto_matrice[i] = &(data_sotto_matrice[i*(dimRecvCol[myrank])]);
	}

	//Scatterv del vettore
	MPI_Scatterv(&vettore[0], dimRecvCol, displs, MPI_DOUBLE, &vettore_parziale[0], dimRecvCol[myrank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

	//Scatterv della matrice
	MPI_Scatterv(&matrice[0][0], dimRecvCol, displs, acoltype, &sotto_matrice[0][0], dimRecvCol[myrank]*m, ccoltype, 0, MPI_COMM_WORLD);
	
	if (myrank == 0) {
		//Inizio tempo
		t1 = MPI_Wtime();
	}

	//Prodotto matrice vettore in parallelo
	prodottoMatriceVettore(sotto_matrice, m, dimRecvCol[myrank], vettore_parziale, risultato_parziale);

	//Reduce per il risultato in root
	MPI_Reduce(risultato_parziale, risultato, m, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	//Stampa risultato finale
	if (myrank == 0) {

		//Fine tempo
		t2 = MPI_Wtime();

		printf("Sono il processo %d. Il vettore risultato di %d righe è :\n", myrank, m);
		printVett(risultato, m, myrank);
		printf("\nSono il processo %d: tempo di esecuzione totale di %.16lf secondi.\n", myrank, t2 - t1);
	}


	//Libero la memoria nel processo root
	if (myrank == 0) {
		free(matrice);
		matrice = NULL;
		free(data_matrice);
		data_matrice = NULL;
		free(vettore);
		vettore = NULL;
		free(risultato);
		risultato = NULL;
	}


	//Libero la memoria in tutti i processi
	free(sotto_matrice);
	sotto_matrice = NULL;
	free(risultato_parziale);
	risultato_parziale = NULL;
	free(vettore_parziale);
	vettore_parziale = NULL;
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



void printMat(double** data, int r, int c, int rank) {

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

void printVett(double* vett, int dim, int myrank){

	int i;

	for (i = 0;i < dim;i++) {
		
		printf("%.16lf \n", vett[i]);
		
	}

	printf("\n");
}

void prodottoMatriceVettore(double** matrice, int r, int c, double* vettore, double* risultato) {

	int i, j;

	for (i = 0; i < r; i++) {

		risultato[i] = 0.0;

		for (j = 0; j < c; j++) {

			risultato[i] = risultato[i] + matrice[i][j] * vettore[j];

		}
	}
}
