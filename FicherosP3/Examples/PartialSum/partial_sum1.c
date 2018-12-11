#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

int total_sum = 0;
sem_t mutex;

typedef struct {
  int inicio;
  int fin;
} interval;

void * partial_sum(void* arg) {
  int j = 0;
  interval *v = (interval *) arg;
  int ni= v->inicio;
  int nf= v->fin;


  for (j = ni; j <= nf; j++) {
    sem_wait(&mutex);
    total_sum = total_sum + j;
    sem_post(&mutex); 
  }
  pthread_exit(0);
}


int main(int argc, char *argv[]) {
  
  int nthreads =  atoi (argv[1]);
  int n = atoi (argv[2]);
  int tam = n / nthreads;
  
  interval *index;
  pthread_t *t;
  
  sem_init(&mutex, 0, 1);
  index =(interval *) malloc(nthreads*sizeof(interval));
  t=(pthread_t *)malloc(nthreads * sizeof(pthread_t ));
  int i;
	for(i=0;i<nthreads;i++){
			index[i].inicio = i * tam;
		if (i == nthreads - 1) {
			index[i].fin = n;
		} else {
			index[i].fin = (i + 1) * tam - 1;
		}
		pthread_create(&t[i],NULL,partial_sum, &index[i]); //Default Attributes
	}
	
	for(i=0;i<nthreads;i++){
		pthread_join(t[i],NULL);
	}
 
  printf("total_sum=%d ", total_sum);
  sem_destroy(&mutex);
  return 0;
}
