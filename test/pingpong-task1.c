// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 07/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste da gestão básica de tarefas

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

static struct task_t *ping, *pong;

// corpo da tarefa Ping
void body_ping(void *)
{
    char *name = task_name(NULL);

    printk("\t%s: inicio\n", name);
    for (int i = 0; i < 4; i++)
    {
        printk("\t%s: %d\n", name, i);
        task_switch(pong);
    }
    printk("\t%s: fim\n", name);
    task_switch(NULL);
}

// corpo da tarefa Pong
void body_pong(void *)
{
    char *name = task_name(NULL);

    printk("\t\t%s: inicio\n", name);
    for (int i = 0; i < 4; i++)
    {
        printk("\t\t%s: %d\n", name, i);
        task_switch(ping);
    }
    printk("\t\t%s: fim\n", name);
    task_switch(NULL);
}

// corpo da tarefa principal
void user_main(void *arg)
{
    int status, id;
    char *name;

    name = task_name(NULL);
    id   = task_id(NULL);
    printk("%5u ms: %s (id %d): inicio\n", time(), name, id);

    ping = task_create("ping", body_ping, NULL);
    assert(ping);
    pong = task_create("pong", body_pong, NULL);
    assert(pong);

    status = task_switch(ping);
    assert(status == NOERROR);

    status = task_switch(pong);
    assert(status == NOERROR);

    status = task_destroy(ping);
    assert(status == NOERROR);

    status = task_destroy(pong);
    assert(status == NOERROR);

    printk("%5u ms: %s fim\n", time(), name);

    task_switch(NULL);
}
