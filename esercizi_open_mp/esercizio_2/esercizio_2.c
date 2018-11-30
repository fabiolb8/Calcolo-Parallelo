#include <stdio.h>
#include <omp.h>
#include <stdlib.h>


#define N 10


int main( int argc, char *argv[] )
{

	int i,tid;
	int a[N], b[N];
	int c[N], d[N];
	
	for (i=0; i<N; i++){
	
		a[i]=1;
		b[i]=2;
	
	}
	
	#pragma omp parallel shared(a,b,c,d) private (i, tid)
	{
		tid = omp_get_thread_num();
		printf("Ciao sono il thread %d di %d.\n",tid,omp_get_num_threads());
		
		#pragma omp sections
		{
		
			#pragma omp section
			{
				for (i=0; i<N; i++){
					printf("[Thread %d] Ciclo %d SU C\n", tid, i);
					c[i]=3*b[i];
				}
			}
		
			#pragma omp section
			{
				for (i=0; i<N; i++){
					printf("[Thread %d] Ciclo %d SU D\n", tid, i);
					d[i]=3*a[i];
				}
			}

		}
				
		printf("[Thread %d] TERMINATO\n", tid);

	} 	

	
	return EXIT_SUCCESS;
}
