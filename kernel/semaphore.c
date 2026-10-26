//GRR20244409 Iago Cardoso Bariuka
// PingPongOS - PingPong Operating System
// Este arquivo PODE/DEVE ser alterado.
// Semáforos e spinlocks

#include "lib/queue.h"
#include "lib/map.h"
#include "semaphore.h"
#include "dispatcher.h"
#include "memory.h"


struct map_t *semaphore_map = NULL;                                                  // Variável global restrita a este arquivo
        
extern struct queue_t *ready_queue;                                                  // Fila de tarefas prontas
extern struct task_t *current_task;                                                  // Tarefa atualmente em execução
        
typedef struct{              
    int id;                                                                          // Identificador do semáforo
    int counter;                                                                     // Contador do semáforo
    int lock;                                                                        // Spinlock para proteger o semáforo
    int destroyed;                                                                   // Flag indicando se o semáforo foi destruído
    int waiters;                                                                     // Contador de tarefas bloqueadas no semáforo
    struct queue_t *queue;                                                           // Fila de tarefas bloqueadas no semáforo
} semaphore_t;               
        
// inicia o subsistema de semáforos              
// (chamada pelo núcleo na inicialização).               
void sem_init(){                 
    semaphore_map = map_create(50);                                                  // Cria um mapa de semaforos com 50 posicoes
}                
        
// encerra o subsistema de semáforos                 
// (chamada pelo núcleo no encerramento).                
void sem_term(){                 
    if(semaphore_map) map_destroy(semaphore_map);                
}                
        
// trava um spin-lock (busy wait)                
void spin_lock(int *lock){               
  while(__sync_fetch_and_or(lock, 1));                                               // atomic OR, que retorna o valor antigo de *lock
}                
        
// libera um spin-lock               
void spin_unlock(int *lock){                 
    (*lock) = 0;                                                                 
}                
        
// Cria um novo semáforo, inicializado com value >= 0.               
// Retorno: descritor do semáforo ou -1 (erro).              
int sem_create(int value) {              
    if(value < 0) return -1;                                                     
    semaphore_t *s = mem_alloc(sizeof(semaphore_t));                             
    if (!s) return -1;               
        
    s->counter = value;              
    s->lock = 0;                                                                     // Libera o spinlock inicial
    s->destroyed = 0;                                                                
    s->waiters = 0;              
    s->queue = queue_create();               
    if (!s->queue) {                 
        mem_free(s);                 
        return -1;               
    }                
        
    s->id = map_put(semaphore_map, s);                                               // Adiciona o semáforo ao mapa e obtém um ID único
    if(s->id < 0){               
        queue_destroy(s->queue);                 
        mem_free(s);                 
        return -1;               
    }                
    return s->id;                
}                
        
// destrói um semáforo, liberando recursos e tarefas bloqueadas              
// Retorno: NOERROR (0) ou ERROR (<0)                
int sem_destroy(int sem_id) {                
    semaphore_t *s = map_get(semaphore_map, sem_id);                 
    if(!s) return -1;                                                                // Semáforo não encontrado ou já destruído
    spin_lock(&s->lock);                                                             // Protege o semáforo antes de mexer no estado
        
    s->destroyed = 1;                
    struct task_t *task = queue_head(s->queue);              
    while(task){                                                                     // Acorda todas as tarefas bloqueadas no semáforo
        queue_del(s->queue, task);               
        task_awake(task);                
        task = queue_head(s->queue);                 
    }                
        
    spin_unlock(&s->lock);               
    map_del(semaphore_map, sem_id);                                                  // Remove o semáforo do mapa

    // Tarefas bloqueadas ainda podem retornar de task_suspend e precisam
    // consultar o estado destroyed antes de o semáforo ser liberado.
    if(s->waiters == 0){
        queue_destroy(s->queue);
        mem_free(s);
    }

    return 0;
}

// Requisita acesso a um semáforo
// Retorno: NOERROR (0) ou ERROR (<0)
int sem_down(int sem_id){
    semaphore_t *s = map_get(semaphore_map, sem_id);
    if(!s) return -1;
    spin_lock(&s->lock);                                                              // Protege o semáforo antes de mexer no contador

    if(s->destroyed){                                                                 // Se o semáforo foi destruído, retorna erro
        spin_unlock(&s->lock);
        return -1;
    }

    s->counter--;                                                                
    if(s->counter < 0){                                                               // Se o contador for negativo, a tarefa precisa esperar
        s->waiters++;
        spin_unlock(&s->lock);
        task_suspend(s->queue);                                                       // Suspende a tarefa atual
    }

    spin_unlock(&s->lock);
    return 0;
}

// libera o acesso a um semáforo
// Retorno: NOERROR (0) ou ERROR (<0)
int sem_up(int sem_id){
    semaphore_t *s = map_get(semaphore_map, sem_id);
    if(!s) return -1;

    spin_lock(&s->lock);

    if(s->destroyed){                                                                 // Se o semáforo foi destruído, retorna erro
        spin_unlock(&s->lock);
        return -1;
    }

    s->counter++;                                                                     // Incrementa o contador do semáforo
    if(s->counter <= 0){                                                              // Se o contador for menor ou igual a zero, há tarefas esperando
        struct task_t *task = queue_head(s->queue);
        if(task){                                                                     // Se houver uma tarefa esperando, remove-a da fila e acorda-a
            queue_del(s->queue, task);
            task_awake(task);
        }
    }
    spin_unlock(&s->lock);                                                            // Libera o spinlock do semáforo
    return 0;
}
