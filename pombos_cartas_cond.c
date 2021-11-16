// Aluno: Joao Victor Pinheiro de Souza
// 180103407

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

/*
  Criacao de dois locks, sendo um deles para os pombos e o outro para o usuario,
  criando duas variaves de condicoes para o pombo e tambem para o usuario,
  e por fim criando uma variavel global para monitoras o quantidades de cartas na mochila

*/

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pombo = PTHREAD_COND_INITIALIZER;
pthread_cond_t usuario = PTHREAD_COND_INITIALIZER;
int mochila = 0;

void *f_usuario(void *arg);
void *f_pombo(void *arg);

int main(int argc, char **argv)
{
    int i;

    pthread_t usuario[N];
    int *id;
    for (i = 0; i < N; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&(usuario[i]), NULL, f_usuario, (void *)(id));
    }
    pthread_t pombo;
    id = (int *)malloc(sizeof(int));
    *id = 0;
    pthread_create(&(pombo), NULL, f_pombo, (void *)(id));

    pthread_join(pombo, NULL);
}

void *f_pombo(void *arg)
{

    while (1)
    {
        // alocando um lock para ter exclusao
        pthread_mutex_lock(&lock);
        // verifica se o quantidades de cartas feitas eh menor do que numero total
        // caso seja coloca o pombo para dormi, esperando ter o numero total de cartas
        while (mochila < CARTAS)
        {
            printf("Pombo dormindo...\n");
            sleep(1);
            pthread_cond_wait(&pombo, &lock); // colocando o pombo para dormi
        }

        // quando eh liberado para viagem ele vai para B e depois volta
        printf("Pombo indo de A para B...\n");
        sleep(3);
        printf("Pombo voltando de B para A...\n");
        sleep(1);
        // zerando o quantidade de cartas na mochila, para inicializar o contagem
        mochila = 0;
        // liberando todos os usuarios
        pthread_cond_broadcast(&usuario);
        // libera o lock
        pthread_mutex_unlock(&lock);
    }
}

void *f_usuario(void *arg)
{
    int id = *(int *)arg; // id para numerar os usuarios
    while (1)
    {
        sleep(1);
        // entrando na exclusao, sendo diferente do pombo pelo fato que os escritores pode escrever
        // enquanto acontece a viagem, mas nao pode colocar na mochila durante esse acontecimento
        // colocando quem escreveu a carta para dormi.
        pthread_mutex_lock(&lock_1);
        // verifica se o numero de cartas feitas sao igual ao total
        // caso seja vai colocar para dormi os usuarios, esperando o pombo voltar
        // e liberar eles
        while (mochila == CARTAS)
        {
            printf("Usuario %d tentou escever durante a viagem entao foi dormi...\n", id);
            sleep(1);
            pthread_cond_wait(&usuario, &lock); // colocando para dormi
        }

        // adicioando o numero de cartas na mochila
        mochila++;
        printf("Usuario %d colocando uma carta na mochila, Total: %d\n", id, mochila);
        sleep(1);
        // caso o numero de cartas feitas eh igual ao numero total vai acordar o pombo
        if (mochila == CARTAS)
        {
            printf("usuario %d Acordando o pombo...\n", id);
            sleep(1);
            pthread_cond_signal(&pombo); // liberando o pombo
        }

        pthread_mutex_unlock(&lock_1);
    }
}
