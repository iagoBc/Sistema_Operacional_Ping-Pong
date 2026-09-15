// GRR20244409 Iago Cardoso Bariuka
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
#define SUSPENDED 4

#define USER 1
#define SYSTEM 2

// Task Control Block (TCB), infos sobre uma tarefa
struct task_t{
    int id;                                 // identificador da tarefa
    char *name;                             // nome da tarefa
    struct ctx_t context;                   // contexto da tarefa
    char state;                             // pronta, executando, finalizada ...
    void *stack;                            // ponteiro para a pilha da tarefa
    struct task_t *parent;                  // ponteiro para a tarefa pai
    int vg_id;		                        // ID da pilha da tarefa no Valgrind
    int static_prio;                        // prioridade estática da tarefa
    int dynamic_prio;                       // prioridade dinâmica da tarefa          
    int quantum;                            // quantum da tarefa
    int type;                               // tipo da tarefa (USER ou SYSTEM)
    int cpu;                                // tempo de CPU usado pela tarefa
    int run;                                // tempo de vida da tarefa
    int acts;                               // número de ativações da tarefa
    int exit;                               // código de saída da tarefa                            
                                            // demais informações, a completar
};

#endif
