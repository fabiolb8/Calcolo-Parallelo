#include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
    int myrank, numprocs;
    int namelen;
	int i,j;
	int tag=10;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    const int dim = 10;
    int vector[dim];
    const int start_send_buf = 3;
    const int start_recv_buf = 5;
    const int lenght = 4;

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
    
    	if ((start_send_buf<0 && start_send_buf>dim) || (start_recv_buf<0 && start_recv_buf>dim)){
    		printf("Errore! Voglio accedere in posizione dell'array non consentite.");
       		MPI_Abort(MPI_COMM_WORLD, 1);
   		}
    
    	if (lenght<=0){
    		printf("Errore! Il numero di elementi da inviare è non positivo.");
       		MPI_Abort(MPI_COMM_WORLD, 1);
   		}
    
    	if ((start_send_buf+lenght>dim) || (start_recv_buf+lenght>dim)){
    		printf("Errore! Voglio usare porzioni dell'array non consentite.");
       	 	MPI_Abort(MPI_COMM_WORLD, 1);
    	}
    	
	}


	printf("Ciao sono il processo %d e mi chiamo %s\n", myrank, processor_name);

	
	//Inizializzazione vettore
	if (myrank == 0){
	
		for (i=0; i<dim; i++)
			vector[i]=i;
	
		//Invio
		MPI_Send(&vector[start_send_buf],lenght,MPI_INT,1,tag,MPI_COMM_WORLD);

	}
	
	else{
	
		for (i=0; i<dim; i++)
			vector[i]=i+10;
			
		//Ricevi
		MPI_Recv(&vector[start_recv_buf],lenght,MPI_INT,0,tag,MPI_COMM_WORLD,&info);

	}	
		
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

            
