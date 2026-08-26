// GRR20244409 Iago Cardoso Bariuka
// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Alocador básico de memória heap.

// somente para a implementação trivial
#include <stdlib.h>
#define ERROR -1
#define NOERROR 0

// implementação trivial, a ser substituída
void *mem_alloc(int size)
{
    return (malloc(size));
}

// implementação trivial, a ser substituída
int mem_free(void *addr)
{
    free(addr);
    return (NOERROR);
}

void mem_init()
{
}

void mem_term()
{
}

