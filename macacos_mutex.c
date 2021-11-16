// Nome: Joao Victor Pinheiro de Souza
// Matricula: 180103407


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

pthread_mutex_t lock_macAB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_macBA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_vezAB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_vezBA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER;

int mac_ab = 0;
int mac_ba = 0;


void * macacoAB(void * a) {
  int i = *((int *) a);    
  while(1){
    // A implementacao da resolucao foi baseado no estudo dirigido 2

    if(mac_ba > 0){                     // bloqueia os macacos de A->B caso tenha macacos 
      pthread_mutex_lock(&lock_vezBA);  // do outro lado querendo cruzar.
    }

    pthread_mutex_lock(&lock_macAB);    // bloqueia o contador para nao acrescentar a mais
      mac_ab++;                         // fazendo ele nao entrar na condicao.
      if(mac_ab == 1){                  // bloqueia a corda quando o primeiro macaco entra na corda
        pthread_mutex_lock(&lock_corda);
      }
    pthread_mutex_unlock(&lock_macAB);
    
    //Procedimentos para acessar a corda
	  printf("Macaco %d passado de A para B \n",i);
	  sleep(10);
   
    pthread_mutex_lock(&lock_macAB);         // libera o contador para diminuir o numero de macacos
      mac_ab--;                              
      if(mac_ab==0){                         // ele libera quando o ultimo macaco chegar no outro lado
        pthread_mutex_unlock(&lock_corda);
      }
    pthread_mutex_unlock(&lock_macAB);
	  //Procedimentos para quando sair da corda
    pthread_mutex_unlock(&lock_vezAB);       // libera para que os macacos do outro lado possam sair
  }
  pthread_exit(0);
}

void * macacoBA(void * a) {
  // foi a mesma logica utilizada no 'macacoBA' invertendo de macaco AB para BA

  int i = *((int *) a);    
  while(1){
    if(mac_ab > 0){
      pthread_mutex_lock(&lock_vezAB);
    }

    pthread_mutex_lock(&lock_macBA);
      mac_ba++;
      if(mac_ba == 1){
        pthread_mutex_lock(&lock_corda);
      }
    pthread_mutex_unlock(&lock_macBA);
   
    //Procedimentos para acessar a corda
	  printf("Macaco %d passado de B para A \n",i);
	  sleep(10);
	  //Procedimentos para quando sair da corda
    
    pthread_mutex_lock(&lock_macBA);
      mac_ba--;
      if(mac_ba==0){
        pthread_mutex_unlock(&lock_corda);
      }
    pthread_mutex_unlock(&lock_macBA);
	  //Procedimentos para quando sair da corda
    pthread_mutex_unlock(&lock_vezBA);
  }
  pthread_exit(0);
}



void * gorila(void * a){
  while(1){
	  //Procedimentos para acessar a corda
	  printf("Gorila passado de A para B \n");
	  sleep(5);
    //Procedimentos para quando sair da corda
  }
  pthread_exit(0);
}

int main(int argc, char * argv[])
{
  pthread_t macacos[MA+MB];
  int *id;
  int i = 0;

  for(i = 0; i < MA+MB; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    if(i%2 == 0){
    if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
      printf("Não pode criar a thread %d\n", i);
      return -1;
    }
    }else{
      if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
          printf("Não pode criar a thread %d\n", i);
          return -1;
      }
    }
  }
  pthread_t g;
  pthread_create(&g, NULL, &gorila, NULL);

  pthread_join(macacos[0], NULL);
  return 0;
}
