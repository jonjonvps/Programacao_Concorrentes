#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer


void * produtor(void * meuid);
void * consumidor (void * meuid);

int produto = 0; // variavel indicando o numero de produtos

sem_t empty; // criando um semaforo de vazio

sem_t full; // craindo um semaforo pear cheio

sem_t mutex; // criando um semaforo para o lock

void main(argc, argv)
int argc;
char *argv[];
{

  sem_init(&full,0,0); // iniciando o semaforo e o seus valores

  sem_init(&empty,0,N); // iniciando o semaforo e o seus valores
  
  sem_init(&mutex,0,1); // iniciando o semaforo e o seus valores
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[PR];
   
  for (i = 0; i < PR; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
 
  pthread_join(tid[0],NULL);

} 

void * produtor (void* pi)
{

  //int id = *((int *) pi); // criando um id para os 

  while(1)
  {

   sem_wait(&empty); // decrescenta o valor de empty

   sem_wait(&mutex); // entrando na regiao critica usando um semafora como lock

   produto++; // acrescenta no contador de produtos
   printf("produzindo, total: %d\n",produto);
   sleep(1);
   
   sem_post(&mutex); // saindo da regia critica


   sem_post(&full); // acrescentando no contador  de full

  }
  pthread_exit(0);
  
}

void * consumidor (void* pi)
{

  //int id = *((int *) pi);
  
  while(1)
  {
    
   sem_wait(&full); // decrescenta o valor de full
   
   sem_wait(&mutex); // entrando na regiao critica usando um semafora como lock
   
   produto--; // decrescenta no contador de produtos
   sleep(1);
   printf("consumindo, restando: %d\n",produto);
   
   sem_post(&mutex); // saindo da regia critica

   sem_post(&empty); // acrescentando no contador  de empty
  }
  pthread_exit(0);
  
}
