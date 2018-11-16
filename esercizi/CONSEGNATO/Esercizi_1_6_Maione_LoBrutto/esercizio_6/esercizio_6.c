#include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
    int myrank, numprocs;
    int namelen;
	int i,j;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    const int dim = 10;
    float array[dim];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Get_processor_name(processor_name,&namelen);
	MPI_Status info;

	//Controlli di robustezza delegati al processo 0
    if (myrank==0){
    
    	if(numprocs!=5){
        	printf("Errore! Il numero dei processi deve essere pari a 5\n.");
        	MPI_Abort(MPI_COMM_WORLD, 1);
   		 } 	
	}

	printf("Ciao sono il processo %d e mi chiamo %s\n", myrank, processor_name);
	
	//Inizializzazione vettore	
	if (myrank==0){
    	//Creazione vettore da inviare in broadcast
    	printf("Processo %d. Invio in broadcast il vettore: ",myrank);
    	for(i=0; i<dim; i++){
    	  array[i] = ((float)769*i)/(float)796;
    	  printf("%.6f ",array[i]);
   		}
   		printf("\n");
  }

  MPI_Bcast(&array,dim,MPI_FLOAT,0,MPI_COMM_WORLD);
  
  //Stampa in ordine
  for (i=0; i<numprocs; i++){
    if (myrank==i){
      printf("Sono il processo %d. Ho ricevuto l'array : ",myrank);
      for(j=0; j<dim; j++){
        printf("%.6f ",array[j]);
		
      }
	  printf("\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
	
  MPI_Finalize();
  return 0;
}

            
