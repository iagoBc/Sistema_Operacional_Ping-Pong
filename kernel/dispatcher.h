// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Implementa o dispatcher e operações internas sobre as tarefas.

#ifndef __PPOS_DISPATCHER__
#define __PPOS_DISPATCHER__

#include "lib/queue.h"
#include "tcb.h"

// inicia o subsistema dispatcher
// (chamada pelo núcleo na inicialização).
void dispatcher_init();

// encerra o subsistema dispatcher
// (chamada pelo núcleo no encerramento).
void dispatcher_term();

// executa o dispatcher (chamada pelo núcleo após a inicialização).
void dispatcher();

// transfere a CPU da tarefa atual para outra tarefa; se task_id == 0, 
// transfere para o núcleo. Ignora sem erro se "task" já tiver terminado.
// Retorno: NOERROR (0) ou ERROR (<0)
int task_switch(struct task_t *task);

// executa a tarefa indicada: retira-a da fila de prontas, muda seu status
// para RODANDO e transfere a CPU para ela.
void task_run(struct task_t *task);

// suspende a tarefa atual: retira-a da fila de prontas, muda seu status para
// SUSPENSA, a insere na fila "queue" (se não for NULL) e retorna ao dispatcher.
void task_suspend(struct queue_t *queue);

// acorda uma tarefa: retira-a da fila onde se encontra suspensa (se estiver
// em uma fila), muda seu status para PRONTA e a insere na fila de prontas,
// para retomar (ou iniciar) sua execução.
void task_awake(struct task_t *task);

#endif
