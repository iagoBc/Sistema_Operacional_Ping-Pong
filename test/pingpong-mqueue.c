// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste das filas de mensagens

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

// estrutura com vários valores para teste
struct pacote_t
{
    int v[3], prod;
};

// tarefas e filas
struct task_t *prod[3], *agrup, *cons[2];
int fila_inteiros, fila_pacotes;

// corpo da tarefa produtor
void body_prod()
{
    int valor, status;
    char *name;

    name = task_name(NULL);
    printk("%5u ms: %s inicia\n", time(), name);

    for (;;)
    {
        // sorteia um valor inteiro e o envia na fila de valores
        valor = 1 + randnum() % 20;
        status = mqueue_send(fila_inteiros, &valor);
        if (status < 0)
            break;
        printk("%5u ms: %s envia %d\n", time(), name, valor);

        // dorme um intervalo aleatorio
        task_sleep(randnum() % 3000);
    }

    printk("%5u ms: %s termina\n", time(), name);
    task_exit(NOERROR);
}

// corpo da tarefa agrupador
void body_agrup()
{
    struct pacote_t pacote;
    char *name;
    int status;

    name = task_name(NULL);
    printk("%5u ms: \t\t\t%s inicia\n", time(), name);

    for (int i = 0; i < 10; i++)
    {
        // recebe N valores inteiros e os põe no pacote
        pacote.prod = 1;
        for (int j = 0; j < 3; j++)
        {
            status = mqueue_recv(fila_inteiros, &pacote.v[j]);
            assert(status == NOERROR);

            printk("%5u ms: \t\t\t%s recebe %d\n", time(), name,
                   pacote.v[j]);
            pacote.prod *= pacote.v[j];
        }

        // mostra o pacote
        printk("%5u ms: \t\t\t%s envia [%d*%d*%d = %d]\n",
               time(), name, pacote.v[0], pacote.v[1], pacote.v[2],
               pacote.prod);

        // envia o pacote
        status = mqueue_send(fila_pacotes, &pacote);
        assert(status == NOERROR);

        // dorme um intervalo aleatorio
        task_sleep(randnum() % 3000);
    }

    printk("%5u ms: \t\t\t%s termina\n", time(), name);
    task_exit(NOERROR);
}

// corpo da tarefa consumidor
void body_cons()
{
    struct pacote_t pacote;
    char *name;
    int status;

    name = task_name(NULL);
    printk("%5u ms: \t\t\t\t\t\t%s inicia\n", time(), name);

    for (;;)
    {
        // recebe um pacote e o imprime
        status = mqueue_recv(fila_pacotes, &pacote);
        if (status < 0)
            break;
        printk("%5u ms: \t\t\t\t\t\t%s recebe [%d*%d*%d = %d]\n",
               time(), name, pacote.v[0], pacote.v[1], pacote.v[2],
               pacote.prod);

        // dorme um intervalo aleatorio
        task_sleep(randnum() % 3000);
    }

    printk("%5u ms: \t\t\t\t\t\t%s termina\n", time(), name);
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

    // cria as filas de mensagens (5 valores cada)
    fila_inteiros = mqueue_create(5, sizeof(int));
    assert(fila_inteiros >= 0);
    fila_pacotes = mqueue_create(5, sizeof(struct pacote_t));
    assert(fila_pacotes >= 0);

    // cria tarefas
    agrup   = task_create("agrup", body_agrup, NULL);
    assert(agrup);
    cons[0] = task_create("cons0", body_cons, NULL);
    assert(cons[0]);
    cons[1] = task_create("cons1", body_cons, NULL);
    assert(cons[1]);
    prod[0] = task_create("prod0", body_prod, NULL);
    assert(prod[0]);
    prod[1] = task_create("prod1", body_prod, NULL);
    assert(prod[1]);
    prod[2] = task_create("prod2", body_prod, NULL);
    assert(prod[2]);

    // aguarda o agrupador encerrar
    status = task_wait(agrup);
    assert(status == NOERROR);

    // destrói as filas de mensagens
    printk("%5u ms: %s destroi fila_inteiros\n", time(), name);
    status = mqueue_destroy(fila_inteiros);
    assert(status == NOERROR);
    printk("%5u ms: %s destroi fila_pacotes\n", time(), name);
    status = mqueue_destroy(fila_pacotes);
    assert(status == NOERROR);

    // destrói as demais tarefas
    status = task_wait(cons[0]);
    assert(status == NOERROR);
    status = task_wait(cons[1]);
    assert(status == NOERROR);
    status = task_wait(prod[0]);
    assert(status == NOERROR);
    status = task_wait(prod[1]);
    assert(status == NOERROR);
    status = task_wait(prod[2]);

    printk("%5u ms: %s fim\n", time(), name);

    task_exit(NOERROR);
}
