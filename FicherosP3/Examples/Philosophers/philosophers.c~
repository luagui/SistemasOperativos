#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NR_PHILOSOPHERS 5

pthread_t philosophers[NR_PHILOSOPHERS];
pthread_mutex_t forks[NR_PHILOSOPHERS];


void init()
{
    int i;
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_mutex_init(&forks[i],NULL);

    
}

void think(int i) {
    printf("Philosopher %d thinking... \n" , i);
        sleep(random() % 10);
    printf("Philosopher %d stopped thinking!!! \n" , i);
}

void eat(int i) {
    printf("Philosopher %d eating... \n" , i);
     sleep(random() % 5);
    printf("Philosopher %d is not eating anymore!!! \n" , i);

}

void toSleep(int i) {
    printf("Philosopher %d sleeping... \n" , i);
    sleep(random() % 10);
    printf("Philosopher %d is awake!!! \n" , i);
    
}
void lock_fork(int i) {
    pthread_mutex_lock(&forks[i]);
}

void unlock_fork(int i) {
    pthread_mutex_unlock(&forks[i]);
}

void doble_lock_fork(int right, int left) {
    if (right > left) { //Si es menor el actual, se puede coger
	 lock_fork(left);
    } else {  //Si es mayor el de la izquierda, es decir, estamos en el primer filoso, debemos coger los "tenedores" al revés
    	unlock_fork(right);
	lock_fork(left);
	lock_fork(right);
    }
}

void* philosopher(void* i)
{
    int nPhilosopher = (int)i;
	//Sentido contrario a las agujas del reloj
    int right = nPhilosopher;
    int left = (nPhilosopher - 1 == -1) ? NR_PHILOSOPHERS - 1 : (nPhilosopher - 1);
    while(1)
    {
        
        think(nPhilosopher);
        
        /// TRY TO GRAB BOTH FORKS (right and left)
	lock_fork(right); //Coge su tenedor
	doble_lock_fork(right, left);

        eat(nPhilosopher);
       	
	
        // PUT FORKS BACK ON THE TABLE
	unlock_fork(left);
	unlock_fork(right);
        
        toSleep(nPhilosopher);
   }

}



int main()
{
    init();
    unsigned long i;
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_create(&philosophers[i], NULL, philosopher, (void*)i);
    
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_join(philosophers[i],NULL);
    return 0;
} 
