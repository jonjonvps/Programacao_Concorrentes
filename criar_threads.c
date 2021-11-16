#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "unistd.h"

#define N 10

int contador = 0;

void * pthread_func(void * arg){
    int id = *((int *) arg); 
    for(int i = 0; i < 1000; i++){
        contador += 1;
    }
    printf("Criou um pthread com id = %d \n",id);
     pthread_exit(0);    
}

int main() {
   pthread_t a[N];
   printf("Meu id: %lu\n",pthread_self());

   int i;
   int * id;
   for (i = 0; i < N ; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&a[i], NULL, pthread_func, (void *) (id));
   }
  for (i = 0; i < N ; i++) {
      pthread_join(a[i],NULL);
  }

  printf("Contador: %d \n",contador);
  printf("TERMINANDO\n");
  return 0;
}

















