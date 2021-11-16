#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

void *veterinario(void *arg);
void *cliente(void *arg);
void *emergencia(void *arg);

#define NC 25
#define NE 1
#define NV 4
#define CAPACIDADE_MAX 15
#define ATENDIMENTO_DIARIO 20

//sem_t sem_cadeiras;
sem_t cli;
sem_t vet;
sem_t sem_vaga;

pthread_barrier_t barrier;

pthread_mutex_t lock_fila = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_fila1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_fila2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cliente_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t emergencia_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t veterinario_cond = PTHREAD_COND_INITIALIZER;

int capMaxima = 0;
int vagas = 0;
int prioridade = 0;
int id_cliente = 0;
int id_emer = 0;
int emer = 0;
int inx = 0;

int main()
{
    sem_init(&vet, 0, NV);
    sem_init(&cli, 0, 0);
    sem_init(&sem_vaga, 0, CAPACIDADE_MAX);

    pthread_barrier_init(&barrier, NULL, NC);

    pthread_t c[NC], v[NV], e[NE];
    int i;
    int *id;

    for (i = 0; i < NC; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&c[i], NULL, cliente, (void *)(id));
    }

    for (i = 0; i < NV; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&v[i], NULL, veterinario, (void *)(id));
    }

    for (i = 0; i < NE; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&e[i], NULL, emergencia, (void *)(id));
    }
    pthread_join(c[0], NULL);
    return 0;
}

void *veterinario(void *arg)
{
    int i = *((int *)arg);
    while (1)
    {

        sleep(1);
        sem_wait(&vet);
        pthread_mutex_lock(&lock_fila1);
        while (inx == 1)
        {
            pthread_cond_wait(&veterinario_cond, &lock_fila1);
        }
        sleep(2);
        printf(YELLOW "Enfermeira %d aplicou a vacina na paciente %d.\n" RESET, i + 1, id_cliente + 1);
        sleep(3);
        capMaxima++;
        //printf(CYAN "Veterinario %d terminou de atender o cliente, n:%d.\n" RESET, i + 1, capMaxima);
        sleep(1);
        if (capMaxima == ATENDIMENTO_DIARIO)
        {
            inx = 1;
            printf(CYAN "Foram aplicados todas as vacinas do dia\n" RESET);
            printf(CYAN "Posta já vai fechar ...\n" RESET);
            sleep(2);
        }
        pthread_mutex_unlock(&lock_fila1);
        sem_post(&cli);
    }
}

void *cliente(void *arg)
{
    int i = *((int *)arg);
    while (1)
    {
        sleep(i % 5);
        if (sem_trywait(&sem_vaga) == 0 && inx == 0)
        {

            printf(GREEN "Paciente %d entrou na fila de espera \n" RESET, i + 1);

            pthread_mutex_lock(&lock_fila);
            if (inx == 0)
            {
                pthread_mutex_lock(&lock_fila2);
                while (emer > 0)
                {
                    pthread_cond_wait(&cliente_cond, &lock_fila2);
                }
                id_cliente = i;
                printf(BLUE "Chegou a vez do paciente %d ser atendido \n" RESET, i + 1);
                sem_post(&sem_vaga);
                sleep(1);
                sem_post(&vet);

                sem_wait(&cli);
                pthread_mutex_unlock(&lock_fila2);
                printf(MAGENTA "Paciente %d foi atendido e foi embora \n" RESET, i + 1);
                sleep(1);
            }
            else
            {
                printf("Paciente %d foi embora, acabaram as vacians\n", i + 1);
                sleep(2);
            }
            pthread_mutex_unlock(&lock_fila);
        }
        else
        {
            if (inx == 1)
            {
                pthread_barrier_wait(&barrier);
                if (i == 0)
                {
                    for (int j = 0; j < CAPACIDADE_MAX; j++)
                    {
                        sem_post(&sem_vaga);
                    }
                    inx = 0;
                    capMaxima = 0;
                    pthread_cond_broadcast(&veterinario_cond);
                    pthread_cond_broadcast(&emergencia_cond);
                }
            }
            else
            {
                printf("muitas pessoas na fila, paciente %d indo embora\n", i + 1);
                sleep(3);
            }
        }
    }
}

void *emergencia(void *arg)
{
    int i = *((int *)arg);
    while (1)
    {
        sleep(rand() % (i + 1));
        pthread_mutex_lock(&lock_fila);
        while (inx == 1)
        {
            pthread_cond_wait(&emergencia_cond, &lock_fila);
        }
        emer++;
        printf(RED "Uma senhora apareceu e vai pegar prioridade \n" RESET);
        id_cliente = i;
        sleep(1);
        sem_post(&vet);

        sem_wait(&cli);

        pthread_cond_broadcast(&cliente_cond);
        emer--;
        pthread_mutex_unlock(&lock_fila);
        printf(MAGENTA "A senhora foi atendido e foi embora \n" RESET);
        sleep(1);
    }
}