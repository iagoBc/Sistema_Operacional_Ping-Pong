// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste das operações de alocação de memória heap

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

#define MAX_BLOCKS (1024 * 1024)

static unsigned char *block[MAX_BLOCKS];
static int bsize[MAX_BLOCKS];

// corpo da tarefa principal
void user_main()
{
    int size, nblocks, avail, avail_before, avail_after;
    void *ptr;
    int status, id;
    char *name;

    name = task_name(NULL);
    id   = task_id(NULL);
    printk("%5u ms: %s (id %d): inicio\n", time(), name, id);

    // testa mem_size
    printk("Testando mem_size\n");
    size = mem_size();
    printk("mem_size  retornou %d bytes\n", size);
    assert(size > 0);

    // testa mem_avail
    printk("Testando mem_avail\n");
    avail = mem_avail();
    printk("mem_avail retornou %d bytes\n", avail);
    assert(avail > 0);
    assert(avail <= size);

    // testa alocação de tamanho zero
    printk("Testando mem_alloc com tamanho 0\n");
    ptr = mem_alloc(0);
    assert(ptr == NULL);

    // testa alocação negativa
    printk("Testando mem_alloc com tamanho negativo\n");
    ptr = mem_alloc(-100);
    assert(ptr == NULL);

    // testa alocação maior que a memória disponível
    printk("Testando mem_alloc tamanho excessivo\n");
    avail = mem_avail();
    ptr = mem_alloc(avail + 1);
    assert(ptr == NULL);

    // testa free nulo
    printk("Testando mem_free com ponteiro nulo\n");
    status = mem_free(NULL);
    assert(status == ERROR);

    // testa free inválido
    printk("Testando mem_free com ponteiro inválido\n");
    status = mem_free(&status);
    assert(status == ERROR);

    // alocações aleatórias
    printk("Criando blocos de tamanho aleatório\n");
    avail_before = mem_avail();
    nblocks = 0;
    do
    {
        // aloca um bloco de tamanho aleatório
        size = randnum() % (mem_size() / 1024);
        ptr = mem_alloc(size);

        if (ptr)
        {
            // guarda o bloco alocado
            block[nblocks] = ptr;
            bsize[nblocks] = size;
            nblocks++;

            // ao chegar em 10 blocos, gera relatório
            if (nblocks == 10)
            {
                printk("Testando mem_report (10 alocações)\n");
                mem_report();
            }
        }
    } while (mem_avail() > 4096);

    printk("Foram alocados %d blocos\n", nblocks);

    // testa se alocações estão alinhadas em 16 bytes
    printk("Testando alinhamento dos blocos alocados\n");
    for (int i = 0; i < nblocks; i++)
        assert(((unsigned long)block[i] % 16) == 0);

    // preenche os blocos alocados
    printk("Preenchendo blocos com bytes fixos\n");
    for (int i = 0; i < nblocks; i++)
    {
        // preenche bloco com valor fixo
        for (int j = 0; j < bsize[i]; j++)
            block[i][j] = i % 256;
    }

    // verifica o conteúdo dos blocos alocados
    printk("Verificando conteúdo dos blocos preenchidos\n");
    for (int i = 0; i < nblocks; i++)
    {
        // verifica o conteúdo do bloco
        for (int j = 0; j < bsize[i]; j++)
            assert(block[i][j] == i % 256);
    }

    // liberando os blocos alocados acima
    printk("Liberando os blocos alocados\n");
    for (int i = 0; i < nblocks; i++)
    {
        status = mem_free(block[i]);
        assert(status == NOERROR);

        // ao chegar aos últimos N blocos, gera relatório
        if (nblocks - (i + 1) == 10)
        {
            printk("Testando mem_report (10 alocações)\n");
            mem_report();
        }
    }

    // verificando se toda a memória foi liberada
    printk("Verificando a memória disponível\n");
    avail_after = mem_avail();
    printk("mem_avail retornou %d bytes\n", avail_after);
    assert(avail_after == avail_before);
    printk("Testando mem_report (sem alocações do usuário)\n");
    mem_report();

    printk("Testes concluídos com sucesso!\n");

    printk("%5u ms: %s fim\n", time(), name);

    task_exit(NOERROR);
}
