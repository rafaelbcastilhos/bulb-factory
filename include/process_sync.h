#ifndef __PROCESS_SYNC_H__
#define __PROCESS_SYNC_H__

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

/*============================================================================*
 * Semáforos e Mutexes                                                               *
 *============================================================================*/

typedef struct semaphores_s {
    sem_t esteira_lampadas_disponiveis;     // init
    sem_t buffer_disponivel;
    sem_t buffer_slots_vazios;
    sem_t buffer_slots_ocupados;
    sem_t agv_posiciona;                    // init      /* Funciona como comando para posicionar o próximo AGV da fila.      */
    sem_t agv_transporta_reprovadas;        // init
    sem_t agv_transporta_aprovadas;         // init
    sem_t agvs_ambos_disponiveis;           // init
    sem_t bancada_disponivel;
    sem_t bancada_slots_vazios;
    sem_t bancada_slots_testados;
    sem_t finaliza_sistema;
} semaphore_t;

semaphore_t semaphores;

typedef struct mutexes_s {
    pthread_mutex_t agv_reciclar_prox;
    pthread_mutex_t bancada_slots_ocupados;
    pthread_mutex_t buffer_slots_ocupados;
    pthread_mutex_t total_lampadas_entregues;
    pthread_mutex_t total_lampadas_testadas;
    pthread_mutex_t total_lampadas_aprovadas;
    pthread_mutex_t total_lampadas_reprovadas;
} mutexes_t;

mutexes_t mutexes;

#endif