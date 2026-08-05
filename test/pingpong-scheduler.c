// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste do escalonador com prioridades dinâmicas

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

struct task_t *pang, *peng, *ping, *pong, *pung;

// corpo das tarefas
void body(void *arg)
{
    printk("%s: inicio (prioridade %d)\n",
           (char *)arg, sched_getprio(NULL));

    for (int i = 0; i < 10; i++)
    {
        printk("%s: %d\n", (char *)arg, i);
        task_yield();
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

    // ajusta prioridades
    sched_setprio(pang, 0);
    sched_setprio(peng, 2);
    sched_setprio(ping, 4);
    sched_setprio(pong, 6);
    sched_setprio(pung, 8);

    printk("%5u ms: %s fim\n", time(), name);

    task_exit(NOERROR);
}
