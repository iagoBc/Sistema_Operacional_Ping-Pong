// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// Este arquivo PODE/DEVE ser alterado.

// Implementação do TAD fila genérica
#include "queue.h"
#include "pplibc.h"
#include "kernel/memory.h"

typedef struct No{
    void *dado;
    struct No *prox;
} No;

typedef struct queue_t{
    No *head;
    No *it;
    No *tail;
    int tam;
} queue_t;

struct queue_t *queue_create(){
    queue_t *fila = (queue_t*) mem_alloc(sizeof(queue_t)); // Aloca dinamicamente memoria para struct

    if(!fila) return NULL; // Erro ao alocar memoria para struct
    
    else{ // Inicia os valores dos nos e o tamanho = 0
        fila->head = NULL;
        fila->it = NULL;
        fila->tail = NULL;
        fila->tam = 0;
        return fila;
    }
}

int queue_destroy(struct queue_t *queue){
    if(!queue) return ERROR; // Fila inexistente retorna ERROR
    
    No *prox = NULL;

    while(queue->head){ // Enquanto tiver um no na fila
        prox = queue->head->prox;
        mem_free(queue->head);
        queue->head = prox; // Nova cabeca da fila
    } 

    mem_free(queue); // Libera memoria 

    return NOERROR;
}

int queue_add(struct queue_t *queue, void *item){
    if(!queue || !item) return ERROR; // Retorna ERROR se a fila ou item nao existe

    No* no = (No*) mem_alloc(sizeof(No)); // Aloca memoria para o no

    no->prox = NULL; // Sempre o proximo no = NULL
    no->dado = item; // O valor dentro do no = item

    if(!queue->tam){ // Se a fila esta vazia
        queue->head = queue->tail = queue->it = no;
    }

    else{ 
        queue->tail->prox = no; // Relaciona o valor da calda com o novo no
        queue->tail = no; // Nova calda
    }

    queue->tam++; // Incrementa o tamanho da fila

    return NOERROR;
}

int queue_del(struct queue_t *queue, void *item){
    if(!queue || !item) return ERROR; // Retorna ERROR se a fila ou item nao existe

    No* no = queue->head; // Comeca pela cabeca da fila 
    No* anterior = NULL; // No anterior da cabeca da fila 

    while(no && no->dado != item){ // Enquanto dado do no != item e no != NULL
        anterior = no; // Anterior guarda o valor do no
        no = no->prox; // No vira o proximo no da fila
    }

    if(!no) return ERROR; // Se no = NULL nao existe o item na fila ou a fila = vazia

    if(queue->tam == 1) queue->head = queue->it = queue->tail = NULL; // Se a fila tem apenas um no
    else if(queue->head == no) queue->head = no->prox; // Se o no a ser deletado = cabeca da fila
    else anterior->prox = no->prox; // Deleta o no e liga o no anterior com o proximo no

    if(!no->prox) queue->tail = anterior; // Se o no deletado = calda da fila, atualiza a calda
    if(queue->it == no) queue->it = no->prox; // Se o no deletado esta apontado pelo iterador, o iterador vai para o prox no

    mem_free(no); // Libera o no

    queue->tam--; // Diminui o tamanho da fila

    return NOERROR;
}

bool queue_has(struct queue_t *queue, void *item){
    if(!queue || !item) return false; // Retorna ERROR se a fila ou item nao existe

    No *no = queue->head; // Comeca pela cabeca da fila
    while(!no && no->dado != item) no = no->prox; // Loop ate achar o no com dado procurado ou ate no ser nulo

    if(!no) return false; // Retorna falso se no == NULL
    else return true; // True se encontrou o item
}

int queue_size(struct queue_t *queue){
    if(!queue) return ERROR; // Retorna ERROR se a fila nao existe
    else return queue->tam; // Retorna o tamanho da fila
}

void *queue_head(struct queue_t *queue){
    if (!queue || !queue->head) return NULL; // Retorna ERROR se a fila ou item nao existe

    queue->it = queue->head; // Coloca o iterador na cabeca da fila

    return queue->it->dado; // Retorna para o item apontado pelo iterador
}

void *queue_next(struct queue_t *queue){
    if (!queue || !queue->it) return NULL; // Retorna ERROR se a fila ou item nao existe

    queue->it = queue->it->prox; // Avanca o iterador ao proximo no da fila
    if (!queue->it) return NULL; // Se o proximo no == NULL

    return queue->it->dado; // Retorna para o item apontado pelo iterador
}

void *queue_item(struct queue_t *queue){
    if(!queue || !queue->tam || !queue->it) return NULL; // Retorna ERROR se a fila ou item nao existe
    else return queue->it->dado; // Retorna para o item apontado pelo iterador
}

void queue_print(char *name, struct queue_t *queue, void(func)(void *)){
    if(!queue){ // Se a fila nao existe
        printk("%s: undef\n", name);
        return;
    }

    printk("%s: [ ", name);
    No* aux = queue->head;

    while (aux != NULL) {
        if (func == NULL) printk("undef ");
        else printk(aux->dado);
        printk(" ");
        aux = aux->prox;
    }
    printk("] (%d items)\n", queue->tam);
}
