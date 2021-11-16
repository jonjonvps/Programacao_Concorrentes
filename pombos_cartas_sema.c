// Aluno: Joao Victor Pinheiro de Souza
// 180103407

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila
/*
 craindo um lock e dois semaforos sendo um deles para pombo e outro para mochila dele
*/
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t pombo;
sem_t mochila_pombo;
int mochila = 0;

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;
    // inicializando os semaforos, para o pombo com 0 e para a mochila o numero
    // total de cartas que suporta a mochila
    sem_init(&pombo,0,0);
    sem_init(&mochila_pombo,0,CARTAS);
    pthread_t usuario[N];
    int *id;
    for(i = 0; i < N; i++){
         id = (int *) malloc(sizeof(int));
          *id = i;
	pthread_create(&(usuario[i]),NULL,f_usuario,  (void *) (id));
    }
     pthread_t pombo;
     id = (int *) malloc(sizeof(int));
     *id = 0;
     pthread_create(&(pombo),NULL,f_pombo, (void*) (id));

     pthread_join(pombo,NULL);
   
}


void * f_pombo(void *arg){
  
    while(1){
        // colocando o pombo para dormi, esperando ate que seja acordado
        sem_wait(&pombo);

        // realizando a viagem
        printf("Pombo indo de A para B...\n");
        sleep(3);
        printf("Pombo voltando de B para A...\n");
        sleep(1); 
        // entrando na exclusao pois vai mecher com informacoes criticas 
        pthread_mutex_lock(&lock);
            // zerando o numero de cartas, para inicializar a contagem
            mochila = 0;
            // liberando o semaforo da mochila do pombo
            for(int i = 0; i < CARTAS; i++){
                sem_post(&mochila_pombo);
            }
        pthread_mutex_unlock(&lock); 
    }
}
 
void * f_usuario(void *arg){
    int id = *(int*) arg;
    while(1){
        sleep(1);
        // entrando cada usuarios para colocar na mochila as cartas
        if(sem_trywait(&mochila_pombo) == 0)
        {
            // entrando na exclusao
            pthread_mutex_lock(&lock);
            // adicionando as cartas na mochila
            mochila++;
            printf("Usuario %d colocando uma carta na mochila, Total: %d\n",id, mochila);
            sleep(1);
            // caso as cartas do mochila eh igual ao total libera o pombo para a viagem
            if(mochila == CARTAS){
                printf("usuario %d Acordando o pombo...\n",id);
                sleep(1);
                sem_post(&pombo);
            }
            pthread_mutex_unlock(&lock);
        }
    }
}
