// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste das operações de acesso a disco com múltiplas tarefas
// fazendo operações de leitura e escrita simultâneas.

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

#define NUMTASKS 8

// tarefas
struct task_t *mover[NUMTASKS];     // tarefas movedoras de blocos
static int num_blk;             // numero de blocos no disco
static int blk_size;            // tamanho de cada bloco (bytes)

// lê um bloco do disco
void le_bloco(int task, int block, char *buffer)
{
    int status;

    printk("%5u ms: T%-2d lendo bloco %3d\n", time(), task, block);
    status = block_read(block, buffer);
    if (status == 0)
        printk("%5u ms: T%-2d leu   bloco %3d\n", time(), task, block);
    else
        printk("%5u ms: T%-2d erro ao ler bloco %3d\n", time(), task, block);
}

// escreve um bloco no disco
void escreve_bloco(int task, int block, char *buffer)
{
    int status;

    printk("%5u ms: T%-2d escrevendo bloco %3d\n", time(), task, block);
    status = block_write(block, buffer);
    if (status == 0)
        printk("%5u ms: T%-2d escreveu   bloco %3d\n", time(), task, block);
    else
        printk("%5u ms: T%-2d erro ao escrever bloco %3d\n",
               time(), task, block);
}

// mostra o conteudo do bloco
void mostra_bloco(int task, int block, char *buffer)
{
    printk("%5u ms: T%-2d bloco %3d tem: [", time(), task, block);
    for (int j = 0; j < blk_size; j++)
        printk("%c", buffer[j]);
    printk("]\n");
}

// corpo das tarefas "mover"
void body(void *arg)
{
    int my_id, blk_orig, blk_dest;
    int tid, blk_per_task;
    char *buf1, *buf2;

    tid = task_id(NULL);
    blk_per_task = (num_blk / NUMTASKS / 2);

    // aloca espaço para os buffers
    buf1 = mem_alloc(blk_size);
    assert(buf1);
    buf2 = mem_alloc(blk_size);
    assert(buf2);

    // define os blocos iniciais
    my_id = (long)arg;
    blk_orig = my_id * blk_per_task;
    blk_dest = num_blk - 1 - (my_id * blk_per_task);

    printk("%5u ms: T%-2d movendo %2d blocos entre blocos %3d e %3d\n",
           time(), tid, blk_per_task, blk_orig, blk_dest);

    // move blk_per_task blocos
    for (int i = 0; i < blk_per_task; i++)
    {
        // lê dois blocos do disco
        le_bloco(tid, blk_orig, buf1);
        le_bloco(tid, blk_dest, buf2);

        // mostra o conteudo dos blocos
        mostra_bloco(tid, blk_orig, buf1);
        mostra_bloco(tid, blk_dest, buf2);

        // escreve os blocos trocados no disco
        escreve_bloco(tid, blk_dest, buf1);
        escreve_bloco(tid, blk_orig, buf2);

        // define os próximos blocos
        blk_orig++;
        blk_dest--;
    }
    printk("%5u ms: T%-2d terminou\n", time(), tid);

    mem_free(buf1);
    mem_free(buf2);
    task_exit(NOERROR);
}

// corpo da tarefa principal
void user_main(void *arg)
{
    int status, id;
    char *name;

    name = task_name(NULL);
    id   = task_id(NULL);
    printk("%5u ms: %s (id %d): inicio\n", time(), name, id);

    // busca geometria do disco
    num_blk = block_blocks();
    assert(num_blk);
    blk_size = block_size();
    assert(blk_size);

    printk("%5u ms: disco contem %d blocos de %d bytes cada\n",
           time(), num_blk, blk_size);

    // cria as tarefas
    for (long i = 0; i < NUMTASKS; i++)
    {
        mover[i] = task_create(NULL, body, (void *)i);
        assert(mover[i]);
    }

    // aguarda as tarefas encerrarem
    for (int i = 0; i < NUMTASKS; i++)
    {
        status = task_wait(mover[i]);
        assert(status == NOERROR);
    }

    // destrói as tarefas
    for (int i = 0; i < NUMTASKS; i++)
    {
        status = task_destroy(mover[i]);
        assert(status == NOERROR);
    }

    printk("%5u ms: %s fim\n", time(), name);

    task_exit(NOERROR);
}
