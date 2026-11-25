// GRR20244409 Iago Cardoso Bariuka
// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

#include "lib/pplibc.h"

#define HEAP_SIZE (64 * 1024 * 1024)                                                    // Tamanho total da heap: 64 MB
static unsigned char heap[HEAP_SIZE] __attribute__((aligned(16)));                      // Memória heap alinhada a 16 bytes

typedef unsigned int size_t;                                                            

#define MAX_BLOCKS 8192
#define NO_BLOCK -1
#define BLOCK_FREE 0
#define BLOCK_ALLOCATED 1

typedef struct{
    unsigned char *init;                                                                // Endereço inicial do bloco
    size_t size;                                                                        // Tamanho do bloco em bytes
    int state;                                                                          // Estado do bloco (livre ou alocado)
    int left;                                                                           // Número do bloco à esquerda
    int right;                                                                          // Número do bloco à direita
} descriptor;                                                                           // Descritor de bloco de memória

static descriptor blocks[MAX_BLOCKS];                                                   // Lista de blocos de memória

// inicia o subsistema de memória RAM (heap)
// (chamada pelo núcleo na inicialização).
void mem_init(){
    for(int i = 0; i < MAX_BLOCKS; i++){
        blocks[i].init = NULL;
        blocks[i].size = 0;
        blocks[i].state = BLOCK_FREE;
        blocks[i].left = NO_BLOCK;
        blocks[i].right = NO_BLOCK;
    }

    blocks[0].init = heap;                                                              // Inicializa o primeiro bloco com o endereço da heap
    blocks[0].size = HEAP_SIZE;                                                         // Define o tamanho do primeiro bloco como o tamanho total da heap
    blocks[0].state = BLOCK_FREE;                                                       
    blocks[0].left = 0;
    blocks[0].right = 0;
}

// encerra o subsistema de memória RAM (heap)
// (chamada pelo núcleo no encerramento).
void mem_term(){
}

// informa a quantidade de memória total, em bytes
int mem_size(){
    return HEAP_SIZE;
}

// informa a quantidade de memória disponível, em bytes
int mem_avail(){
    if(!blocks[0].init) return 0;

    size_t qt_mem = 0;                                                                  // Quantidade de memória disponível
    int current = 0;                                                                    // Índice do bloco atual
    do{
        if(blocks[current].state == BLOCK_FREE)                                         // Se o bloco estiver livre
            qt_mem += blocks[current].size;
        current = blocks[current].right;                                                // Vai para o próximo bloco
    } while(current != 0);

    return (int)qt_mem;
}

// aloca um bloco de memória com o tamanho indicado
// retorna ponteiro ou NULL se houver erro
void *mem_alloc(int size){
    // 1. Rejeita tamanhos inválidos e ajusta o pedido ao próximo múltiplo de 16.
    if(size <= 0 || size > HEAP_SIZE) return NULL;

    size_t aligned_size = (((size_t)size - 1U) | 0x00000FU) + 1U;
    if(aligned_size > HEAP_SIZE || !blocks[0].init) return NULL;

    // 2. Percorre os blocos na ordem da lista e escolhe o primeiro livre que caiba.
    int current = 0;
    do{
        if(blocks[current].state == BLOCK_FREE 
            && blocks[current].size >= aligned_size)                                    // Se o bloco estiver livre e tiver tamanho suficiente
            break;
        current = blocks[current].right;
    } while(current != 0);

    // 3. Se nenhum bloco servir, a alocação falha.
    if(blocks[current].state != BLOCK_FREE ||
        blocks[current].size < aligned_size)                                            // Se o bloco não estiver livre ou não tiver tamanho suficiente
        return NULL;

    // 4. Se sobrar espaço, cria um descritor para o restante livre e ajusta os vínculos.
    if(blocks[current].size > aligned_size){
        int remainder = NO_BLOCK;                                                       
        for(int i = 0; i < MAX_BLOCKS; i++){
            if(!blocks[i].init){                                                        // Se o bloco não estiver inicializado
                remainder = i;                                                          // Armazena o índice do bloco restante
                break;
            }
        }

        if(remainder == NO_BLOCK) return NULL;

        int old_right = blocks[current].right;                                          // Armazena o índice do bloco à direita do bloco atual
        blocks[remainder].init = blocks[current].init + aligned_size;                   
        blocks[remainder].size = blocks[current].size - aligned_size;                   // Define o tamanho do bloco restante como o tamanho do bloco atual - tamanho alinhado
        blocks[remainder].state = BLOCK_FREE;
        blocks[remainder].left = current;                                               
        blocks[remainder].right = old_right;                                            // Define o bloco à direita do bloco restante como o bloco à direita do bloco atual
        blocks[old_right].left = remainder;                                             // Define o bloco à esquerda do bloco à direita do bloco restante como o bloco restante
        blocks[current].right = remainder;                                              // Define o bloco à direita do bloco atual como o bloco restante
        blocks[current].size = aligned_size;                                            // Define o tamanho do bloco atual como o tamanho alinhado
    }

    // 5. Marca o bloco como ocupado.
    blocks[current].state = BLOCK_ALLOCATED;

    // 6. Retorna o endereço inicial do bloco alocado.
    return blocks[current].init;
}

// libera um bloco de memória previamente alocado
// retorna NOERROR se ok ou ERROR se ptr for NULL ou inválido
int mem_free(void *ptr){
    if(!ptr || !blocks[0].init) return ERROR;

    // 1. Localiza o descritor pelo endereço inicial exato do bloco.
    int current = 0;
    do{
        if(blocks[current].init == ptr) break;
        current = blocks[current].right;
    } while(current != 0);

    // 2. Um endereço que não corresponde a um bloco ativo é inválido.
    if(blocks[current].init != ptr) return ERROR;

    // 3. Não permite liberar um bloco que já está livre.
    if(blocks[current].state == BLOCK_FREE) return ERROR;

    // 4. Marca o bloco como livre.
    blocks[current].state = BLOCK_FREE;

    // 5. Funde com blocos livres contíguos à esquerda.
    while(blocks[current].left != current){
        int left = blocks[current].left;
        if(blocks[left].state != BLOCK_FREE ||
            blocks[left].init + blocks[left].size != blocks[current].init)              // Se o bloco à esquerda não estiver livre ou não for contíguo, sai do loop
            break;

        int right = blocks[current].right;
        blocks[left].size += blocks[current].size;                                      // Soma o tamanho do bloco atual ao bloco à esquerda
        blocks[left].right = right;                                                     // Define o bloco à direita do bloco à esquerda como o bloco à direita do bloco atual
        blocks[right].left = left;                                                      // Define o bloco à esquerda do bloco à direita como o bloco à esquerda 

        blocks[current].init = NULL;
        blocks[current].size = 0;
        blocks[current].left = NO_BLOCK;
        blocks[current].right = NO_BLOCK;
        current = left;                                                                 // Atualiza o bloco atual para o bloco à esquerda
    }

    // 6. Funde recursivamente com blocos livres contíguos à direita.
    while(blocks[current].right != current){
        int right = blocks[current].right;
        if (blocks[right].state != BLOCK_FREE ||
            blocks[current].init + blocks[current].size != blocks[right].init)          // Se o bloco à direita não estiver livre ou não for contíguo, sai do loop
            break;

        int after = blocks[right].right;                                                
        blocks[current].size += blocks[right].size;                                     // Soma o tamanho do bloco à direita ao bloco atual
        blocks[current].right = after;                                                  // Define o bloco à direita do bloco atual como o bloco à direita do bloco à direita do bloco atual
        blocks[after].left = current;                                                   // Define o bloco à esquerda do bloco à direita do bloco atual como o bloco atual

        blocks[right].init = NULL;
        blocks[right].size = 0;
        blocks[right].left = NO_BLOCK;
        blocks[right].right = NO_BLOCK;
    }

    return NOERROR;
}

// gera um relatório sobre o uso da memória
void mem_report(){
    if(!blocks[0].init) return;

    size_t allocated_size = 0;                                                          
    size_t free_size = 0;                                                               
    unsigned int allocated_blocks = 0;                                                  
    unsigned int free_blocks = 0;                                                           

    int current = 0;
    do{
        // Conta o tamanho e o número de blocos alocados e livres
        if(blocks[current].state == BLOCK_FREE){                                        
            free_size += blocks[current].size;
            free_blocks++;
        }
        // Conta o tamanho e o número de blocos alocados 
        else{
            allocated_size += blocks[current].size;
            allocated_blocks++;
        }
        current = blocks[current].right;
    } while(current != 0);

    printk("heap: %u KB allocated (%u blocks), %u KB free (%u blocks)\n",
           (unsigned int)(allocated_size / 1024),
           allocated_blocks,
           (unsigned int)(free_size / 1024),
           free_blocks);

    current = 0;
    do{
        unsigned char *start = blocks[current].init;                                    // Endereço inicial do bloco
        unsigned char *end = start + blocks[current].size - 1;                          // Endereço final do bloco
        const char *status = blocks[current].state == BLOCK_FREE ? "FREE" : "aloc";     // Estado do bloco (livre ou alocado)

        printk("heap: block %5d: %p - %p %s prev %5d next %5d size %u\n",
               current,
               (unsigned long)start,
               (unsigned long)end,
               status,
               blocks[current].left,
               blocks[current].right,
               blocks[current].size);
        current = blocks[current].right;
    } while(current != 0);
}