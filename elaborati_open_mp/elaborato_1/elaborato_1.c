#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <math.h>

#define a 0.0
#define b 1.0
#define num_intervalli 100000

double funzione(double);

int main( int argc, char *argv[] )
{

	double risultato = 0.0;
	double somma, t1, t2;
	int i;
	const double h = (double) (b-a)/num_intervalli;
	

	//CONTROLLI DI ROBUSTEZZA
	if (atoi(getenv("OMP_NUM_THREADS")) <= 0) {
		printf("Errore! Il numero dei threads deve essere maggiore di zero\n");
		exit(1);
	}

	if (floor((double)num_intervalli) != (double)num_intervalli) {
		printf("Errore! Il numero di intervalli deve essere un valore intero\n");
		exit(1);
	}

	if (num_intervalli <= 0) {	
		printf("Errore! Il numero di intervalli non è un numero positivo\n");
		exit(1);
	}

	if (b < a) {
		printf("Errore! L'intervallo non è coerente.\n");
		exit(1);
	}

	if (num_intervalli < atoi(getenv("OMP_NUM_THREADS"))) {
		printf("Errore! La dimensione non è coerente con il numero di threads.\n");
		exit(1);
	}
	
	printf("[Thread %d] Il programma sarà eseguito con %d threads\n", omp_get_thread_num(), atoi(getenv("OMP_NUM_THREADS")));
	
	//INIZIO TEMPI
	t1 = omp_get_wtime();
	
	#pragma omp parallel for private(i) reduction(+:somma) 
	for (i=1; i<(int)num_intervalli; i++){
		somma += funzione(a+i*h);
	}
	
	
	risultato = (h / 2)*(funzione(a)+funzione(b) + 2 * somma);
	
	//FINE TEMPI
	t2 = omp_get_wtime();
	
	//STAMPA RISULTATO
	printf("[Thread %d] L'integrale è pari a %.16lf.\n",omp_get_thread_num(),risultato);
	printf("Il tempo di esecuzione totale è stato di %.16lf secondi.\n", t2-t1);
	
	return 0;
}

//Funzione da integrare
double funzione(double x) {

  double y=0.0; 
  
  //funzione usata per l'esempio di funzionamento
  y= x / (pow(x, 2) + 5);
  
  //funzione usata per l'esempio di accuratezza
  //y = 1 + sin(10*x) * (x / (pow(x, 2) + 5));

  return y;
}
