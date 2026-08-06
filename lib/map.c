// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// Este arquivo PODE/DEVE ser alterado.

// Implementação do TAD Mapa de objetos

#include "map.h"
#include "pplibc.h"
#include <stdlib.h>

typedef struct map_t{
    void **vetor; // 
    unsigned int tamanho; // Tamanho do mapa
    unsigned int ocupados; // Quantos IDs estao ocupados no mapa
    unsigned int prox; // Proximo ID
}map_t;

struct map_t *map_create(int size){
    if(size <= 0) return NULL; 

    map_t *map = malloc(sizeof(map_t)); // Aloca dinamicamente memoria para struct

    if(!map) return NULL; // Erro ao alocar memoria para struct

    map->vetor = malloc(sizeof(void*) * size); // Aloca dinamicamente memoria para o vetor

    if(!map->vetor){ // Erro ao alocar memoria para o vetor
        free(map);
        return NULL;
    }

    map->tamanho = size; // Tamanho do mapa setado
    map->ocupados = 0; // Ainda nenhum ID esta ocupado
    map->prox = 0; // ID proximo ainda 0
    return map;
}

int map_destroy(struct map_t *map){
    if(!map) return -1; // Retorno -1 indicando erro
    free(map->vetor);  // Libera apenas o vetor
    free(map);          // libera a struct

    return 0; // Retorno 0 indicando sucesso
}

int map_put(struct map_t *map, void *object){
    if(!map || !object) return -1; // Se o mapa ou o objeto sao nulos, retorna -1 erro

    if(map->ocupados < map->tamanho){ // Se o mapa ja possui todos IDs ocupados retorna -1
        while(1){ // Ate achar um ID livre
            if(map->vetor[map->prox] == NULL){ // Se o ID esta livre adiciona o objeto nesse ID
                map->vetor[map->prox] = object;
                map->ocupados++;    // Incrementa ocupados

                int id = map->prox;
                map->prox = (map->prox + 1) % map->tamanho; // Incrementa ou reseta map->prox para 0 se map->prox+1 == map->tamanho
                return id; // Retorna o ID do objeto adicionado
            }
            map->prox = (map->prox + 1) % map->tamanho; // Incrementa ou reseta map->prox para 0 se map->prox+1 == map->tamanho
        }
    }

    else return -1;
}

void *map_get(struct map_t *map, int id){
    if(!map) return NULL; 
    else if(id < 0 || id >= map->tamanho) return NULL; // Retorna NULL se o id < 0 ou  id >= tamanho do mapa 
    else return map->vetor[id]; // Retorna o ponteiro do objeto
}

void *map_del(struct map_t *map, int id){
    if(!map) return NULL;
    else if(id < 0 || id >= map->tamanho) return NULL; // Retorna NULL se o id < 0 ou  id >= tamanho do mapa 
    else if(map->vetor[id] == NULL) return NULL; // Retorna NULL se na posicao id o ponteiro = NULL

    else{
        void *object = map->vetor[id]; // Guarda o objeto para ser retornado
        map->vetor[id] = NULL;         // Na posicao id agora ponteiro = NULL
        map->ocupados--;               // Agora temos mais um ID desocupado
        return object;                 // Retorna o objeto deletado
    }
}

int map_items(struct map_t *map){
    if(!map) return -1;
    return map->ocupados; // Retorna quantos IDs do mapa estao desocupadas
}

int map_size(struct map_t *map){
    if(!map) return -1;
    return map->tamanho; // Retorna o tamanho do mapa
}

void map_print(char *name, struct map_t *map){
    if(!map){
        printk("Mapa nulo:     %s: undef\n", name);
        return;
    }

    printk("%s: [ ", name);
    for(int i = 0; i < map->tamanho; i++){
        if(map->vetor[i] != NULL)
            printk("* "); // Imprime * se o ID esta ocupado
        else
            printk("- "); // Imprime - se o ID esta desocupado
    }
    
    printk("] (%d/%d)\n", map->ocupados, map->tamanho);
}
