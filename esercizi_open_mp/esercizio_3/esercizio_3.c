#include <stdio.h>
#include <omp.h>
#include <stdlib.h>


#define N 5


int main( int argc, char *argv[] )
{

	int i,j,tid;
	int k=0;
	int a[N][N]; 
	int b[N][N];
	
	
	//CONTROLLO MATRICE QUADRATA
	
	for (i=0; i<N; i++){
		for (j=0; j<N; j++){

			a[i][j]=k++;
		}
	}
	
	for (i=0; i<N; i++){
		for (j=0; j<N; j++)
			printf("%d ", a[i][j]);
		printf("\n");
	}
	
	#pragma omp parallel shared(a,b) private (i, j, tid)
	{
		tid = omp_get_thread_num();
		printf("Ciao sono il thread %d di %d.\n",tid,omp_get_num_threads());
		
		
		#pragma omp for 
		for (i=0; i<N; i++)
			for (j=0; j<N; j++){
				printf("[Thread %d] %d %d.\n",tid,i,j);
				b[i][j] = a[j][i];
		}
				
		printf("[Thread %d] TERMINATO\n", tid);

	} 	
	
	for (i=0; i<N; i++){
		for (j=0; j<N; j++)
			printf("%d ", b[i][j]);
		printf("\n");
	}

	
	return EXIT_SUCCESS;
}

