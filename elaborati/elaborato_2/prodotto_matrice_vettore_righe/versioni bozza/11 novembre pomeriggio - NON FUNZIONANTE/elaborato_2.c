#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define m 1000
#define n 1000
#define dim_vett 1000

void printMat(double**, int, int, int);
void printVettF(double*, int, int);
void prodottoMatriceVettore(double*, int , int , double*, double*);

int main( int argc, char *argv[] )
{
    int myrank, numprocs;
    int namelen;
	int i,j;
	double k;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    double* data_matrice;
    double* data_sotto_matrice;
    double** matrice;
    double** sotto_matrice;
    double* vettore;
    double* risultato;
    double* risultato_parziale;
	int *displs,*dimRecv;
	int *displs_risultato,*dimRecv_risultato;
	double t1, t2;
	double *sotto_matrice_finta;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Get_processor_name(processor_name,&namelen);
	MPI_Status info;
	
	//Controlli di robustezza delegati al processo 0
    if (myrank==0){
    
    	if(numprocs<=0){
        	printf("Errore! Il numero dei processi deve essere positivo\n");
        	MPI_Abort(MPI_COMM_WORLD, 1);
   		} 	
   		 
   		if (m <= 0 || n <= 0 || dim_vett<=0) {
			printf("Errore! La dimensione non è positiva.\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		
		if (m < numprocs) {
			printf("Errore! La dimensione di righe non è coerente con il numero di processori.\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		
		if (n != dim_vett) {
			printf("Errore! Le dimensioni non sono coerenti\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		if ( (floor((double)m) != (double)m) || (floor((double)n) != (double)n) || (floor((double)dim_vett) != (double)dim_vett)) {
			printf("Errore! Le dimensioni della matrice e del vettore devono essere valori interi\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		
	}
    MPI_Barrier(MPI_COMM_WORLD);

	printf("Ciao sono il processo %d e mi chiamo %s\n", myrank, processor_name);
	matrice = (double**) malloc ((int) m*sizeof(double*));
	vettore = (double*) calloc ((int)n,sizeof(double));

	if (myrank==0){
    			
    	//Inizializzazione matrice
    	data_matrice = (double*) malloc ((int)m*(int)n*sizeof(double));
		for (i=0; i<m; i++){
		
			matrice[i]=&(data_matrice[i*n]);
		}
    			
		k=0.0;
		for (i=0; i<m; i++){
			for (j=0; j<n; j++){
	
				matrice[i][j]=1.0*i;
			}
		}
				
		//Inizializzazione vettore
		for (i=0; i<n; i++){
		
			vettore[i]=1.0;
		}
				
		//Inizializzazione risultato nel processo root
		risultato = (double*) malloc ((int)m*sizeof(double));

	}
	
	//Allocazione parametri per scatterv e gatherv
	dimRecv=(int*)calloc(numprocs,sizeof(int));
	displs=(int*)calloc(numprocs,sizeof(int));
	dimRecv_risultato=(int*)calloc(numprocs,sizeof(int));
	displs_risultato=(int*)calloc(numprocs,sizeof(int));

	//Inizializzazione parametri per scatterv  e gatherv delegati al processo 0
	if (myrank == 0){

		for (i=0; i<numprocs; i++){
		
	 		if(i<((int)m%numprocs)){
				//Devo ricevere i+1 elementi
		 		dimRecv[i]=(int) (m/numprocs+1)*n;
			}
	 		else{
				 //Devo ricevere i elementi
		 		dimRecv[i]=(int) (m/numprocs)*n;
			}
		}
			
		for (i=0; i<numprocs; i++){
		
	 		if(i<((int)m%numprocs)){
				//Devo ricevere i+1 elementi
		 		dimRecv_risultato[i]=(int) (m/numprocs+1);
			}
	 		else{
				 //Devo ricevere i elementi
		 		dimRecv_risultato[i]=(int) (m/numprocs);
			}
		}
			
		//Inizializzazione displs

		displs[0]=0;
		for (i=1; i<numprocs; i++){
		
	 		displs[i]=displs[i-1]+dimRecv[i-1];
		}
		
		displs[0]=0;
		for (i=1; i<numprocs; i++){
		
	 		displs_risultato[i]=displs_risultato[i-1]+dimRecv_risultato[i-1];
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);

	//Allocazione sottovettore risultato parziale nei processi
	risultato_parziale = (double*) calloc ((int)dimRecv[myrank]/n,sizeof(double));

	printf("ciao0");
	
	//Broadcast
	MPI_Bcast(dimRecv, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(dimRecv_risultato, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs_risultato, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(vettore, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	//Allocazione matrice in cui ricevere
	data_sotto_matrice = (double*) malloc (dimRecv[myrank]*sizeof(double));
	sotto_matrice = (double**) malloc (dimRecv[myrank]/n*sizeof(double*));
		
	for (i=0; i<dimRecv[myrank]/n; i++){
		
		sotto_matrice[i]=&(data_sotto_matrice[i*n]);
	}
	
	printf("ciao1");
	
	double *punt=&(matrice[0][0]);
	
	
	sotto_matrice_finta = (double*)malloc(dimRecv[myrank]*sizeof(double));
	
	//Scatterv
	//MPI_Scatterv(punt, dimRecv, displs, MPI_DOUBLE, &sotto_matrice[0][0], dimRecv[myrank], MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Scatterv(punt, dimRecv, displs, MPI_DOUBLE, sotto_matrice_finta, dimRecv[myrank], MPI_DOUBLE,0,MPI_COMM_WORLD);
	
	
	printf("ciao2");
	
	if (myrank == 0){
		//Inizio tempo
		t1 = MPI_Wtime();
	}
  	
  	//Prodotto matrice vettore
  	prodottoMatriceVettore(sotto_matrice_finta, dimRecv[myrank]/n, n, vettore, risultato_parziale);
	
  	
  	//Gatherv del risultato
  	MPI_Gatherv(risultato_parziale, dimRecv_risultato[myrank], MPI_DOUBLE, risultato, dimRecv_risultato, displs_risultato, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  	
	
	//Stampa risultato finale
	if (myrank == 0) {

		//Fine tempo
		t2 = MPI_Wtime();

  		printf("Sono il processo %d. Il vettore risultato di %d colonne è :\n", myrank, m);
		//printVettF(risultato, m, myrank);
		//printf("Vettore[0]= %.16lf ,  Vettore[0]= %.16lf \n" ,risultato[0],risultato[m-1]);
    	printf("Sono il processo %d: tempo di esecuzione totale di %.16lf secondi.\n", myrank, t2 - t1);
	}
	
	
	
	if (myrank == 0) {

		//Libero la memoria nel processo root
		free(matrice);
    	matrice=NULL;
    	free(data_matrice);
    	data_matrice=NULL;
    }
	
	
	//Libero memoria di tutti i processori
	free(vettore);
  	vettore=NULL;
  	free(risultato);
  	risultato=NULL;
  	free(risultato_parziale);
  	vettore=NULL;
	free(sotto_matrice);
  	sotto_matrice=NULL;
  	free(data_sotto_matrice);
  	data_sotto_matrice=NULL;
  	free(displs);
  	displs=NULL;
  	free(dimRecv);
  	dimRecv=NULL;
  	free(displs_risultato);
  	displs_risultato=NULL;
  	free(dimRecv_risultato);
  	dimRecv_risultato=NULL;
  
  
    MPI_Finalize();
    return 0;
}


void printMat(double** data, int r, int c, int rank){

	int i=0,j=0;
	
	printf("Sono il processo %d e stampo matrice r=%d x c=%d: \n", rank, r, c);
		
	for (i=0; i<r; i++){
	
		printf("Riga %d: ", i);
	
		for (j=0; j<c; j++){
		
			printf("%.16lf  ", data[i][j]);
		}	
		printf("\n");
	}
}


void printVettF(double* array, int c, int rank){

	int i;
	
	printf("Stampa array del processo %d: ", rank);

	for (i=0; i<c; i++){
		
		printf("%.16lf   ", array[i]);
	}	
 	printf("\n");
	
}


void printVettI(int* array, int c, int rank){

	int i;
	
	printf("Stampa array del processo %d: ", rank);

	for (i=0; i<c; i++){
		
		printf("%d  ", array[i]);
	}	
 	printf("\n");
	
}


void prodottoMatriceVettore(double* matrice, int r, int c, double* vettore, double* risultato){

	int i, j;
	double somma=0.0;

	for (i=0; i<r; i++){
	
		risultato[i] = 0.0;
		
		for (j=0; j<c; j++){

			risultato[i] = risultato[i] + matrice[i*c+j] * vettore[j];

		}
		
		//risultato[i] = somma;
	}

}

