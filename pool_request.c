#include <stdlib.h>
#include <stdlib.h>
#include <pthread.h>
#include "pool_request.h"

// instancia global

static buffer_t *global_buffer = NULL;

void inicializar(buffer_t *buf, int capacidad)
{
    buf->elementos = (int *)malloc(sizeof(int) * capacidad);
    buf->indice = 0;
    buf->back_indice = 0;
    buf->count = 0;
    buf->capacidad = capacidad;
    //incializar
    pthread_mutex_init(&buf->lock, NULL);
    pthread_cond_init(&buf->produce, NULL);
    pthread_cond_init(&buf->consume, NULL);
}

void destruir(buffer_t *buffer)
{
    free(buffer->elementos);
    buffer->elementos = NULL;

    pthread_mutex_destroy(&buffer->lock);
    pthread_cond_destroy(&buffer->produce);
    pthread_cond_destroy(&buffer->consume);

    buffer->indice = 0;
    buffer->back_indice = 0;
    buffer->count = 0;
    buffer->capacidad = 0;
}

void insertar(buffer_t *buffer, int item)
{
    pthread_mutex_lock(&buffer->lock);
    while (is_buffer_full(buffer))
    {
        pthread_cond_wait(&buffer->produce, &buffer->lock);
    }

    buffer->elementos[buffer->back_indice] = item;
    buffer->back_indice = (buffer->back_indice + 1) % buffer->capacidad;
    buffer->count++;

    pthread_cond_signal(&buffer->consume);
    pthread_mutex_unlock(&buffer->lock);
}

int borrar(buffer_t *buffer)
{
    pthread_mutex_lock(&buffer->lock);

    while (is_buffer_empty(buffer))
    {
        pthread_cond_wait(&buffer->consume, &buffer->lock);
    }

    int delete = buffer->elementos[buffer->indice];
    buffer->indice = (buffer->indice + 1) % buffer->capacidad;
    buffer->count--;

    // SEÑALAR a los productores que hay espacio
    pthread_cond_signal(&buffer->produce);
    pthread_mutex_unlock(&buffer->lock);

    // RETORNAR el elemento extraído
    return delete;
}