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

void *enfermeira(void *arg);
void *paciente(void *arg);
void *emergencia(void *arg);

#define NP 15 // numero de pacientes
#define EN 4  // numero de enfermeiras  
#define CAPACIDADE_MAX 5 // numero de pessoas na fila para acharem que esta grande
#define VACINAS_DIARIAS 10 // numero total de atendimentos diarios
#define NE 1  // numero de casos de emergencia

sem_t pac; // semaforo para paciente
sem_t enf; // semaforo para enfermeira
sem_t sem_vaga; // semaforo para indicar as vagas da fila

pthread_barrier_t barrier; // criando uma barreira

// criando os locks
pthread_mutex_t lock_fila = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_fila1 = PTHREAD_MUTEX_INITIALIZER;

// criando as variaveis de condicoes
pthread_cond_t paciente_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t emergencia_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t enfermeira_cond = PTHREAD_COND_INITIALIZER;

// variaveis globais para auxiliar no programa
int capMaxima = 0;
int vagas = 0;
int prioridade = 0;
int id_paciente = 0;
int id_emer = 0;
int emer = 0;
int aux = 0;

int main()
{
    // inicializando os semaforos
    sem_init(&enf, 0, EN); 
    sem_init(&pac, 0, 0);
    sem_init(&sem_vaga, 0, CAPACIDADE_MAX);
    // inicializando barreira
    pthread_barrier_init(&barrier, NULL, NP);

    int i;
    pthread_t c[NP], v[EN], e[NE];
    int *id;

    for (i = 0; i < NP; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&c[i], NULL, paciente, (void *)(id));
    }

    for (i = 0; i < EN; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&v[i], NULL, enfermeira, (void *)(id));
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

void *enfermeira(void *arg)
{
    int i = *((int *)arg);
    while (1)
    {

        sleep(1);
        // espera o paciente seja atendido
        sem_wait(&enf);
        // entrando no lock
        pthread_mutex_lock(&lock_fila1);
        // caso o numero de atendimentos diario seja atingido, entra na variavel de condição
        // esperando o outro dia 
        while (aux == 1)
        {
            pthread_cond_wait(&enfermeira_cond, &lock_fila1);
        }
        sleep(2);
        printf(YELLOW "Enfermeira %d aplicou a vacina no paciente %d.\n" RESET, i + 1, id_paciente + 1);
        sleep(3);
        // somando no contador de atendimentos diario
        capMaxima++;
        printf(CYAN "Numero de Vacinas aplicado:%d.\n" RESET,capMaxima);
        sleep(1);
        // caso é atingido a meta diario entra na condição
        if (capMaxima == VACINAS_DIARIAS)
        {
            // muda a variavel para 1 indicando que foi alcançado o numero diario
            aux = 1;
            printf(CYAN "Foram aplicados todas as vacinas do dia\n" RESET);
            printf(CYAN "Posta de saúde já vai fechar ...\n\n" RESET);
            sleep(2);
        }
        pthread_mutex_unlock(&lock_fila1);
        // libera o cliente
        sem_post(&pac);
    }
}

void *paciente(void *arg)
{
    int i = *((int *)arg);
    while (1)
    {
        sleep(i % 5);
        // verifica se o numero da fila esta grande ou se o numero de atendimentos diarios foi alcançado
        if (sem_trywait(&sem_vaga) == 0 && aux == 0)
        {

            printf(GREEN "Paciente %d entrou na fila de espera \n" RESET, i + 1);
            // entrando no lock
            pthread_mutex_lock(&lock_fila);
            // verifica se o numero de atendimentos foram atingidos
            // caso estiver alcançado ele deixa a fila e vai embora
            if (aux == 0)
            {
                // verifica se tem prioridade com a senhora, caso tenho entra na loop ate que ela
                // seja vacinado
                while (emer > 0)
                {
                    pthread_cond_wait(&paciente_cond, &lock_fila);
                }
                // salva o id na variavel global para usar na função ddo veterinario
                id_paciente = i;
                printf(BLUE "Chegou a vez do paciente %d ser atendido \n" RESET, i + 1);
                // libera a fila de espera
                sem_post(&sem_vaga);
                sleep(1);
                // libera para a enfermeira trabalhar
                sem_post(&enf);
                // aguarda o paciente seja vacinado
                sem_wait(&pac);
                printf(MAGENTA "Paciente %d foi atendido e foi embora \n" RESET, i + 1);
                sleep(1);
            }
            else
            {
                // caso tenha atingido os atendimentos diarios 
                printf("Paciente %d foi embora, acabaram as vacians\n", i + 1);
                sleep(2);
            }
            // saindo do lock
            pthread_mutex_unlock(&lock_fila);
        }
        else
        {
            // caso tenha atingido os atendimentos diarios  ele entra nessa condição
            if (aux == 1)
            {
                // espera todos os pacientes para inicializar o outro dia
                pthread_barrier_wait(&barrier);
                // setando os valores para o que era antes
                if (i == 0)
                {
                    printf("---------------------------------------------------------------\n");
                    printf("Cameçando outro dia da campanha da vacina\n");
                    // liberando as vagas das filas
                    for (int j = 0; j < CAPACIDADE_MAX; j++)
                    {
                        sem_post(&sem_vaga);
                    }
                    // resetando os valoros
                    aux = 0;
                    capMaxima = 0;
                    pthread_cond_broadcast(&enfermeira_cond);
                    pthread_cond_broadcast(&emergencia_cond);
                }
            }
            else
            {
                // caso o numero de filas estaja grande 
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
        // entrando no lock
        pthread_mutex_lock(&lock_fila);
        // caso o numero de atendimentos diario seja atingido, entra na variavel de condição
        // esperando o outro dia
        while (aux == 1)
        {
            pthread_cond_wait(&emergencia_cond, &lock_fila);
        }
        // seta para indicar que prioridade
        emer++;
        printf(RED "Uma senhora apareceu e vai pegar prioridade \n" RESET);
        // salva o id na variavel global para usar na função ddo veterinario
        id_paciente = i;
        sleep(1);
        sem_post(&enf);

        sem_wait(&pac);
        // libera todos que tiveram que ceder o prioridade
        pthread_cond_broadcast(&paciente_cond);
        // seta para indicar que não quer mais prioridade
        emer--;
        pthread_mutex_unlock(&lock_fila);
        printf(MAGENTA "A senhora foi atendido e foi embora \n" RESET);
        sleep(1);
    }
}