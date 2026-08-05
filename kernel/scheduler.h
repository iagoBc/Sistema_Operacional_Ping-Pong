// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Escalonador de tarefas prontas.

#ifndef __PPOS_SCHEDULER__
#define __PPOS_SCHEDULER__

#include "lib/queue.h"

// inicia o subsistema do escalonador de tarefas
// (chamada pelo núcleo na inicialização).
void sched_init();

// encerra o subsistema do escalonador de tarefas
// (chamada pelo núcleo no encerramento).
void sched_term();

// função escalonador: devolve a próxima tarefa a escalonar na fila
struct task_t *scheduler(struct queue_t *ready_queue);

// muda a prioridade de uma tarefa (da tarefa atual se task == NULL)
void sched_setprio(struct task_t *task, int prio);

// obtém a prioridade de uma tarefa (da tarefa atual se task == NULL)
int sched_getprio(struct task_t *task);

#endif
