#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define q 3				//griglia 3x3
#define m 7
#define n 8
#define dim_vett 8

void printVett(double*, int, int);
void printMat(double**, int, int, int);
void prodottoMatriceVettore(double**, int, int, double*, double*);


int main(int argc, char *argv[])
{
	int myrank, numprocs, myrank_col, myrank_row;
	int namelen;
	int i, j;
	double k;
	int somma, count;
	float media;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int coords[2];
	int dims[2] = { q,q };
	int free_coords[2];							
	int periods[2] = { 0,0 };			//Caso griglia non periodica
	double** matrice=NULL;
	double* data_matrice = NULL;
	double **sotto_matrice_per_righe = NULL, **sotto_matrice_per_colonne_e_righe = NULL;
	double *data_sotto_matrice_per_righe = NULL, *data_sotto_matrice_per_colonne_e_righe = NULL;
	int *dimRecv_righe = NULL, *dimRecv_colonne = NULL, *displs_righe = NULL, *displs_colonne = NULL,*displs_risultato = NULL;
	double *vettore,*risultato;
	double *risultato_parziale,*vettore_parziale,*risultato_parziale_col0;
	double t1, t2;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);


	//Controlli di robustezza delegati al processo 0
	if (myrank == 0) {

		if (numprocs <= 0) {
			printf("Errore! Il numero dei processi deve essere positivo\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		
		//q negativo
		
		//q non intero
		
		//Controllo numprocs coerente con definizione griglia
		if (numprocs != q * q) {
			printf("Errore! Il numero dei processi deve essere %d\n", q*q);
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

	
	//Definizione griglia di processi
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


	matrice = (double**)malloc(m * sizeof(double*));
	if (myrank == 0) {

		k=1.0;
	
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
	displs_risultato = (int*)calloc(q, sizeof(int));

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

		for (i = 0; i < q; i++) {

			if (i < ((int)n%q)) {
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
		
		
		//Inizializzazione displs_risultato per gatherv

		displs_risultato[0] = 0;
		for (i = 1; i < q; i++) {

			displs_risultato[i] = displs_risultato[i - 1] + dimRecv_righe[i - 1]/n;
		}
	}


	//Broadcast
	MPI_Bcast(dimRecv_righe, q, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs_righe, q, MPI_INT, 0, MPI_COMM_WORLD);
	//Broadcast
	MPI_Bcast(dimRecv_colonne, q, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs_colonne, q, MPI_INT, 0, MPI_COMM_WORLD);

	//Calcolo dei rank locali nelle sottogriglie
	MPI_Comm_rank(col_comm, &myrank_col);
	MPI_Comm_rank(row_comm, &myrank_row);

	
	//FASE 1.1 : SCATTERV PER RIGHE DELLA MATRICE NELLA PRIMA COLONNA DELLA GRIGLIA
	
	
	sotto_matrice_per_righe = (double**)malloc(dimRecv_righe[myrank_col] / n * sizeof(double*));

	//se sto sulla prima colonna della griglia invio la matrice per righe
	if (coords[1] == 0) {

		//Allocazione matrice da distribuire
		data_sotto_matrice_per_righe = (double*)malloc(dimRecv_righe[myrank_col] * sizeof(double));
		for (i = 0; i < dimRecv_righe[myrank_col] / n; i++) {
			sotto_matrice_per_righe[i] = &(data_sotto_matrice_per_righe[i*n]);
		}


		MPI_Scatterv(&matrice[0][0], dimRecv_righe, displs_righe, MPI_DOUBLE, &sotto_matrice_per_righe[0][0], dimRecv_righe[myrank_col], MPI_DOUBLE, 0, col_comm);
		
		
		
		//Allocazione vettore
		vettore = (double*)malloc(dim_vett * sizeof(double));
		
		
		//Allocazione vettore risultato_parziale della reduce sulla prima colonna
		risultato_parziale_col0 = (double*)malloc(dimRecv_righe[myrank_col]/n* sizeof(double));
		
		
	
	//FASE 1.2 : BROADCAST DEL VETTORE SULLA PRIMA COLONNA DELLA GRIGLIA
		
		if(myrank==0){
		
			k=1.0;
			//Inizializzazione vettore solo nel processo root
			for (i = 0; i < dim_vett; i++) {
				vettore[i] = M_PI;
			}
			
			
			//Allocazione vettore risultato solo nel processo root
			risultato = (double*)malloc(m * sizeof(double));
		}
		
		//Broadcast del vettore solo sulla colonna 1 della griglia
		MPI_Bcast(vettore, dim_vett, MPI_DOUBLE, 0, col_comm);
		
	}

	MPI_Barrier(griglia);
	
	
	

	//Definizione nuovi tipi per la colonna da inviare
	MPI_Datatype acol, acoltype;
	MPI_Type_vector(dimRecv_righe[myrank_col]/n, 1, n, MPI_DOUBLE, &acol);
	MPI_Type_commit(&acol);
	MPI_Type_create_resized(acol, 0, 1 * sizeof(double), &acoltype);
	MPI_Type_commit(&acoltype);

	//Definizione nuovi tipi per la colonna da ricevere
	MPI_Datatype ccol, ccoltype;
	MPI_Type_vector(dimRecv_righe[myrank_col]/n, 1, dimRecv_colonne[myrank_row], MPI_DOUBLE, &ccol);
	MPI_Type_commit(&ccol);
	MPI_Type_create_resized(ccol, 0, 1 * sizeof(double), &ccoltype);
	MPI_Type_commit(&ccoltype);


	//FASE 2.1 : DISTRIBUZIONE DATI NELLA SOTTOGRIGLIE DI RIGHE

	//Allocazione blocco in cui ricevere
	sotto_matrice_per_colonne_e_righe = (double**)malloc(dimRecv_righe[myrank_col] / n * sizeof(double*));
	data_sotto_matrice_per_colonne_e_righe = (double*)malloc(dimRecv_colonne[myrank_row] * (dimRecv_righe[myrank_col] / n) * sizeof(double));
	for (i = 0; i < dimRecv_righe[myrank_col] / n; i++) {
		sotto_matrice_per_colonne_e_righe[i] = &(data_sotto_matrice_per_colonne_e_righe[i*dimRecv_colonne[myrank_row]]);
	}


	//Scatterv per colonne della matrice
	MPI_Scatterv(&sotto_matrice_per_righe[0][0], dimRecv_colonne, displs_colonne, acoltype, &sotto_matrice_per_colonne_e_righe[0][0], dimRecv_colonne[myrank_row] * (dimRecv_righe[myrank_col] / n), ccoltype, 0, row_comm);

	
	//Allocazione vettore_parziale e risultato_parziale per tutti i processi
	vettore_parziale = (double*) malloc (dimRecv_colonne[myrank_row]*sizeof(double));
	risultato_parziale = (double*) malloc (dimRecv_righe[myrank_col]/n*sizeof(double));
	
	
	//Scatterv per righe del vettore
	MPI_Scatterv(vettore, dimRecv_colonne, displs_colonne, MPI_DOUBLE,vettore_parziale, dimRecv_colonne[myrank_row], MPI_DOUBLE, 0, row_comm);
	
	
	if (myrank == 0) {
		//Inizio tempo
		t1 = MPI_Wtime();
	}
	
	
	//Prodotto matrice vettore in parallelo in tutti i processi
	prodottoMatriceVettore(sotto_matrice_per_colonne_e_righe, dimRecv_righe[myrank_col] / n, dimRecv_colonne[myrank_row], vettore_parziale, risultato_parziale);

	
	//Reduce per il risultato nel communicator di righe
	MPI_Reduce(risultato_parziale,risultato_parziale_col0, dimRecv_righe[myrank_col] / n, MPI_DOUBLE, MPI_SUM, 0, row_comm);
	
	
	if (coords[1] == 0) {
		
		//Gatherv del risultato
		MPI_Gatherv(risultato_parziale_col0,dimRecv_righe[myrank_col]/n, MPI_DOUBLE, risultato, dimRecv_righe, displs_risultato, MPI_DOUBLE,0, col_comm);
	
	}
	
	
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
		free(data_matrice);
		free(risultato);
	}
	
	//Libero la memoria nei processi della prima colonna della griglia	
	if (coords[1] == 0) {
		free(vettore);
		free(risultato_parziale_col0);
	}	
		
		
	//Libero la memoria in tutti i processi
	free(dimRecv_colonne);
	free(dimRecv_righe);
	free(displs_colonne);
	free(displs_righe);
	free(sotto_matrice_per_colonne_e_righe);
	free(data_sotto_matrice_per_colonne_e_righe);
	free(sotto_matrice_per_righe);
	free(data_sotto_matrice_per_righe);
	free(risultato_parziale);
	free(vettore_parziale);

	MPI_Type_free(&acol);
	MPI_Type_free(&acoltype);
	MPI_Type_free(&ccol);
	MPI_Type_free(&ccoltype);

	MPI_Finalize();
	return 0;
}




void printVett(double* vett, int dim, int myrank) {

	int i;

	printf("Sono il processo %d , stampo il vettore di %d elementi\n", myrank,dim);

	for (i = 0;i < dim;i++) {

		printf("%.16lf \n", vett[i]);

	}

	printf("\n");
}


void printMat(double** data, int r, int c, int rank) {

	int i = 0, j = 0;

	printf("Sono il processo %d e stampo matrice r=%d x c=%d: \n", rank, r, c);

	for (i = 0; i < r; i++) {

		printf("Processo %d, Riga %d : ", rank,i);

		for (j = 0; j < c; j++) {

			printf("%f  ", data[i][j]);
		}
		printf("\n");
	}
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



