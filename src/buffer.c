#include "buffer.h"
#include <stdlib.h>
#include "process_sync.h"

void buffer_inicializa(buffer_t *self) {
    sem_init(&semaphores.buffer_slots_vazios, 0,
             config.capacidade_buffer);  // Inicializa semaforo de espacos vazios
    sem_init(&semaphores.buffer_slots_ocupados, 0,
             0);  // Inicializa semaforo de lampadas ocupadas
    sem_init(&semaphores.buffer_disponivel, 0,
             1);  // Inicializa semaforo de acesso de disponibilidade

    pthread_mutex_init(&mutexes.buffer_slots_ocupados, NULL);

    self->put_idx = 0;
    self->take_idx = 0;
    self->slots_ocupados = 0;

    self->slots =
        (lampada_t **)malloc(sizeof(lampada_t *) * config.capacidade_buffer);

    plog("[buffer] Inicializado\n");
}

void buffer_insere(buffer_t *self, lampada_t *lampada) {
    sem_wait(&semaphores.buffer_slots_vazios);

    sem_wait(&semaphores.buffer_disponivel);
    self->slots[self->put_idx] = lampada;
    sem_post(&semaphores.buffer_disponivel);

    sem_post(&semaphores.buffer_slots_ocupados);

    self->put_idx = (self->put_idx + 1) % config.capacidade_buffer;
    pthread_mutex_lock(&mutexes.bancada_slots_ocupados);
    self->slots_ocupados++;
    pthread_mutex_unlock(&mutexes.bancada_slots_ocupados);
}

lampada_t *buffer_remove(buffer_t *self) {
    sem_wait(&semaphores.buffer_slots_ocupados);

    sem_wait(&semaphores.buffer_disponivel);
    lampada_t *lampada = self->slots[self->take_idx];
    sem_post(&semaphores.buffer_disponivel);

    sem_post(&semaphores.buffer_slots_vazios);

    self->take_idx = (self->take_idx + 1) % config.capacidade_buffer;
    pthread_mutex_lock(&mutexes.bancada_slots_ocupados);
    self->slots_ocupados--;
    pthread_mutex_unlock(&mutexes.bancada_slots_ocupados);

    return lampada;
}

void buffer_finaliza(buffer_t *self) {
    sem_destroy(&semaphores.buffer_disponivel); // Destroi semaforo da disponibilidade do buffer
    sem_destroy(&semaphores.buffer_slots_vazios); // Destroi semaforo dos espacos vazios do buffer
    sem_destroy(&semaphores.buffer_slots_ocupados);  // Destroi semaforo dos espacos ocupados do buffer

    pthread_mutex_destroy(&mutexes.buffer_slots_ocupados); // Destroi mutex dos espacos ocupados
    free(self->slots);
    plog("[buffer] Finalizado\n");
}