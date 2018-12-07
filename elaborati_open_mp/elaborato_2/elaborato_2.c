#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <math.h>

#define m 7
#define n 8
#define dim_vett 8

void printVett(double*, int);

int main( int argc, char *argv[] )
{

	int i,j;
	double **matrice;
	double *data_matrice, *risultato, *vettore;
	double t1, t2;
	double temp = 0.0;

	//CONTROLLI DI ROBUSTEZZA
	if (atoi(getenv("OMP_NUM_THREADS")) <= 0) {
		printf("Errore! Il numero dei threads deve essere positivo\n");
		exit(1);
	}

	if ((floor((double)m) != (double)m) || (floor((double)n) != (double)n) || (floor((double)dim_vett) != (double)dim_vett)) {
		printf("Errore! Le dimensioni della matrice e del vettore devono essere valori interi\n");
		exit(1);
	}


	if (m <= 0 || n <= 0 || dim_vett <= 0) {
		printf("Errore! Le dimensioni dei dati di input non sono valori positivi.\n");
		exit(1);
	}

	if (n != dim_vett) {
		printf("Errore! Le dimensioni non sono coerenti\n");
		exit(1);
	}
	
	printf("[Thread %d] Il programma sarà eseguito con %d threads\n", omp_get_thread_num(), atoi(getenv("OMP_NUM_THREADS")));
	
	
	//MATRICE E VETTORE
	matrice = (double**)malloc((int)m * sizeof(double*));
	vettore = (double*)malloc((int)n * sizeof(double));
	//Vettore RISULTATO 
	risultato = (double*)malloc((int)m * sizeof(double));


	//Inizializzazione MATRICE
	data_matrice = (double*)malloc((int)m*(int)n * sizeof(double));
	for (i = 0; i < m; i++) {

		matrice[i] = &(data_matrice[i*n]);
	}

	printf("[Thread %d] Matrice :\n", omp_get_thread_num());
	
	for (i = 0; i < m; i++) {
	
		for (j = 0; j < n; j++) {

			matrice[i][j] = (double)i + 1.0;
			printf("%lf ", matrice[i][j]);
		}
		
		printf("\n");
	}
	
	printf("\n");
	printf("[Thread %d] Vettore :\n", omp_get_thread_num());


	//Inizializzazione VETTORE
	for (i = 0; i < n; i++) {

		vettore[i] = M_PI;
	}
	
	printVett(vettore,n);
	printf("\n");
	
	//INIZIO TEMPI
	t1 = omp_get_wtime();
	
	
	//CALCOLO IN PARALLELO
	#pragma omp parallel for firstprivate(temp) private(i,j) shared (matrice, vettore, risultato)
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			temp += matrice[i][j] * vettore[j];
		}
		risultato[i] = temp;
		temp=0.0;
	}
	
	
	//FINE TEMPI
	t2 = omp_get_wtime();
	
	
	//STAMPA RISULTATO
	printf("[Thread %d] Il vettore risultato è pari a \n",omp_get_thread_num());
	printVett(risultato, m);
	printf("Il tempo di esecuzione totale è stato di %.16lf secondi.\n", t2-t1);
	
	
	//Libero la memoria
	free(matrice);
	free(data_matrice);
	free(vettore);
	free(risultato);
	
	return 0;
}


void printVett(double* array, int r) {

	int i;

	for (i = 0; i < r; i++) {

		printf("\n%.16lf ", array[i]);
	}
	printf("\n");

}
