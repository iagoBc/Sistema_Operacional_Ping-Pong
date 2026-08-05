// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Gerência de tarefas.

#ifndef __PPOS_TASK__
#define __PPOS_TASK__

#include "tcb.h"

// inicializa o subsistema de tarefas.
// (chamada pelo núcleo na inicialização).
void task_init();

// encerra o subsistema de tarefas.
// (chamada pelo núcleo no encerramento).
void task_term();

// cria uma nova tarefa: "name" é o nome da tarefa, "entry" é a função que
// ela irá executar e "arg" aponta para o valor recebido por "entry" ao
// iniciar (pode ser NULL).
// Retorno: ptr para a tarefa ou NULL se houver erro.
struct task_t * task_create(char *name, void (*entry)(void *), void *arg);

// destrói uma tarefa e libera seus recursos; somente deve atuar sobre tarefas
// terminadas. Retorno: NOERROR (0) ou ERROR (<0).
int task_destroy(struct task_t *task);

// informa o ID de uma tarefa (ou da tarefa atual se task == NULL)
int task_id(struct task_t *task);

// informa o nome de uma tarefa (ou da tarefa atual se task == NULL)
char *task_name(struct task_t *task);

// a tarefa atual libera a CPU e volta para a fila de prontas; a execução
// retorna ao núcleo/dispatcher.
void task_yield();

// suspende a tarefa atual até que a tarefa task termine; a execução retorna
// ao núcleo/dispatcher. Se a tarefa task já terminou, retorna sem suspender.
// Retorno: exit code tarefa que terminou ou ERROR.
int task_wait(struct task_t *task);

// suspende a tarefa atual por t milissegundos; a execução retorna ao
// núcleo/dispatcher.
void task_sleep(int t);

// encerra a execução da tarefa atual, informando um código de encerramento
// (exit_code); a execução retorna ao núcleo/dispatcher.
void task_exit(int exit_code);

#endif
