#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define numero_processori 4
#define dim 8
#define n dim
#define m dim

//Funzione per la stampa di una matrice
void printMat(float**, int, int, int);



int main( int argc, char *argv[] )
{
    int myrank, numprocs;
    int namelen;
	int i,j;
	float k;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
	float* data_matrice = NULL;
    float* data_sotto_matrice = NULL;
    float** matrice = NULL;
    float** sotto_matrice = NULL;
	
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Get_processor_name(processor_name,&namelen);
	MPI_Status info;
	
	//Controlli di robustezza delegati al processo 0
    if (myrank==0){
    
    	if(numprocs!=numero_processori){
        	printf("Errore! Il numero dei processi deve essere pari a %d\n", numero_processori);
        	MPI_Abort(MPI_COMM_WORLD, 1);
   		 } 	
   		 
   		 
   		//Controllo m multiplo di numprocs
		if (m % numprocs != 0){
			printf("Errore! Il numero di righe non è multiplo del numero di processori\n");
        	MPI_Abort(MPI_COMM_WORLD, 1);
   		 } 	

	}
    MPI_Barrier(MPI_COMM_WORLD);


	printf("Ciao sono il processo %d e mi chiamo %s\n", myrank, processor_name);
	matrice = (float**) malloc (m*sizeof(float*));
	
	if (myrank==0){
    	
    	data_matrice = (float*) malloc (m*n*sizeof(float));
		for (i=0; i<m; i++){
			matrice[i]=&(data_matrice[i*n]);
		}
    			
				
		//Inizializzazione matrice principale nel processo root
		k=0.0;
		for (i=0; i<m; i++){
			for (j=0; j<n; j++){
				matrice[i][j]=k++;		
			}
		}
		
		printMat(matrice, m, n, myrank);
	}
	
	//Allocazione sotto_matrice in cui ricevere
	data_sotto_matrice = (float*) malloc (m/numprocs*n*sizeof(float));
	sotto_matrice = (float**) malloc (m/numprocs*sizeof(float*));
	for (i=0; i<m/numprocs; i++){
		sotto_matrice[i]=&(data_sotto_matrice[i*n]);
	}
	
	//Scatter
	MPI_Scatter(&matrice[0][0], m/numprocs*n, MPI_FLOAT, &sotto_matrice[0][0], m/numprocs*n, MPI_FLOAT, 0, MPI_COMM_WORLD);
	
	if (myrank==0){
		free(matrice);
    	matrice=NULL;
    	free(data_matrice);
    	data_matrice=NULL;
	}
	
	
	//Stampa in ordine
    for (i=0; i<numprocs; i++){
     if (myrank==i){
		
    	printMat(sotto_matrice, m/numprocs, n, i);
     }
     MPI_Barrier(MPI_COMM_WORLD);
	}
	
  	
	//Libero la memoria di tutti i processi 
	free(sotto_matrice);
  	sotto_matrice=NULL;
  	free(data_sotto_matrice);
  	data_sotto_matrice=NULL;
  
    MPI_Finalize();
    return 0;
}



//Funzione per la stampa di una matrice
void printMat(float** data, int r, int c, int rank){

	int i=0,j=0;
	
	printf("Sono il processo %d e stampo matrice r=%d x c=%d: \n", rank, r, c);
		
	for (i=0; i<r; i++){
	
		printf("Riga %d: ", i);
	
		for (j=0; j<c; j++){
		
			printf("%f  ", data[i][j]);
		}	
		printf("\n");
	}
}

