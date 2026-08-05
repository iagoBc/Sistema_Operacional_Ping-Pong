// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

/*
TAD fila genérica de ponteiros, que armazena itens de tipo "void *".
A fila mantém um iterador que aponta para um item da fila (ou para NULL).
O iterador pode ser ajustado usando as funções queue_head() e queue_next()
e ter seu item consultado com a função queue_item(). Inicialmente o iterador
aponta para o começo da fila.
*/

#ifndef __QUEUE__
#define __QUEUE__

#include <stdbool.h>

// códigos de retorno das funções
#ifndef ERROR
#define ERROR -1
#define NOERROR 0
#endif

// ponteiro nulo
#ifndef NULL
#define NULL ((void *)0)
#endif

// struct que representa uma fila; este é um struct "opaco",
// que deve ser redefinido completamente em queue.c
struct queue_t;

// Cria uma fila inicialmente vazia.
// Retorno: ponteiro p/ a nova fila
//          NULL se houver erro
struct queue_t *queue_create();

// Destroi uma fila, liberando a memória alocada por ela.
// IMPORTANTE: os itens apontados pela fila NÃO devem ser liberados,
// pois a aplicação que os criou e pôs na fila é responsável por eles.
// Retorno: NOERROR ou ERROR (se a fila não existir)
int queue_destroy(struct queue_t *queue);

// Adiciona um item no fim da fila; ajusta o iterador para ele
// se for o primeiro item (ou seja, se a fila estiver vazia).
// Retorno: NOERROR ou ERROR (se fila ou item não existir)
int queue_add(struct queue_t *queue, void *item);

// Retira da fila o item com o valor indicado; se o item estiver
// em mais de uma posição da fila, retira apenas da primeira posição
// encontrada; se o item estiver apontado pelo iterador, este avança
// para o próximo item da fila (ou para NULL, se for o último).
// Retorno: NOERROR ou ERROR (não encontrou ou outro erro).
int queue_del(struct queue_t *queue, void *item);

// Informa se o item indicado está na fila.
// Retorno: true/false (error: false).
bool queue_has(struct queue_t *queue, void *item);

// Informa o número de itens na fila.
// Retorno: número de itens na fila (>= 0)
//          ERROR se a fila não existir
int queue_size(struct queue_t *queue);

// Põe o iterador no início da fila.
// Retorno: ptr para o item apontado pelo iterador
//          NULL se a fila estiver vazia ou não existir
void *queue_head(struct queue_t *queue);

// Avança o iterador ao próximo item na fila.
// Retorno: ptr para o item apontado pelo iterador após avançar
//          NULL se o iterador passou do último item da fila
//          NULL se a fila estiver vazia ou não existir
void *queue_next(struct queue_t *queue);

// Informa o item atualmente sob o iterador na fila.
// Retorno: ptr para o item apontado pelo iterador
//          NULL se a fila estiver vazia ou não existir
//          NULL se o iterador passou do fim da fila
void *queue_item(struct queue_t *queue);

// Imprime os elementos de uma fila; a função externa "func"
// deve ser chamada para imprimir cada item.
// Exemplos de saída, com name == "Frutas":
// Frutas: [ banana pera ameixa uva ] (4 items)
// Frutas: [ ] (0 items)
// Frutas: undef   se queue == NULL
// Frutas: [ undef undef undef ] (3 items)  se func == NULL
void queue_print(char *name, struct queue_t *queue, void(func)(void *));

#endif
