#include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
    int a, b, numprocs;
    int namelen;
	int i;
	int tag[2]={0,1};
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&a);
    MPI_Get_processor_name(processor_name,&namelen);
	MPI_Status info;

	//Controlli di robustezza delegati al processo 0
    if (a==0){
    
    	if(numprocs!=2){
     	   printf("Errore! Il numero dei processi deve essere pari a 2.");
    	   MPI_Abort(MPI_COMM_WORLD, 1);
   		}
    }

	printf("Ciao sono il processo %d e mi chiamo %s\n", a, processor_name);
	
	if(a==0){
	
		//Invio il mio rank e aspetto il rank dell'altro processo
		MPI_Send(&a,1,MPI_INT,1,tag[0],MPI_COMM_WORLD);
		MPI_Recv(&b,1,MPI_INT,1,tag[1],MPI_COMM_WORLD,&info);
	
		//Si può anche fare in questo altro modo
		//MPI_Sendrecv(&a,1,MPI_INT,1,tag[0],&b,1,MPI_INT,0,tag[1],MPI_COMM_WORLD,&info);
	
	}
	else{
		
		//Ricevo il rank e invio il rank all'altro processo
		MPI_Recv(&b,1,MPI_INT,0,tag[0],MPI_COMM_WORLD,&info);
		MPI_Send(&a,1,MPI_INT,0,tag[1],MPI_COMM_WORLD);
		
		//Si può anche fare in questo altro modo
		//MPI_Sendrecv(&b,1,MPI_INT,1,tag[1],&a,1,MPI_INT,0,tag[0],MPI_COMM_WORLD,&info);

	
	}
	
	printf("Sono il processo %d e ho ricevuto rank = %d\n",a,b);
	
    MPI_Finalize();
    return 0;
}

            
