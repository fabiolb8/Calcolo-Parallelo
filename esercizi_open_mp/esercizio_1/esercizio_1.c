#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

#define num_threads_4	4
#define num_threads_2	2

int main( int argc, char *argv[] )
{


	if(atoi(getenv("OMP_NUM_THREADS")) < num_threads_4 || atoi(getenv("OMP_NUM_THREADS")) < num_threads_2){
		
		printf("Numero di threads definiti in OMP_NUM_THREADS è minore di quello richiesto dal programma\n");
		exit(1);
		
	}

	//PRIMO MODO (TRAMITE IL PBS)
	#pragma omp parallel 
	{
		printf("Primo modo: Ciao sono il thread %d di %d.\n",omp_get_thread_num(),omp_get_num_threads());
	} 	

	printf("\n");

	
	//SECONDO MODO (TRAMITE LA CLAUSOLA)
	#pragma omp parallel num_threads(num_threads_4)
	{
		printf("Secondo modo: Ciao sono il thread %d di %d.\n",omp_get_thread_num(),omp_get_num_threads());
	}

	printf("\n");

	//TERZO MODO (TRAMITE LA FUNZIONE SET)
	omp_set_num_threads(num_threads_2);
	#pragma omp parallel
	{
		printf("Terzo modo: Ciao sono il thread %d di %d.\n",omp_get_thread_num(),omp_get_num_threads());
	} 	

	return 0;
}
