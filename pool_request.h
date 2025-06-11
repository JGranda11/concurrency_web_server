#ifndef POOL_REQUEST_H
#define POOL_REQUEST_H

#include <pthread.h>
#include <stdbool.h>

// Estructura del buffer
typedef struct buffer
{
    int *elementos;
    int indice;
    int back_indice;
    int count;
    int capacidad;

    pthread_mutex_t lock;
    pthread_cond_t produce;
    pthread_cond_t consume;
} buffer_t;

void inicializar(buffer_t *buffer, int capacidad);

void destruir(buffer_t *buffer);

void insertar(buffer_t *buffer, int item);

int borrar(buffer_t *buffer);

static inline bool is_buffer_full(const buffer_t *buffer)
{
    return buffer->count == buffer->capacidad;
}

static inline bool is_buffer_empty(const buffer_t *buffer)
{
    return buffer->count == 0;
}

#endif