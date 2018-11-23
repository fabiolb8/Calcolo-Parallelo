#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define up		0
#define down	1
#define left	2
#define right	3
#define dimensioni	2
#define num_vicini	4
#define q 3		//griglia 3x3
#define m 4
#define n 4

void printVett(int*, int, int);
void printMat(double**, int, int, int);


int main(int argc, char *argv[])
{
	int myrank, numprocs, myrank_col, myrank_row;
	int namelen;
	int i, j;
	int somma, count;
	float media;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	//int mio_valore;
	int coords[dimensioni];
	int dims[dimensioni] = { q,q };
	int free_coords[2];
	//int periods[dim]={1,1};				//Caso griglia periodica
	int periods[dimensioni] = { 0,0 };			//Caso griglia non periodica
	//int vicini[num_vicini];
	//float* valore_medie;
	double** matrice;
	double* data_matrice;
	double **sotto_matrice_per_righe, **sotto_matrice_per_colonne_e_righe;
	double *data_sotto_matrice_per_righe, *data_sotto_matrice_per_colonne_e_righe;
	int *dimRecv_righe, *dimRecv_colonne, *displs_righe, *displs_colonne;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	//Controlli di robustezza delegati al processo 0
	if (myrank == 0) {

		//Controllo numprocs coerente con definizione griglia
		if (numprocs != q*q) {
			printf("Errore! Il numero dei processi deve essere %d\n", q*q);
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	//Definizione griglia di processi 9x9
	MPI_Comm griglia;
	MPI_Cart_create(MPI_COMM_WORLD, dimensioni, dims, periods, 0, &griglia);
	MPI_Comm_rank(griglia, &myrank);
	MPI_Cart_coords(griglia, myrank, dimensioni, coords);
	MPI_Get_processor_name(processor_name, &namelen);
	MPI_Status info;



	//Stampa in ordine dei processi
	for (i = 0; i < numprocs; i++) {

		if (myrank == i) {
			printf("Ciao sono il processo %d e mi chiamo %s. Le mie coordinate sono: (%d, %d)\n", i, processor_name, coords[0], coords[1]);
		}
		MPI_Barrier(griglia);
	}

	/*if(myrank==0){
		valore_medie = (float*) malloc (numprocs*sizeof(float));
	}*/

	matrice = (double**)malloc(m * sizeof(double*));
	if (myrank == 0) {

		//Allocazione matrice da distribuire
		data_matrice = (double*)malloc(m*n * sizeof(double));
		for (i = 0; i < m; i++) {
			matrice[i] = &(data_matrice[i*n]);
		}

		//Inizializzazione matrice da distribuire
		for (i = 0; i < m; i++) {
			for (j = 0; j < n; j++) {

				matrice[i][j] = (double)i + 1.0;
			}
		}

		printMat(matrice, m, n, myrank);
	}


	//Definizione sottogriglie di righe e di colonne
	MPI_Comm row_comm, col_comm;
	//Sottogriglia per righe
	free_coords[0] = 0;
	free_coords[1] = 1;
	MPI_Cart_sub(griglia, free_coords, &row_comm);


	//Sottogriglia per colonne
	free_coords[0] = 1;
	free_coords[1] = 0;
	MPI_Cart_sub(griglia, free_coords, &col_comm);



	//Allocazione parametri per scatterv e gatherv
	dimRecv_righe = (int*)calloc(q, sizeof(int));
	displs_righe = (int*)calloc(q, sizeof(int));
	dimRecv_colonne = (int*)calloc(q, sizeof(int));
	displs_colonne = (int*)calloc(q, sizeof(int));

	//Inizializzazione parametri per scatterv e gatherv delegati al processo 0
	if (myrank == 0) {


		//Inizializzazione dimRecv_righe per scatterv

		for (i = 0; i < q; i++) {

			if (i < ((int)m%q)) {
				//Devo ricevere i+1 elementi
				dimRecv_righe[i] = (int)(m / q + 1)*n;
			}
			else {
				//Devo ricevere i elementi
				dimRecv_righe[i] = (int)(m / q)*n;
			}
		}

		for (i = 0; i < numprocs; i++) {

			if (i < (n%q)) {
				//Devo ricevere i+1 elementi
				dimRecv_colonne[i] = (int)(n / q + 1);
			}
			else {
				//Devo ricevere i elementi
				dimRecv_colonne[i] = (int)(n / q);
			}
		}

		//Inizializzazione displs_righe per scatterv

		displs_righe[0] = 0;
		for (i = 1; i < q; i++) {

			displs_righe[i] = displs_righe[i - 1] + dimRecv_righe[i - 1];
		}

		//Inizializzazione displs_colonne per scatterv

		displs_colonne[0] = 0;
		for (i = 1; i < q; i++) {

			displs_colonne[i] = displs_colonne[i - 1] + dimRecv_colonne[i - 1];
		}
	}


	//Broadcast su col_comm
	MPI_Bcast(dimRecv_righe, q, MPI_INT, 0,MPI_COMM_WORLD);
	MPI_Bcast(displs_righe, q, MPI_INT, 0, MPI_COMM_WORLD);


	//Broadcast su row_comm
	MPI_Bcast(dimRecv_colonne, q, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs_colonne, q, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Comm_rank(col_comm, &myrank_col);
	MPI_Comm_rank(row_comm, &myrank_row);

	sotto_matrice_per_righe = (double**)malloc(dimRecv_righe[myrank_col]/n * sizeof(double*));
	
	//se sto sulla prima colonna della griglia invio la matrice per righe
	if (coords[1] == 0) {

		
		//Allocazione matrice da distribuire
		data_sotto_matrice_per_righe = (double*)malloc(dimRecv_righe[myrank_col] * sizeof(double));
		for (i = 0; i < dimRecv_righe[myrank_col]/n; i++) {
			sotto_matrice_per_righe[i] = &(data_sotto_matrice_per_righe[i*n]);
		}


		//printMat(sotto_matrice_per_righe, dimRecv_righe[myrank], n, myrank);


		MPI_Scatterv(&matrice[0][0],dimRecv_righe,displs_righe,MPI_DOUBLE,&sotto_matrice_per_righe[0][0],dimRecv_righe[myrank_col],MPI_DOUBLE,0,col_comm);
	
		printMat(sotto_matrice_per_righe,dimRecv_righe[myrank_col]/n,n,myrank);

	}

	MPI_Barrier(griglia);

	//Definizione nuovi tipi per la colonna da inviare
	MPI_Datatype acol, acoltype;
	MPI_Type_vector(dimRecv_righe[myrank_col], 1, n, MPI_DOUBLE, &acol);
	MPI_Type_commit(&acol);
	MPI_Type_create_resized(acol, 0, 1 * sizeof(double), &acoltype);
	MPI_Type_commit(&acoltype);

	//Definizione nuovi tipi per la colonna da ricevere
	MPI_Datatype ccol, ccoltype;
	MPI_Type_vector(dimRecv_righe[myrank_col], 1, dimRecv_colonne[myrank_row], MPI_DOUBLE, &ccol);
	MPI_Type_commit(&ccol);
	MPI_Type_create_resized(ccol, 0, 1 * sizeof(double), &ccoltype);
	MPI_Type_commit(&ccoltype);

	

	//tutti devono avere questo
	sotto_matrice_per_colonne_e_righe = (double**)malloc(dimRecv_righe[myrank_col]/n* sizeof(double*));
	//Allocazione matrice da distribuire
		data_sotto_matrice_per_colonne_e_righe = (double*)malloc(dimRecv_colonne[myrank_row]*(dimRecv_righe[myrank_col]/n)*sizeof(double));
		for (i = 0; i < dimRecv_righe[myrank_col] / n; i++) {
			sotto_matrice_per_colonne_e_righe[i] = &(data_sotto_matrice_per_colonne_e_righe[i*dimRecv_colonne[myrank_row]]);
		}


	MPI_Scatterv(&sotto_matrice_per_righe[0][0], dimRecv_colonne, displs_colonne, acoltype, &sotto_matrice_per_colonne_e_righe[0][0], dimRecv_colonne[myrank_row]* (dimRecv_righe[myrank_col]/n), ccoltype, 0, row_comm);

	printMat(sotto_matrice_per_colonne_e_righe, dimRecv_righe[myrank_col] / n, dimRecv_colonne[myrank_row], myrank);




	//Inizializzazione valore da distribuire
	//mio_valore = myrank;

	//Get rank dei miei vicini
	//MPI_Cart_shift(griglia, 0, 1, &vicini[up], &vicini[down]);
	//MPI_Cart_shift(griglia, 1, 1, &vicini[left], &vicini[right]);

	//Stampa in ordine dei processi
	/*for (i = 0; i < numprocs; i++) {

		if (myrank == i) {
			printf("Ciao sono il processo %d. I rank dei miei vicini sono U:%d, D:%d, L:%d, R:%d\n", myrank, vicini[up], vicini[down], vicini[left], vicini[right]);
		}
		MPI_Barrier(griglia);
	}*/


	/*
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
	*/


	/*

	//Stampa in ordine
	for (i = 0; i < numprocs; i++) {

		if (myrank == i) {
			printf("Ciao sono il processo %d. La media dei rank dei miei vicini e' %f\n", myrank, media);
		}
		MPI_Barrier(griglia);
	}


	//Mando le medie a 0
	MPI_Gather(&media, 1, MPI_FLOAT, valore_medie, 1, MPI_FLOAT, 0, griglia);

	if (myrank==0){


		printf("Ciao sono il processo %d. Le medie sono: ", myrank);

		for (i = 0; i < numprocs; i++)
			printf("%f\n",valore_medie[i]);

		printf("\n");

	}

	//Libero la memoria nel processo root
	if(myrank==0)
		free(valore_medie);


	*/

	
	//if (myrank == 0) {

		
		printf("dimRecv_righe:\n");
		printVett(dimRecv_righe, q, myrank);

		printf("dimRecv_colonne:\n");
		printVett(dimRecv_colonne, q, myrank);


		printf("displs_righe:\n");
		printVett(displs_righe, q, myrank);

		printf("displs_colonne:\n");
		printVett(displs_colonne, q, myrank);
		

	




	MPI_Finalize();
	return 0;
}




void printVett(int* vett, int dim, int myrank) {

	int i;

	printf("Sono il processo %d , stampo il vettore \n", myrank);

	for (i = 0;i < dim;i++) {

		printf("%d \t", vett[i]);

	}

	printf("\n");
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
