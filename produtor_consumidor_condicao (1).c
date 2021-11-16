// Aluno: Joao Victor Pinheiro de Souza
// Matricula: 18010307

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t prod = PTHREAD_COND_INITIALIZER;
pthread_cond_t cons = PTHREAD_COND_INITIALIZER;

void * produtor(void * meuid);
void * consumidor (void * meuid);

int cont = 0;

void main(argc, argv)
int argc;
char *argv[];
{

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
  while(1){

    sleep(5);
    // Fechando para  para que na hora de analisar a condicao nao altere o valor
    pthread_mutex_lock(&lock);             
      if(cont == N){                       // caso o contador seja igual ao tamanho do buffer ele  
        printf("Produto dormindo\n");      // colocar para dormi o produtor
        pthread_cond_wait(&prod, &lock);
      }

    pthread_mutex_unlock(&lock);

    printf("Esta produzindo\n");
    sleep(1);

     // Fechando para  para que na hora de analisar a condicao nao altere o valor
    pthread_mutex_lock(&lock);
      // adiciona mais um no contador, e caso ele seja igual a 1 acorda o consumidor
      cont++;
      printf("O produtor colocou o item no buffer. Buffer = %d\n", cont);
      if(cont == 1){
        printf("Acordando o consumidor\n");
        pthread_cond_signal(&cons);
      }

    pthread_mutex_unlock(&lock);

  }
  pthread_exit(0);
  
}

void * consumidor (void* pi)
{
  while(1){
    sleep(1);
     // Fechando para  para que na hora de analisar a condicao nao altere o valor
    pthread_mutex_lock(&lock);
      // verifica se o contador eh igual a 0 colocar pra dormi o consumidor
      if(cont == 0){
        printf("consumo dormindo\n");
        pthread_cond_wait(&cons, &lock);
      }

    pthread_mutex_unlock(&lock);

    printf("Esta consumindo\n");
    sleep(1);

     // Fechando para  para que na hora de analisar a condicao nao altere o valor
    pthread_mutex_lock(&lock);
      // decrescenta o contador, e caso ele seja um a menos que o tamanho do buffer acorda o produtor
      cont--;
      printf("O consumidor esta consumindo o item. Buffer = %d\n", cont);

      if(cont == N - 1){
        printf("Acordando o produtor\n");
        pthread_cond_signal(&prod);
      }

    pthread_mutex_unlock(&lock);
  }
  pthread_exit(0);
  
}
