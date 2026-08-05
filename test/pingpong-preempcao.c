// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste da preempção por tempo (quantum)

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

#define WORKLOAD 10000

struct task_t *pang, *peng, *ping, *pong, *pung;

// simula um processamento pesado
int hardwork(int n)
{
    int soma = 0;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            soma += j;
    return (soma);
}

// corpo das tarefas
void body(void *arg)
{
    printk("%s: inicio\n", (char *)arg);
    for (int i = 0; i < 10; i++)
    {
        printk("%s: %d\n", (char *)arg, i);
        hardwork(WORKLOAD);
    }
    printk("%s: fim\n", (char *)arg);
    task_exit(NOERROR);
}

// corpo da tarefa principal
void user_main(void *arg)
{
    int id;
    char *name;

    name = task_name(NULL);
    id   = task_id(NULL);
    printk("%5u ms: %s (id %d): inicio\n", time(), name, id);

    // cria tarefas
    pang = task_create("pang", body, "\tPang");
    assert(pang);
    peng = task_create("peng", body, "\t\tPeng");
    assert(peng);
    ping = task_create("ping", body, "\t\t\tPing");
    assert(ping);
    pong = task_create("pong", body, "\t\t\t\tPong");
    assert(pong);
    pung = task_create("pung", body, "\t\t\t\t\tPung");
    assert(pung);

    printk("%5u ms: %s fim\n", time(), name);

    task_exit(NOERROR);
}
