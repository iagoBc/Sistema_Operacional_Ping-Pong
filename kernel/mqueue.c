// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência de filas de mensagens

#include "mqueue.h"
#include "lib/queue.h"
#include "lib/map.h"
#include "lib/pplibc.h"
#include "semaphore.h"
#include "dispatcher.h"
#include "memory.h"
#include "task.h"
#include <string.h>

struct map_t *mqueue_map = NULL;

typedef struct{
    int id;                                                                         // Identificador único da fila de mensagens               
    int msg_size;                                                                   // Tamanho de cada mensagem
    struct queue_t *buffer;                                                         // Fila de mensagens
    int mutex;                                                                      // Semáforo de exclusão mútua
    int max_msgs;                                                                   // Semáforo de limite máximo de mensagens
    int msg;                                                                        // Semáforo de mensagens disponíveis
}mqueue_t;

// inicia o subsistema de filas de mensagens
// (chamada pelo núcleo na inicialização).
void mqueue_init(){
    mqueue_map = map_create(50);                                                    // Cria um mapa para armazenar as filas de mensagens   
}

// encerra o subsistema de filas de mensagens
// (chamada pelo núcleo no encerramento).
void mqueue_term(){
    if(mqueue_map) map_destroy(mqueue_map);                                         // Destroi o mapa de filas de mensagens 
}

// cria uma fila de mensagens.
// Retorno: descritor da nova fila ou ERROR
int mqueue_create(int max_msgs, int msg_size){
    if(max_msgs <= 0 || msg_size <= 0) return ERROR;                                                
    mqueue_t *mq = mem_alloc(sizeof(mqueue_t));                             
    if(!mq) return ERROR;  

    mq->msg_size = msg_size;
    mq->buffer = queue_create();                                                    // Cria a fila de mensagens        

    if(!mq->buffer){
        mem_free(mq);
        return ERROR;
    }

    mq->max_msgs = sem_create(max_msgs);                                            // Cria semáforo de limite máximo de mensagens
    mq->mutex = sem_create(1);
    mq->msg = sem_create(0);

    if(mq->max_msgs == ERROR || mq->mutex == ERROR || mq->msg == ERROR){
        if(mq->max_msgs != ERROR) sem_destroy(mq->max_msgs);
        if(mq->mutex != ERROR) sem_destroy(mq->mutex);
        if(mq->msg != ERROR) sem_destroy(mq->msg);
        mem_free(mq);
        return ERROR;
    }

    mq->id = map_put(mqueue_map, mq);                                               // Adiciona a fila de mensagens ao mapa e obtém um ID único
    if(mq->id == ERROR){
        sem_destroy(mq->max_msgs);
        sem_destroy(mq->mutex);
        sem_destroy(mq->msg);               
        queue_destroy(mq->buffer);                 
        mem_free(mq);                 
        return ERROR;               
    }

    return mq->id; 
}

// destrói uma fila de mensagens, liberando recursos e tarefas
// Retorno: NOERROR ou ERROR
int mqueue_destroy(int mqueue_id){
    mqueue_t *mq = map_del(mqueue_map, mqueue_id);                 
    if(!mq) return ERROR;                                                                
                                   
    void *msg = queue_head(mq->buffer);              
    while(msg){                                                                     // Libera a memória de cada msg                                                                   
        queue_del(mq->buffer, msg);
        mem_free(msg);                               
        msg = queue_head(mq->buffer);                 
    }                
    
    if(queue_destroy(mq->buffer) == ERROR) return ERROR;
    if(sem_destroy(mq->max_msgs) == ERROR) return ERROR;
    if(sem_destroy(mq->mutex) == ERROR) return ERROR;
    if(sem_destroy(mq->msg) == ERROR)return ERROR;

    mem_free(mq);                                                  

    return NOERROR;
}

// envia uma mensagem
// Retorno: NOERROR ou ERROR
int mqueue_send(int mqueue_id, void *msg){
    mqueue_t *mq = map_get(mqueue_map, mqueue_id);
    if(!mq || !msg) return ERROR;

    if(sem_down(mq->max_msgs) == ERROR) return ERROR;                               // Decrementa o semáforo de limite máximo de mensagens

    if(sem_down(mq->mutex) == ERROR){                                               // Decrementa o semáforo de exclusão mútua
        sem_up(mq->max_msgs);                                                       // Incrementa o semáforo de limite máximo de mensagens
        return ERROR;
    }

    void *copy = mem_alloc(mq->msg_size);
    if(!copy){
        sem_up(mq->mutex);
        sem_up(mq->max_msgs);
        return ERROR;
    }

    memcpy(copy, msg, mq->msg_size);                                                // Copia a mensagem para a memória alocada

    if(queue_add(mq->buffer, copy) == ERROR){                                       // Adiciona a mensagem à fila de mensagens
        mem_free(copy);
        sem_up(mq->mutex);
        sem_up(mq->max_msgs);
        return ERROR;
    }

    if(sem_up(mq->mutex) == ERROR) return ERROR;
    if(sem_up(mq->msg) == ERROR) return ERROR;

    return NOERROR;
}

// recebe uma mensagem
// Retorno: NOERROR ou ERROR
int mqueue_recv(int mqueue_id, void *msg){
    mqueue_t *mq = map_get(mqueue_map, mqueue_id);
    if(!mq || !msg) return ERROR;
                                                     
    if(sem_down(mq->msg) == ERROR) return ERROR;                                    // Decrementa o semáforo de item
    if(sem_down(mq->mutex) == ERROR) return ERROR;
    
    void *item = queue_head(mq->buffer);                                            // Pega o item do buffer
    if(!item) return ERROR;

    memcpy(msg, item, mq->msg_size);                                                // Copia a mensagem para o buffer fornecido
    if(!msg) return ERROR;

    if(queue_del(mq->buffer, item) == ERROR) return ERROR;                          // Remove a mensagem da fila de mensagens

    if(sem_up(mq->mutex) == ERROR) return ERROR;
    if(sem_up(mq->max_msgs) == ERROR) return ERROR;

    if(mem_free(item) == ERROR) return ERROR;

    return NOERROR;
}

// retorna o numero de mensagens em uma fila
// Retorno: número >= 0 ou ERROR
int mqueue_msgs(int mqueue_id){
    mqueue_t *mq = map_get(mqueue_map, mqueue_id);
    if(!mq) return ERROR;
    return queue_size(mq->buffer);
}

