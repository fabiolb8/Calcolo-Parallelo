#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

#define N 15

int main( int argc, char *argv[] )
{

	int i,tid;
	int a[N], b[N];
	int c[N], d[N];
	
	//INIZIALIZZAZIONE VETTORI A e B
	for (i=0; i<N; i++){
		a[i]=1;
		b[i]=2;
	}
	
	#pragma omp parallel shared(a,b,c,d) private (i, tid)
	{
		tid = omp_get_thread_num();
		
			#pragma omp sections
			//#pragma omp sections nowait
			{
				for (i=0; i<N; i++){
					c[i]=3*b[i];
					printf("[Thread %d] Ciclo %d SU C\n", tid, i);
				}
			}
		
			#pragma omp sections
			//#pragma omp sections nowait
			{
				for (i=0; i<N; i++){
					d[i]=3*a[i];
					printf("[Thread %d] Ciclo %d SU D\n", tid, i);
				}
			}

		printf("[Thread %d] TERMINATO\n", tid);
	} 	
	
	return 0;
}



