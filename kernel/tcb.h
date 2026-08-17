// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// Este arquivo PODE/DEVE ser alterado.

// Descritor de tarefas (TCB - Task Control Block).

#ifndef __PPOS_TCB__
#define __PPOS_TCB__

#include "ctx.h"
#define READY 1
#define RUNNING 2
#define TERMINATED 3

// Task Control Block (TCB), infos sobre uma tarefa
struct task_t
{
    int id;                     // identificador da tarefa
    char *name;                 // nome da tarefa
    struct ctx_t context;       // contexto da tarefa
    char stats;                 // pronta, executando, finalizada ...
    void *stack;                // ponteiro para a pilha da tarefa
    struct task_t *parent;      // ponteiro para a tarefa pai          
                                // demais informações, a completar
};

#endif
