// Aluno: Joao Victor Pinheiro de Souza
// Matricula: 180103407
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cozi = PTHREAD_COND_INITIALIZER;
pthread_cond_t canib = PTHREAD_COND_INITIALIZER;

int N, cont = 0; // declarando variaves para utilizar no programa

void *canibal(void*meuid);
void *cozinheiro(int m);


void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if(argc != 3){
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }
  
  n = atoi (argv[1]); //número de canibais
  m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
  N = m;              // coloc na variavel "N" o numero de porcoes que o cozinhero consegue fazer 
  printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

  if(n > MAXCANIBAIS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }
  
  for (i = 0; i < n; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cozinheiro(m);
}

void * canibal (void* pi){
  
  while(1) { 
    sleep(2);
    // Fechando para  para que na hora de analisar a condicao nao altere o valor
    pthread_mutex_lock(&lock);
      // enquanto o contador ser igual a 0 os canibais vao continuar dormindo
      while(cont == 0){
        printf("Canibais dormindo\n");
        pthread_cond_wait(&canib, &lock);
      }

    pthread_mutex_unlock(&lock);
    
    
    // Fechando para  para que na hora de analisar a condicao nao altere o valor
    pthread_mutex_lock(&lock);
      // decrescenta o contador para cada canibal que comer,
      // e verifica se eh iagual a 0, caso seja acorda o cozinheiro para fazer mais
      printf("%d: vou comer a porcao que peguei\n", *(int *)(pi));
      cont--;
      printf("Numero de porcoes: %d\n",cont);
      if(cont == 0){
        printf("Acordando o cozinheiro\n");
        pthread_cond_signal(&cozi);
      }

    pthread_mutex_unlock(&lock);
    
  }
  
}

void *cozinheiro (int m){
 
  while(1){
    sleep(5);
    // Fechando para  para que na hora de analisar a condicao nao altere o valor
    pthread_mutex_lock(&lock);
      // Verifica se o contador eh igual ao numero de porcoes maximas,
      // cado seja coloca pra dormi a conzinheiro
      if(cont == N){
        printf("Conzinheiro dormindo\n");
        pthread_cond_wait(&cozi, &lock);
      }

    pthread_mutex_unlock(&lock);
    
    // Fechando para  para que na hora de analisar a condicao nao altere o valor
    pthread_mutex_lock(&lock);
      // coloca o contador ser igual ao numero de porcoes maximas, em seguida acorda todos os canibais para comer
      printf("cozinheiro: vou cozinhar\n");
      cont = N;
      printf("%d porcoes feitas\n",cont);
      printf("Acordando os canibais\n");
      pthread_cond_broadcast(&canib);

    pthread_mutex_unlock(&lock);
   }

}
