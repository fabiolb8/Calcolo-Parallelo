#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] )
{
    int myrank, numprocs;
    int namelen;
	int i,j;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    const int dim = 32;
    int *sendArray=NULL,*receiveArray=NULL;

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
		 
		 if(dim%numprocs!=0){
			 printf("Errore! La dimensione dell'array deve essere un multiplo del numero di processi \n");
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
	
	//Allocazione vettore in cui ricevere
	receiveArray=(int*)malloc((dim/numprocs)*sizeof(int));
	
	
	MPI_Scatter(sendArray,dim/numprocs,MPI_INT,receiveArray,dim/numprocs,MPI_INT,0,MPI_COMM_WORLD);
	

	if(myrank==0){
		
		//Libero la memoria nel processo 0
		free(sendArray);
		sendArray=NULL;
	}
	
   //Stampa in ordine
   for (i=0; i<numprocs; i++){
    if (myrank==i){
		
      printf("Sono il processo %d. Ho ricevuto l'array : \n",myrank);
      for(j=0; j<dim/numprocs; j++){
        printf("receiveArray[%d] = %d \n",j,receiveArray[j]);
		
      }
	  printf("\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  
  
  //Libero la memoria in tutti i processi 
  free(receiveArray);
  receiveArray=NULL;
	
  MPI_Finalize();
  return 0;
}

            
