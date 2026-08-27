// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Escalonador de tarefas prontas.

#define NULL 0
#include "lib/queue.h"

void sched_init()
{
}

void sched_term()
{
}

// função escalonador FCFS: devolve a próxima tarefa a escalonar na fila
struct task_t *scheduler(struct queue_t *ready_queue){
    return queue_head(ready_queue);
}