#include "bancada.h"
#include <semaphore.h>
#include <stdlib.h>
#include "lampada.h"
#include "process_sync.h"

void bancada_inicializa(bancada_t *self) {
    sem_init(&semaphores.bancada_slots_testados, 0, 0); // Inicializa semaforo de espacos testados
    sem_init(&semaphores.bancada_slots_vazios, 0, config.capacidade_bancada); // Inicializa semaforo de espacos vazios
    sem_init(&semaphores.bancada_disponivel, 0, 1); // Inicializa semaforo de disponivilidade da bancada
    pthread_mutex_init(&mutexes.bancada_slots_ocupados, NULL); // Inicializa mutex de espacos ocupados da bancada

    self->slots_ocupados = 0;
    self->total_testadas = 0;
    self->total_aprovadas = 0;
    self->total_reprovadas = 0;
    self->slots =
        (lampada_t **)malloc(sizeof(lampada_t *) * config.capacidade_bancada);

    for (int i = 0; i < config.capacidade_bancada; i++) {
        self->slots[i] = NULL;
    }

    plog("[bancada] Inicializada\n");
}

void bancada_insere(bancada_t *self, lampada_t *lampada) {
    sem_wait(&semaphores.bancada_slots_vazios);
    // Encontra um slot vazio
    for (int i = 0; i < config.capacidade_bancada; i++) {
        if (self->slots[i] == NULL) {
            sem_wait(&semaphores.bancada_disponivel);
            self->slots[i] = lampada;
            sem_post(&semaphores.bancada_disponivel);
            break;
        }
    }

    pthread_t thread;

    // Inicia thread de teste
    bancada_testa_params_t* bancada_testa_params = malloc(sizeof(bancada_testa_params_t));
    bancada_testa_params->self = self;
    bancada_testa_params->lampada = lampada;
    pthread_create(&thread, NULL, bancada_testa, (void *)bancada_testa_params);

    /* Incrementa a quantidade de slots ocupados na bancada. */
    pthread_mutex_lock(&mutexes.bancada_slots_ocupados);
    self->slots_ocupados++;
    pthread_mutex_unlock(&mutexes.bancada_slots_ocupados);
}

// bool bancada_testa(bancada_t *self, lampada_t *lampada) {
void *bancada_testa(void *params_void) {
    /* Simula um tempo aleatório de teste da lâmpada. NÃO REMOVER! */
    msleep(rand() % config.tempo_max_teste);

    bancada_testa_params_t *params = (bancada_testa_params_t *)params_void;
    bancada_t *self = params->self;
    lampada_t *lampada = params->lampada;
    free(params);

    pthread_mutex_lock(&mutexes.total_lampadas_testadas);
    self->total_testadas++;
    pthread_mutex_unlock(&mutexes.total_lampadas_testadas);

    /* Testa se a lâmpada possui algum defeito. */
    if (lampada->bulbo == FALHA || lampada->luz == FALHA ||
        lampada->marca == FALHA || lampada->rosca == FALHA) {
        lampada->resultado_teste = REPROVADA;

        pthread_mutex_lock(&mutexes.total_lampadas_reprovadas);
        self->total_reprovadas++;
        pthread_mutex_unlock(&mutexes.total_lampadas_reprovadas);

        plog("[bancada] Resultado do teste da lâmpada %u: REPROVADA!\n",
            lampada->id);
    } else {
        pthread_mutex_lock(&mutexes.total_lampadas_aprovadas);
        self->total_aprovadas++;
        pthread_mutex_unlock(&mutexes.total_lampadas_aprovadas);
        lampada->resultado_teste = APROVADA;
        plog("[bancada] Resultado do teste da lâmpada %u: APROVADA!\n",
            lampada->id);
    }

    sem_post(&semaphores.bancada_slots_testados);
    return 0;
}

lampada_t *bancada_remove(bancada_t *self) {
    sem_wait(&semaphores.bancada_slots_testados);

    lampada_t *lampada = NULL;
    for (int i = 0; i < config.capacidade_bancada; i++) {
        if (self->slots[i] != NULL) {
            if (self->slots[i]->resultado_teste != DESCONHECIDO) {
                sem_wait(&semaphores.bancada_disponivel);
                lampada = self->slots[i];
                self->slots[i] = NULL;
                sem_post(&semaphores.bancada_disponivel);
                break;
            }
        }
    }

    if (lampada == NULL) {
        plog("[bancada]: ERRO - LAMPADA TESTADA NÃO ENCONTRADA \n");
        return NULL;
    }

    sem_post(&semaphores.bancada_slots_vazios);

    /* Decrementa a quantidade de slots ocupados na bancada. */
    pthread_mutex_lock(&mutexes.bancada_slots_ocupados);
    self->slots_ocupados--;
    pthread_mutex_unlock(&mutexes.bancada_slots_ocupados);
    return lampada;
}

void bancada_finaliza(bancada_t *self) {
    sem_destroy(&semaphores.bancada_disponivel); // Destroi semaforo da disponibilidade da bancada
    sem_destroy(&semaphores.bancada_slots_vazios); // Destroi semaforo de espacos vazios da bancada
    sem_destroy(&semaphores.bancada_slots_testados); // Destroi semaforo de espacos testados
    pthread_mutex_destroy(&mutexes.bancada_slots_ocupados); // Destroi mutex de espacos ocupados 
    
    free(self->slots);
    plog("[bancada] Finalizada\n");
}

void bancada_imprime_resultados(bancada_t *self) {
    printf("\n");
    printf(BAR);
    printf("RESULTADOS DA SIMULAÇÃO\n");
    printf(BAR);
    printf(
        "Lâmpadas testadas  : %u\nLâmpadas aprovadas : %u\nLâmpadas "
        "reprovadas: %d\n",
        self->total_testadas, self->total_aprovadas, self->total_reprovadas);
    printf(BAR);
    printf("\n");
}