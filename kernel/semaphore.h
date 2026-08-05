// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Semáforos e spinlocks

#ifndef __PPOS_SEMAPHORE__
#define __PPOS_SEMAPHORE__

// inicia o subsistema de semáforos
// (chamada pelo núcleo na inicialização).
void sem_init();

// encerra o subsistema de semáforos
// (chamada pelo núcleo no encerramento).
void sem_term();

// trava um spin-lock (busy wait)
void spin_lock(int *lock);

// libera um spin-lock
void spin_unlock(int *lock);

// Cria um novo semáforo, inicializado com value >= 0.
// Retorno: descritor do semáforo ou -1 (erro).
int sem_create(int value);

// destrói um semáforo, liberando recursos e tarefas bloqueadas
// Retorno: NOERROR (0) ou ERROR (<0)
int sem_destroy(int sem_id);

// Requisita acesso a um semáforo
// Retorno: NOERROR (0) ou ERROR (<0)
int sem_down(int sem_id);

// libera o acesso a um semáforo
// Retorno: NOERROR (0) ou ERROR (<0)
int sem_up(int sem_id);

#endif
