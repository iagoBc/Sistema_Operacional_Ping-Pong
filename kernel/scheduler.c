//GRR20244409 Iago Cardoso Bariuka
// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Escalonador de tarefas prontas.
#include "scheduler.h"
#include "tcb.h"

extern struct task_t *current_task;

// muda a prioridade de uma tarefa (da tarefa atual se task == NULL)
void sched_setprio(struct task_t *task, int prio){
    if(prio < -20 || prio > 20) return;
    else if(task == NULL) task = current_task;
    task->static_prio = task->dynamic_prio = prio;
}

// obtém a prioridade de uma tarefa (da tarefa atual se task == NULL)
int sched_getprio(struct task_t *task){
    if(!task) return current_task->static_prio;
    else return task->static_prio;
}

void sched_init(){}

void sched_term(){}

// função escalonador: devolve a próxima tarefa a escalonar na fila
struct task_t* scheduler(struct queue_t* ready_queue) {
    struct task_t *next = queue_head(ready_queue);
    struct task_t *it = queue_next(ready_queue);

    while(it){ // percorre a fila de tarefas prontas para encontrar a de maior prioridade(menor valor de prioridade)
        if (it->dynamic_prio < next->dynamic_prio) next = it;
        it = queue_next(ready_queue);
    }

    it = queue_head(ready_queue); 

    while(it){ // percorre a fila de tarefas prontas para envelhecer a prioridade dinâmica das demais tarefas, exceto a escolhida para execução
        if (it != next && it->dynamic_prio > -20) it->dynamic_prio--;
        it = queue_next(ready_queue);
    }

    next->dynamic_prio = next->static_prio; // restaura prioridade da tarefa escolhida para execução
    return next;
}