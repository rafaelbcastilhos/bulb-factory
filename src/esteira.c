#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "config.h"
#include "esteira.h"
#include <semaphore.h>
#include "process_sync.h"

void esteira_insere(esteira_t *self);

void * esteira_executa(void *arg) {
    plog("[esteira] Inicializada\n");

    /* Recupera o argumento de entrada (esteira_t). */
    esteira_t *esteira = (esteira_t *) arg;

    double fator = (double) (ESTEIRA_VEL_MAX + 1 - config.velocidade_esteira);
    
    /* Produz config.total_lampadas lâmpadas. */
    for (int i = 0; i < config.quantidade_lampadas; i++) {
        // Espera até que hajam slots livres no buffer
        sem_wait(&semaphores.buffer_slots_vazios);
        sem_post(&semaphores.buffer_slots_vazios);

        esteira_insere(esteira);
        sem_post(&semaphores.esteira_lampadas_disponiveis); // incrementa semaforo
        msleep((long) ESTEIRA_VEL_TEMPO * pow(2.0, fator));
    }

    pthread_exit(NULL);
}

void esteira_inicializa(esteira_t *self) {
    self->tipo = ESTEIRA;
    sem_init(&semaphores.finaliza_sistema, 0, 0); // Inicializa semaforo para controle de finalizacao do sistema

    self->lampadas_produzidas = 0;
    self->lampadas_consumidas = 0;
    self->lampadas = (lampada_t *) malloc(sizeof(lampada_t) * config.quantidade_lampadas);
    sem_init(&semaphores.esteira_lampadas_disponiveis, 0, 0); // Inicializa semaforo da disponibilidade da esteira

    if (pthread_create(&self->thread, NULL, esteira_executa, (void *) self) != 0) {
        plog("[esteira] Erro ao inicializar esteira\n");
    }
}

void esteira_insere(esteira_t *self) {   
    if (rand() % 2 == 0)
        lampada_inicializa(&(self->lampadas[self->lampadas_produzidas]), self->lampadas_produzidas, OK, OK, OK, OK);
    else
        lampada_inicializa(&(self->lampadas[self->lampadas_produzidas]), self->lampadas_produzidas, rand() % 2, rand() % 2, rand() % 2, rand() % 2);
    
    plog("[esteira] Lâmpada %u INSERIDA na esteira (bulbo: %s, luz: %s, rosca: %s, marca: %s)\n", self->lampadas_produzidas, lampada_param_status(self->lampadas[self->lampadas_produzidas].bulbo), 
        lampada_param_status(self->lampadas[self->lampadas_produzidas].luz), lampada_param_status(self->lampadas[self->lampadas_produzidas].rosca), lampada_param_status(self->lampadas[self->lampadas_produzidas].marca));
    
    self->lampadas_produzidas++;
}

lampada_t * esteira_remove(esteira_t *self) {
    return(&self->lampadas[self->lampadas_consumidas++]);
}

void esteira_finaliza(esteira_t *self) {
    sem_wait(&semaphores.finaliza_sistema);
    pthread_join(self->thread, NULL);
    sem_destroy(&semaphores.esteira_lampadas_disponiveis); // Destroi semaforo de disponibilidade da esteira
    free(self->lampadas);
    plog("[esteira] Finalizada\n");
}
