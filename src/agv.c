#include "agv.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "agv_data.h"
#include "process_sync.h"

void agv_inicializa(agv_t *self, unsigned int id) {
    self->id = id;
    self->cont_lampadas = 0;
    self->posicionado = false;
    self->reciclar = false;

    if (pthread_create(&self->thread, NULL, agv_executa, (void *)self) == 0) {
        plog("[AGV %u] Inicializado\n", self->id);
    } else {
        plog("[AGV %u] Erro ao inicializar AGV\n", self->id);
    }
}

void *agv_executa(void *arg) {
    /* Recupera o argumento de entrada (agv_t). */
    agv_t *self = (agv_t *)arg;

    // Esse if é executado na inicialização, quando não há nenhum AGV
    // posicionado. Ele difere do loop principal apenas pela proteção extra,
    // necessária porque na inicialização, e apenas na inicialização, o
    // posicionamento simultaneo de 2 AGVs pode acontecer
    if (!sem_trywait(&semaphores.agv_posiciona)) {
        // Garante que os AGVs se posicionem para cargas diferentes
        pthread_mutex_lock(&mutexes.agv_reciclar_prox);
        bool reciclar_prox = agv_data.reciclar_prox;
        agv_data.reciclar_prox = !agv_data.reciclar_prox;
        pthread_mutex_unlock(&mutexes.agv_reciclar_prox);

        agv_posiciona(self, reciclar_prox);

        // Garante que não haverão 2 posts no semáforo de disponibilidade dos
        // AGVs, afinal este deve ser binário. Apenas o carrinho de reciclagem,
        // que é o segundo a chegar, faz o post
        if (reciclar_prox) {
            sem_post(&semaphores.agvs_ambos_disponiveis);
        }

        if (self->reciclar) {
            sem_wait(&semaphores.agv_transporta_reprovadas);
        } else {
            sem_wait(&semaphores.agv_transporta_aprovadas);
        }
        agv_transporta(self);
        if (agv_data.total_lampadas_entregues == config.quantidade_lampadas) {
            sem_post(&semaphores.finaliza_sistema);
        }
    }

    while (true) {
        // Posiciona assim que um carrinho sair para transporte
        sem_wait(&semaphores.agv_posiciona);
        agv_posiciona(self, agv_data.reciclar_prox);
        sem_post(&semaphores.agvs_ambos_disponiveis);

        if (self->reciclar) {
            sem_wait(&semaphores.agv_transporta_reprovadas);
        } else {
            sem_wait(&semaphores.agv_transporta_aprovadas);
        }
        agv_transporta(self);
        // Finalize o sistema se todas as lâmpadas já tiverem sido entregues
        if (agv_data.total_lampadas_entregues == config.quantidade_lampadas) {
            sem_post(&semaphores.finaliza_sistema);
        }
    }

    pthread_exit(NULL);
}

void agv_posiciona(agv_t *self, bool reciclar) {
    /* AGV se posiciona para que lâmpadas sejam colocadas no seu compartimento
     * de carga. */
    if (!self->posicionado && self->cont_lampadas == 0) {
        self->reciclar = reciclar;
        self->posicionado = true;

        if (reciclar) {
            plog(
                "[AGV %u] Posicionado para carregamento com lâmpadas para "
                "reciclagem\n",
                self->id);
        } else {
            plog(
                "[AGV %u] Posicionado para carregamento com lâmpadas aprovadas "
                "no teste\n",
                self->id);
        }
    } else {
        plog("[AGV %u] Erro ao tentar posicionar AVG em uso\n", self->id);
    }
}

void agv_insere(agv_t *self, lampada_t *lampada) {
    self->cont_lampadas++;

    agv_data.total_lampadas_recebidas++;

    if (agv_data.total_lampadas_recebidas == config.quantidade_lampadas) {
        sem_post(&semaphores.agv_transporta_reprovadas);
        sem_post(&semaphores.agv_transporta_aprovadas);
    }

    if (self->cont_lampadas == config.capacidade_agv) {
        sem_wait(&semaphores.agvs_ambos_disponiveis);
        if (self->reciclar) {
            sem_post(&semaphores.agv_transporta_reprovadas);
        } else {
            sem_post(&semaphores.agv_transporta_aprovadas);
        }
    }
    plog("[AGV %u] Lâmpada inserida no compartimento de carga.\n", self->id);
}

void agv_transporta(agv_t *self) {
    // Informa ao sistema qual carga (aprovadas/reprovadas) o próximo carrinho
    // deverá carregar
    agv_data.reciclar_prox = self->reciclar;
    self->posicionado = false;

    // Manda posicionar o próximo AGV
    sem_post(&semaphores.agv_posiciona);

    // Aumenta a contagem de lâmpadas entregues
    pthread_mutex_lock(&mutexes.total_lampadas_entregues);
    agv_data.total_lampadas_entregues += self->cont_lampadas;
    pthread_mutex_unlock(&mutexes.total_lampadas_entregues);

    self->cont_lampadas = 0;
    plog("[AGV %u] Lâmpadas transportadas para o depósito.\n", self->id);
}

void agv_finaliza(agv_t *self) {
    pthread_cancel(self->thread);
    pthread_join(self->thread, NULL);
    plog("[AGV %u] Finalizado\n", self->id);
}
