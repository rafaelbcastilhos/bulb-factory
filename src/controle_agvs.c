#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "controle_agvs.h"
#include "process_sync.h"
#include "agv_data.h"

void controle_inicializa_agvs(controle_agvs_t * controle, unsigned int quantidade_agvs){
    // Inicializa o semáforo posiciona para a quantidade máxima de carrinhos posicionados
    sem_init(&semaphores.agv_posiciona, 0, 2);
    pthread_mutex_init(&mutexes.agv_reciclar_prox, NULL); // Inicializa mutex de controle para reciclagem do proximo agv

    agv_data.reciclar_prox = 0;
    agv_data.total_lampadas_recebidas = 0;

    // Inicializa os semáforos que regulam o transporte
    sem_init(&semaphores.agv_transporta_reprovadas, 0, 0);
    sem_init(&semaphores.agv_transporta_aprovadas, 0, 0);
    sem_init(&semaphores.agvs_ambos_disponiveis, 0, 0);

    sem_init(&semaphores.finaliza_sistema, 0, 0); // Inicializa o semáforo que controla a finalização do sistema

    controle->quantidade_agvs = quantidade_agvs;
    controle->agvs = (agv_t *) malloc(sizeof(agv_t) * quantidade_agvs);
    for (int i = 0; i < quantidade_agvs; i++)
        agv_inicializa(&controle->agvs[i], i);

    plog("[controle AGVs] Inicializado\n");
}

agv_t * controle_retorna_agv(controle_agvs_t * self, bool reciclagem){
    // Espera até que ambos os AGVs estejam posicionados
    sem_wait(&semaphores.agvs_ambos_disponiveis);
    sem_post(&semaphores.agvs_ambos_disponiveis);
    for (int i = 0; i < config.quantidade_agvs; i++) {
        if (self->agvs[i].posicionado && self->agvs[i].reciclar == reciclagem) 
            return &self->agvs[i];
    }

    plog("[controle AGVs] AGV Posicionado não encontrado\n");
    return NULL;
}

void controle_finaliza_agvs(controle_agvs_t * controle){
    // Destroi os semáforos que regulam o transporte
    sem_destroy(&semaphores.finaliza_sistema);
    sem_destroy(&semaphores.agv_transporta_reprovadas);
    sem_destroy(&semaphores.agv_transporta_aprovadas);
    sem_destroy(&semaphores.agvs_ambos_disponiveis);
    sem_destroy(&semaphores.agv_posiciona);

    pthread_mutex_destroy(&mutexes.agv_reciclar_prox); // Destroi o mutex que controla a reciclagem do proximo

    for (int i = 0; i < controle->quantidade_agvs; i++)
        agv_finaliza(&controle->agvs[i]);
    free(controle->agvs);

    plog("[controle AGVs] Finalizado\n");
}
