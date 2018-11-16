#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] )
{
    int myrank, numprocs;
    int namelen;
	int i,j;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    const int dim = 14;
    int *sendArray=NULL,*receiveArray=NULL;
	int *displs=NULL,*dimRecv=NULL;
	
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Get_processor_name(processor_name,&namelen);
	MPI_Status info;

	
	
	//Controlli di robustezza delegati al processo 0
    if (myrank==0){
    
    	if(numprocs!=4){
        	printf("Errore! Il numero dei processi deve essere pari a 4\n");
        	MPI_Abort(MPI_COMM_WORLD, 1);
   		 } 	
		 
	}

	printf("Ciao sono il processo %d e mi chiamo %s\n", myrank, processor_name);
	
	
	if (myrank==0){
    	
		//Inizializzazione vettore da distribuire	
		sendArray=(int*)malloc(dim*sizeof(int));
	
		for(i=0;i<dim;i++){
			sendArray[i]=i;
		}
		
	}
	
	dimRecv=(int*)malloc(numprocs*sizeof(int));
	displs=(int*)malloc(numprocs*sizeof(int));

	//Inizializzazione parametri per scatterv delegati al processo 0
	if (myrank == 0){
	

		for (i=0; i<numprocs; i++){
		
	 		if(i<(dim%numprocs)){
				//Devo ricevere i+1 elementi
		 		dimRecv[i]=(int) dim/numprocs+1;
			}
	 		else{
				 //Devo ricevere i elementi
		 		dimRecv[i]=(int) dim/numprocs;
			}
		}
	
		//Inizializzazione displs

		displs[0]=0;
		for (i=1; i<numprocs; i++){
		
	 		displs[i]=displs[i-1]+dimRecv[i-1];
		}
	}

	//Allocazione vettore in cui ricevere
	receiveArray=(int*)malloc(dimRecv[myrank]*sizeof(int));
	
	//Broadcast
	MPI_Bcast(dimRecv, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs, numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	
	
	MPI_Scatterv(sendArray, dimRecv, displs, MPI_INT, receiveArray, dimRecv[myrank], MPI_INT,0,MPI_COMM_WORLD);
	
    //Stampa in ordine
    for (i=0; i<numprocs; i++){
     if (myrank==i){
		
       printf("Sono il processo %d. Ho ricevuto l'array : \n",myrank);
       for(j=0; j<dimRecv[myrank]; j++){
         printf("receiveArray[%d] = %d \n",j,receiveArray[j]);
		
       }
	   printf("\n");
     }
     MPI_Barrier(MPI_COMM_WORLD);
	}
  
  
	//Libero la memoria in tutti i processi 
    free(receiveArray);
    receiveArray=NULL;
	free(dimRecv);
  	dimRecv=NULL;
  	free(displs);
  	displs=NULL;
  
    if (myrank==0){
  	   free(sendArray);
	   sendArray=NULL;

    }
  
    MPI_Finalize();
    return 0;
}
