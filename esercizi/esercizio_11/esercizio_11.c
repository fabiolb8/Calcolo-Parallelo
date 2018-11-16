#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define numero_processori 4
#define m 1000
#define n 50000

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
	int *displs=NULL,*dimRecv=NULL;
	
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
	
	
	//Allocazione parametri per scatterv
	dimRecv=(int*)calloc(numprocs,sizeof(int));
	displs=(int*)calloc(numprocs,sizeof(int));

	
	//Inizializzazione parametri per scatterv delegati al processo 0
	if (myrank == 0){

		//Inizializzazione dimRecv
	
		for (i=0; i<numprocs; i++){
		
	 		if(i<(m%numprocs)){
				//Devo ricevere i+1 elementi
		 		dimRecv[i]=(int) (m/numprocs+1)*n;
			}
	 		else{
				 //Devo ricevere i elementi
		 		dimRecv[i]=(int) (m/numprocs)*n;
			}
		}
			
		
		//Inizializzazione displs

		displs[0]=0;
		for (i=1; i<numprocs; i++){
		
	 		displs[i]=displs[i-1]+dimRecv[i-1];
		}
	}
	
	//MPI_Barrier(MPI_COMM_WORLD);


	//Broadcast
	MPI_Bcast(dimRecv, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs, numprocs, MPI_INT, 0, MPI_COMM_WORLD);

	//Allocazione sotto_matrice in cui ricevere
	data_sotto_matrice = (float*) malloc (dimRecv[myrank]*sizeof(float));
	sotto_matrice = (float**) malloc (dimRecv[myrank]/n*sizeof(float*));
	for (i=0; i<dimRecv[myrank]/n; i++){
		sotto_matrice[i]=&(data_sotto_matrice[i*n]);
	}
	
	//Scatterv
	MPI_Scatterv(&matrice[0][0], dimRecv, displs, MPI_FLOAT, &sotto_matrice[0][0], dimRecv[myrank], MPI_FLOAT,0,MPI_COMM_WORLD);
	
    //Stampa in ordine
    for (i=0; i<numprocs; i++){
    
	 if (myrank==i){
     
    	printMat(sotto_matrice, dimRecv[i]/n, n, i);

     }
     
	 MPI_Barrier(MPI_COMM_WORLD);
	}
  	
  	
	//Libero la memoria per il processo root per la matrice principale
  	if (myrank==0){
		free(matrice);
    	matrice=NULL;
    	free(data_matrice);
    	data_matrice=NULL;
	}
	
	//Libero la memoria di tutti i processi 
	free(sotto_matrice);
  	sotto_matrice=NULL;
  	free(data_sotto_matrice);
  	data_sotto_matrice=NULL;
  	free(displs);
  	displs=NULL;
  	free(dimRecv);
  	dimRecv=NULL;
  
  
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
