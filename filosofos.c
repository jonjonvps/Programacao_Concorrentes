// estudo dirigido 8
// aluno : João Victor Pinheiro de Souza
// Matricula: 180103407

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define ESQ(id) (id)
#define DIR(id)	(id+1)%N
// os tres estrados que o filosofo passa, onde esta com fome
// pensando quando termina de comer, e a parte que ele esta comendo
#define FOME 1
#define PENSANDO 0
#define COMENDO 2 
//criando um lock para a exclusao mutua
pthread_mutex_t lock_1 = PTHREAD_MUTEX_INITIALIZER;
// um vetor para armazer os estados dos filosofos
int status[N];
sem_t s[N];
// um semaforo servendo como lock  
sem_t filo;

void *filosofos (void *arg);

void pega_talher (int n);
void devolve_talher (int n);

int main () {
	int i;
	int *id;
	//semaforo 
	for (i=0; i<N; i++) {
		sem_init(&s[i], 0, 1);
	}
	pthread_t r[N]; 
	sem_init(&filo,0,1); // inicializando o lock filo
	//criacao das threads de filosofos
    for (i = 0; i < N ; i++) {
		id = (int *) malloc(sizeof(int));
	    *id = i;
	    pthread_create(&r[i], NULL, filosofos, (void*) (id));
	}

    pthread_join(r[0],NULL);
	return 0;
}


void *filosofos (void *arg) {
	int n = *((int *) arg);
	while(1) {
		// parte do programa que fica chamando entre pegar os talheres e devolver os talheres.
		printf("Filosofo %d pensando ...\n", n);
		sleep(2);
		pega_talher(n);   
		devolve_talher(n);
	} 
}

void teste(int n){
	// funcao onde verifica se os vizinhos testao comendo, caso nao esteja
	// seta o atual filosofo para comendo
	if (status[n] == FOME && status[ESQ(n)] != COMENDO && status[DIR(n)] != COMENDO){
		status[n] == COMENDO;
		printf("\tFilosofo %d comendo ...\n", n);
		sleep(1);
		sem_post(&filo); // libera para o semaforo filo
	}
}

void pega_talher (int n) {   
	// alocando um lock para ter exclusao
	pthread_mutex_lock(&lock_1);
	// seta o filodofo atual para esta com fome
	status[n] = FOME;
	teste(n);
	pthread_mutex_unlock(&lock_1);
	// trava o semafora filo 
    sem_wait(&filo);
}

void devolve_talher (int n) {
	// alocando um lock para ter exclusao
	pthread_mutex_lock(&lock_1);
	// coloca o filofo atual como pensando indicando que ja terminou de comer
	status[n] = PENSANDO;
	printf("\tFilosofo %d acabou de comer ...\n", n);
	sleep(1);
	// libera o semaforo quando devolve os talheres.
	sem_post(&filo);
	pthread_mutex_unlock(&lock_1);
}

