#include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
    int myrank, numprocs;
    int namelen;
	int i;
	int tag=10;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    const int dim = 10;
    int a[dim];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Get_processor_name(processor_name,&namelen);
	MPI_Status info;
    
    //Controlli di robustezza delegati al processo 0
    if (myrank==0){
    
    	if(numprocs!=2){
     	   printf("Errore! Il numero dei processi deve essere pari a 2.");
    	   MPI_Abort(MPI_COMM_WORLD, 1);
   		}
    }

	printf("Ciao sono il processo %d e mi chiamo %s\n", myrank, processor_name);
	
	if(myrank==0){
	
		//Inizializzo il vettore
		for (i=0; i<dim; i++)
			a[i]=i;
			
		MPI_Send(&a,dim,MPI_INT,1,tag,MPI_COMM_WORLD);
	
	}
	else{
	
		MPI_Recv(&a,dim,MPI_INT,0,tag,MPI_COMM_WORLD,&info);
	
		printf("Sono il processo %d e ho ricevuto\n",myrank);

		for (i=0; i<dim; i++){
			printf("a[%d] = %d\n",i, a[i]);
		}
	
	}
	
    MPI_Finalize();
    return 0;
}

            
