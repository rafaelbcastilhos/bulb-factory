#ifndef __agv_DATA_H__
#define __agv_DATA_H__

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

/*============================================================================*
 * Dados para comunicação entre AGVs e controlador                            *
 *============================================================================*/

typedef struct agv_data_s {
    bool reciclar_prox;         /* Informa o sistema em qual modo (Reciclagem ou não) o próximo AGV deve se posicionar */
    unsigned int total_lampadas_recebidas;
    unsigned int total_lampadas_entregues;
} agv_data_t;

agv_data_t agv_data;

#endif