// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste da função task_wait() (leve)

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

#define WORKLOAD 20000

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
    int max, tid;

    tid = task_id(NULL);
    max = tid * 2;

    printk("%5u ms: %s inicia\n", time(), (char *)arg);
    for (int i = 0; i < max; i++)
    {
        printk("%5u ms: %s %d\n", time(), (char *)arg, i);
        hardwork(WORKLOAD);
    }
    printk("%5u ms: %s termina\n", time(), (char *)arg);

    task_exit(tid);
}

// corpo da tarefa principal
void user_main(void *arg)
{
    int status, id;
    char *name;

    name = task_name(NULL);
    id   = task_id(NULL);
    printk("%5u ms: %s (id %d): inicio\n", time(), name, id);

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

    for (int i = 0; i < 2; i++)
    {
        printk("%5u ms: %s %d\n", time(), name, i);
        hardwork(WORKLOAD);
    }

    printk("%5u ms: %s esperando Pang...\n", time(), name);
    status = task_wait(pang);
    assert(status == task_id(pang));

    printk("%5u ms: %s esperando Peng...\n", time(), name);
    status = task_wait(peng);
    assert(status == task_id(peng));

    printk("%5u ms: %s esperando Ping...\n", time(), name);
    status = task_wait(ping);
    assert(status == task_id(ping));

    printk("%5u ms: %s esperando Pong...\n", time(), name);
    status = task_wait(pong);
    assert(status == task_id(pong));

    printk("%5u ms: %s esperando Pung...\n", time(), name);
    status = task_wait(pung);
    assert(status == task_id(pung));

    // destrói as tarefas
    status = task_destroy(pang);
    assert(status == NOERROR);
    status = task_destroy(peng);
    assert(status == NOERROR);
    status = task_destroy(ping);
    assert(status == NOERROR);
    status = task_destroy(pong);
    assert(status == NOERROR);
    status = task_destroy(pung);
    assert(status == NOERROR);

    printk("%5u ms: %s fim\n", time(), name);

    task_exit(NOERROR);
}
