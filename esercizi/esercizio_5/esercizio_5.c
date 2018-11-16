#include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
    int myrank, numprocs;
    int namelen;
	int i,j,tag=10;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    const int dim = 10;
    const int num_volte = 3;
    int vector[dim];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Get_processor_name(processor_name,&namelen);
	MPI_Status info;

	//Controlli di robustezza delegati al processo 0
    if (myrank==0){
    
    	if(numprocs!=3){
        	printf("Errore! Il numero dei processi deve essere pari a 3.");
        	MPI_Abort(MPI_COMM_WORLD, 1);
   		 } 	
	}

	printf("Ciao sono il processo %d e mi chiamo %s\n", myrank, processor_name);
	
	//Inizializzazione vettore	
	for (i=0; i<dim; i++)
			vector[i]=myrank;
			
	for (i=0; i<numprocs;i++){
	
			//Stampa in ordine dei processi
			if(myrank==i){	
				printf("Sono il processo %d e ho inizialmente il vettore\n",myrank);
				for (j=0; j<dim; j++)
					printf("vector[%d] = %d\n",j,vector[j]);
			}
		
			MPI_Barrier(MPI_COMM_WORLD);
		}
	
	
		//Invio a myrank+1 e ricevo da myrank-1, in modulo numprocs
		MPI_Sendrecv(&vector,dim,MPI_INT,(myrank+1)%numprocs,tag,&vector,dim,MPI_INT,(myrank-1)%numprocs,tag,MPI_COMM_WORLD,&info);
		
				
		for (i=0; i<numprocs;i++){
	
			//Stampa in ordine dei processi
			if(myrank==i){	
				printf("Sono il processo %d e ho il vettore\n",myrank);
				for (j=0; j<dim; j++)
					printf("vector[%d] = %d\n",j,vector[j]);
			}
		
			MPI_Barrier(MPI_COMM_WORLD);
		}
	
	
    MPI_Finalize();
    return 0;
}

            
