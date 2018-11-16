#include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
    int myrank, numprocs;
    int namelen;
	int i;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Get_processor_name(processor_name,&namelen);

	//Controlli di robustezza delegati al processo 0
    if (myrank==0){
    
    	if(numprocs!=4){
     	   printf("Errore! Il numero dei processi deve essere pari a 4.");
    	   MPI_Abort(MPI_COMM_WORLD, 1);
   		}
    }
	
	for (i=0; i<numprocs;i++){
		
		//Stampa in ordine dei processi
		if(myrank==i){
			printf("Ciao sono il processo %d e mi chiamo %s\n", myrank, processor_name);
		}
		
		MPI_Barrier(MPI_COMM_WORLD);
	}
	

    MPI_Finalize();
    return 0;
}

            
