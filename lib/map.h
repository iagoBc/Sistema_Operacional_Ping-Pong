// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 07/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

/*
TAD mapa de objetos, que registra objetos de tipo "void *". Um mapa de tamanho
SIZE registra cada objeto sob um identificador único (ID), que varia entre 0
e SIZE-1 de forma crescente (0, 1, 2, ...). Ao registrar um objeto deve ser
usado o próximo ID livre, com comportamento circular caso não existam mais
IDs livres no final do mapa.
*/

#ifndef __MAP__
#define __MAP__

#define ERROR -1
#define NOERROR 0

// Struct opaco que representa um mapa de objetos.
struct map_t;

// Cria um mapa para até N objetos, com IDs entre 0 e N-1.
// Retorno: ponteiro para o mapa ou NULL (erro)
struct map_t *map_create(int size);

// destrói um mapa existente (mas não destrói os objetos).
// Retorno: 0 em sucesso ou -1 (erro)
int map_destroy(struct map_t *map);

// Registra um objeto no mapa, retornando seu ID.
// Retorno: ID atribuído ao objeto ou -1 (erro).
int map_put(struct map_t *map, void *object);

// Informa o objeto registrado no ID indicado do mapa.
// Retorno: ponteiro para o objeto ou NULL (erro)
void *map_get(struct map_t *map, int id);

// Libera um ID do mapa e devolve o objeto associado.
// Retorno: ponteiro para o objeto ou NULL (erro)
void *map_del(struct map_t *map, int id);

// Informa o número de objetos registrados no mapa.
// Retorno: número de objetos registrados ou -1 (erro)
int map_items(struct map_t *map);

// Informa o número de objetos que o mapa pode registrar.
// Retorno: número de objetos ou -1 (erro)
int map_size(struct map_t *map);

// Imprime o conteúdo do mapa, no seguinte formato:
// Mapa nulo:     nome: undef
// Mapa vazio:    nome: [ - - - - - ] (0/5)
// Mapa qualquer: nome: [ - * - * * ] (3/5)
// As posições "*" no vetor impresso correspondem aos IDs em uso no mapa;
// (3/5) indica que o mapa tem 3 objetos e pode registrar até 5 objetos.
void map_print(char *name, struct map_t *map);

#endif
