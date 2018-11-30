#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

#define num_threads_4	4
#define num_threads_2	2

int main( int argc, char *argv[] )
{

	if (num_threads_4>omp_get_num_threads() || num_threads_2>omp_get_num_threads()){
	
		printf("Numero di threads definiti nel pbs è minore di quello richiesto dal programma\n");
		exit(1);
	}

	#pragma omp parallel 
	{
		printf("Primo modo: Ciao sono il thread %d di %d.\n",omp_get_thread_num(),omp_get_num_threads());
	} 	

	printf("\n");

	#pragma omp parallel num_threads(num_threads_4)
	{
		printf("Secondo modo: Ciao sono il thread %d di %d.\n",omp_get_thread_num(),num_threads_4);
	}

	printf("\n");

	omp_set_num_threads(num_threads_2);
	#pragma omp parallel
	{
		printf("Terzo modo: Ciao sono il thread %d di %d.\n",omp_get_thread_num(),omp_get_num_threads());
	} 	

	return EXIT_SUCCESS;
}
