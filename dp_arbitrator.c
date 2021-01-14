#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_t *philosopher;
pthread_mutex_t *chopstick;
pthread_mutex_t arbitrator;
int argu;
void *eat(void *arg) {
	int n = (int*)(arg);
    // take two chopsticks
	pthread_mutex_lock(&arbitrator);
    pthread_mutex_lock(&chopstick[n]);
    printf("Philosopher %d got chopstick %d\n", n, n);
	pthread_mutex_lock(&chopstick[(n+1)%argu]);
    printf("Philosopher %d got chopstick %d\n", n, (n+1)%argu);
	pthread_mutex_unlock(&arbitrator);
    printf ("Philosopher %d is eating\n",n);
    sleep(1);
    // set them back down
   	printf("Philosopher %d set down chopstick %d\n", n, (n+1)%argu);
	pthread_mutex_unlock(&chopstick[(n+1)%argu]);
    printf("Philosopher %d set down chopstick %d\n", n, n);	
	pthread_mutex_unlock(&chopstick[n]);
    return NULL;
}

int main(int argc, const char *argv[]) {
	if(argc<2)
	{
		argu = 5;
	}
	else 
	{
		argu =atoi(argv[1]);
	}
	philosopher = calloc((size_t)argu,(size_t)sizeof(pthread_t));
	chopstick = calloc((size_t)argu,(size_t)sizeof(pthread_mutex_t));
	
    for(int i = 0; i < argu; i += 1)
        pthread_mutex_init(&chopstick[i], NULL);
	pthread_mutex_init(&arbitrator,NULL);
    for(int i =0; i < argu; i += 1)
        pthread_create(&philosopher[i], NULL, eat, (void *)(size_t)i);
    for(int i=0; i < argu; i += 1)
        pthread_join(philosopher[i], NULL);
	
    for(int i=0; i < argu; i += 1)
        pthread_mutex_destroy(&chopstick[i]);
      
    pthread_mutex_destroy(&arbitrator);
	free(philosopher);
	free(chopstick);

    return 0;
}
