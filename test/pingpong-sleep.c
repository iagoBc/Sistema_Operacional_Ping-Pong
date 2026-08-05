// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste da função task_sleep()

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

struct task_t *pang, *peng, *ping, *pong, *pung;

// corpo das tarefas
void body(void *arg)
{
    int i, t_sleep, t_before, t_after;
    char *status;

    printk("%5u ms: %s: inicio\n", time(), (char *)arg);
    for (i = 0; i < 20; i++)
    {
        // sorteia tempo entre 0 e 2000 ms (2s), em saltos de 100 ms
        t_sleep = 100 * (randnum() % 21);

        // informa o quanto vai dormir
        printk("%5u ms: %s vai dormir %d ms\n",
               time(), (char *)arg, t_sleep);

        // registra tempo antes e depois de dormir
        t_before = time();
        task_sleep(t_sleep);
        t_after = time();

        // verifica se dormiu o intervalo especificado
        status = (t_after - t_before) == t_sleep ? "ok" : "ERRADO";

        // informa o quanto efetivamente dormiu
        printk("%5u ms: %s dormiu     %d ms (%s)\n", time(),
               (char *)arg, t_after - t_before, status);
    }
    printk("%5u ms: %s: fim\n", time(), (char *)arg);
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

    // aguarda tarefas concluírem
    printk("%5u ms: user: espera Pang...\n", time());
    status = task_wait(pang);
    assert(status == NOERROR);
    printk("%5u ms: user: Pang acabou\n", time());

    printk("%5u ms: user: espera Peng...\n", time());
    status = task_wait(peng);
    assert(status == NOERROR);
    printk("%5u ms: user: Peng acabou\n", time());

    printk("%5u ms: user: espera Ping...\n", time());
    status = task_wait(ping);
    assert(status == NOERROR);
    printk("%5u ms: user: Ping acabou\n", time());

    printk("%5u ms: user: espera Pong...\n", time());
    status = task_wait(pong);
    assert(status == NOERROR);
    printk("%5u ms: user: Pong acabou\n", time());

    printk("%5u ms: user: espera Pung...\n", time());
    status = task_wait(pung);
    assert(status == NOERROR);
    printk("%5u ms: user: Pung acabou\n", time());

    // destrói tarefas
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
