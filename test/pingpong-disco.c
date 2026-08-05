// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste das operações de acesso a disco com uma única tarefa,
// que lê e escreve/altera todos os blocos do disco.

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

static int num_blk;     // numero de blocos no disco
static int blk_size;    // tamanho de cada bloco (bytes)

// corpo da tarefa principal
void user_main(void *arg)
{
    int status, id;
    char *name;
    unsigned char c, *buffer;

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

    // aloca o buffer para ler blocos do disco
    buffer = mem_alloc(blk_size);
    assert(buffer);

    // lê e imprime todos os blocos do disco, um a um
    for (int i = 0; i < num_blk; i++)
    {
        // lê o bloco i do disco no buffer
        printk("%5u ms: lendo bloco %d\n", time(), i);
        status = block_read(i, buffer);
        if (status)
            printk("Erro ao ler bloco %d!\n", i);

        // mostra o conteudo do buffer
        printk("%5u ms: buffer: [", time());
        for (int j = 0; j < blk_size; j++)
            printk("%c", buffer[j]);
        printk("]\n");
    }

    // inicia gerador de números aleatórios
    randseed(time());

    // lê e imprime todos os blocos do disco, um a um
    for (int i = 0; i < num_blk; i++)
    {

        // preenche o buffer com um caractere aleatório
        c = 32 + randnum() % 48;
        for (int j = 0; j < blk_size; j++)
            buffer[j] = c;

        // escreve o buffer no bloco i do disco
        printk("%5u ms: escrevendo bloco %d com caracteres \"%c\"\n",
               time(), i, c);
        status = block_write(i, buffer);
        if (status)
            printk("Erro ao escrever bloco %d!\n", i);
    }

    // libera o buffer de blocos do disco
    mem_free(buffer);

    printk("%5u ms: %s fim\n", time(), name);

    task_exit(NOERROR);
}
