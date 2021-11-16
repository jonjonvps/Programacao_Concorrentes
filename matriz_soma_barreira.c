/*
    Aluno: João Victor Pinheiro de Souza
    Matricula: 180103407
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXSIZE 10000 /* maximum matrix size */

pthread_barrier_t barrier;
int size;

int matrix[MAXSIZE][MAXSIZE]; // matrix onde os numeros serao armazenados
int vetor[MAXSIZE];           // vetor que vai receber a soma das linhas da matriz
int resultado = 0;            // variavel do resultado final

void *Worker(void *);

int main(int argc, char *argv[])
{
    int i, j;

    size = atoi(argv[1]);

    if (size > MAXSIZE)
    {
        printf("Tamanho muito grande!\n");
        return 0;
    }

    pthread_t workerid[size];

    pthread_barrier_init(&barrier, NULL, size);
    // alocando todos os numeros da matriz como 1
    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            matrix[i][j] = 1;

    // inicializando o vetor como 0
    for (i = 0; i < size; i++)
        vetor[i] = 0;

    int *id;
    for (i = 0; i < size; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&workerid[i], NULL, Worker, (void *)(id));
    }

    for (i = 0; i < size; i++)
    {
        if (pthread_join(workerid[i], NULL))
        {
            printf("\n ERROR joining thread");
            exit(1);
        }
    }
    printf("Bye!\n");
}

void *Worker(void *arg)
{
    int myid = *(int *)(arg);
    int j, k;

    int self = pthread_self();

    printf("worker %d (pthread id %d) has started\n", myid, self);
    // cada thread soma uma linha e armazena no vetor
    for (j = 0; j < size; j++)
    {
        vetor[myid] += matrix[myid][j];
    }
    // barreira quando cada thread termina a soma
    pthread_barrier_wait(&barrier);
    // soma todos os valores do vetor e coloca na variavel resultado
    if (myid == 0)
    {
        printf("\n ");
        for (j = 0; j < size; j++)
        {
            resultado += vetor[j];
        }
        printf("A soma dos elementos eh: %d ", resultado);
    }
}
