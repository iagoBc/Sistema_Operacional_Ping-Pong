// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Chamadas oferecidas pelo PPOS às aplicações; mais tarde isto
// deve ser transformado em syscalls ativadas por traps simulados.

#ifndef __PPOS_SYSCALLS__
#define __PPOS_SYSCALLS__

#include "tcb.h"

// tarefas (task.*)
struct task_t *task_create(char *name, void (*entry)(void *), void *arg);
int task_destroy(struct task_t *task);
int   task_id(struct task_t *task);
char *task_name(struct task_t *task);
void  task_yield();
int   task_wait(struct task_t *task);
void  task_sleep(int t);
void  task_exit(int exit_code);

// só para P1 (achar uma solução mais elegante)
int task_switch(struct task_t *task);

// escalonamento (scheduler.*)
void sched_setprio(struct task_t *task, int prio);
int  sched_getprio(struct task_t *task);

// tempo (time.*)
unsigned int time();

// semáforos (semaphore.*)
int sem_create(int value);
int sem_destroy(int sem_id);
int sem_down(int sem_id);
int sem_up(int sem_id);

// filas de mensagens (mqueue.*)
int mqueue_create(int max_msgs, int msg_size);
int mqueue_destroy(int mqueue_id);
int mqueue_send(int mqueue_id, void *msg);
int mqueue_recv(int mqueue_id, void *msg);
int mqueue_msgs(int mqueue_id);

// gestão de memeória heap (memory.*)
void *mem_alloc(int size);
int   mem_free(void *ptr);
int   mem_size();
int   mem_avail();
void  mem_report();

// acesso ao disco (block.*)
int block_size();
int block_blocks();
int block_read(int block, void *buffer);
int block_write(int block, void *buffer);

#endif
