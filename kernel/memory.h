// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Alocador básico de memória heap.

#ifndef __PPOS_MEMORY__
#define __PPOS_MEMORY__

// inicia o subsistema de memória RAM (heap)
// (chamada pelo núcleo na inicialização).
void mem_init();

// encerra o subsistema de memória RAM (heap)
// (chamada pelo núcleo no encerramento).
void mem_term();

// informa a quantidade de memória total, em bytes
int mem_size();

// informa a quantidade de memória disponível, em bytes
int mem_avail();

// aloca um bloco de memória com o tamanho indicado
// retorna ponteiro ou NULL se houver erro
void *mem_alloc(int size);

// libera um bloco de memória previamente alocado
// retorna NOERROR se ok ou ERROR se ptr for NULL ou inválido
int mem_free(void *ptr);

// gera um relatório sobre o uso da memória
void mem_report();

#endif
