// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Gerência de filas de mensagens

#ifndef __PPOS_MQUEUE__
#define __PPOS_MQUEUE__

// inicia o subsistema de filas de mensagens
// (chamada pelo núcleo na inicialização).
void mqueue_init();

// encerra o subsistema de filas de mensagens
// (chamada pelo núcleo no encerramento).
void mqueue_term();

// cria uma fila de mensagens.
// Retorno: descritor da nova fila ou ERROR
int mqueue_create(int max_msgs, int msg_size);

// destrói uma fila de mensagens, liberando recursos e tarefas
// Retorno: NOERROR ou ERROR
int mqueue_destroy(int mqueue_id);

// envia uma mensagem
// Retorno: NOERROR ou ERROR
int mqueue_send(int mqueue_id, void *msg);

// recebe uma mensagem
// Retorno: NOERROR ou ERROR
int mqueue_recv(int mqueue_id, void *msg);

// retorna o numero de mensagens em uma fila
// Retorno: número >= 0 ou ERROR
int mqueue_msgs(int mqueue_id);

#endif
