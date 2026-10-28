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
    if(value < 0) return ERROR;                                                     
    semaphore_t *sem = mem_alloc(sizeof(semaphore_t));                             
    if(!sem) return ERROR;               
        
    sem->counter = value;              
    sem->lock = 0;                                                                     // Libera o spinlock inicial
    sem->destroyed = 0;                                                                
    sem->waiters = 0;              
    sem->queue = queue_create();

    if(!sem->queue){                 
        mem_free(sem);                 
        return ERROR;               
    }                
        
    sem->id = map_put(semaphore_map, sem);                                               // Adiciona o semáforo ao mapa e obtém um ID único
    if(sem->id == ERROR){               
        queue_destroy(sem->queue);                 
        mem_free(sem);                 
        return ERROR;               
    }

    return sem->id;                
}                
        
// destrói um semáforo, liberando recursos e tarefas bloqueadas              
// Retorno: NOERROR (0) ou ERROR (<0)                
int sem_destroy(int sem_id){
    semaphore_t *sem = map_del(semaphore_map, sem_id);                                 // Remove o semáforo do mapa
    if(!sem) return ERROR;

    spin_lock(&sem->lock);
    sem->destroyed = 1;

    struct task_t *task = queue_head(sem->queue);
    while(task){                                                                      // Acorda todas as tarefas bloqueadas no semáforo
        queue_del(sem->queue, task);
        task_awake(task);
        task = queue_head(sem->queue);
    }

    int task_block = (sem->waiters == 0);                                                  // Verifica se há tarefas bloqueadas no semáforo

    spin_unlock(&sem->lock);                                                            

    if(task_block){                                                                        // Se não houver tarefas bloqueadas, libera os recursos do semáforo
        queue_destroy(sem->queue);
        mem_free(sem);
    }

    return NOERROR;
}

// Requisita acesso a um semáforo
// Retorno: NOERROR (0) ou ERROR (<0)
int sem_down(int sem_id){
    semaphore_t *sem = map_get(semaphore_map, sem_id);
    if(!sem) return ERROR;

    spin_lock(&sem->lock);

    if(sem->destroyed){                                                                 // Se o semáforo foi destruído, retorna erro
        spin_unlock(&sem->lock);
        return ERROR;
    }

    sem->counter--;

    if(sem->counter < 0){                                                               // Se o contador for menor que zero, bloqueia a tarefa atual
        sem->waiters++;                                                                 // Incrementa o contador de tarefas bloqueadas
        spin_unlock(&sem->lock);

        task_suspend(sem->queue);                                                       // Bloqueia a tarefa atual na fila do semáforo

        spin_lock(&sem->lock);                                                          

        sem->waiters--;                                                                 // Decrementa o contador de tarefas bloqueadas

        if(sem->destroyed){                                                             // Se o semáforo foi destruído, retorna erro
            int task_block = (sem->waiters == 0);
            spin_unlock(&sem->lock);                                                    // Libera o spinlock do semáforo

            if(task_block){                                                             // Se não houver tarefas bloqueadas, libera os recursos do semáforo
                queue_destroy(sem->queue);
                mem_free(sem);
            }

            return ERROR;
        }

        spin_unlock(&sem->lock);
    } 

    else spin_unlock(&sem->lock);
    

    return NOERROR;
}

// libera o acesso a um semáforo
// Retorno: NOERROR (0) ou ERROR (<0)
int sem_up(int sem_id){
    semaphore_t *s = map_get(semaphore_map, sem_id);
    if(!s) return ERROR;

    spin_lock(&s->lock);

    if(s->destroyed){                                                                 // Se o semáforo foi destruído, retorna erro
        spin_unlock(&s->lock);
        return ERROR;
    }

    s->counter++;                                                                     // Incrementa o contador do semáforo
    if(s->counter <= 0){                                                              // Se o contador for menor ou igual a zero, há tarefas esperando
        struct task_t *task = queue_head(s->queue);
        if(task){                                                                     // Se houver uma tarefa esperando, remove-a da fila e acorda-a
            queue_del(s->queue, task);
            task_awake(task);
        }
    }

    spin_unlock(&s->lock);                                                            
    return NOERROR;
}
