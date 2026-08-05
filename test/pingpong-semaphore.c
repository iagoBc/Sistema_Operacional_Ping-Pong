// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste de semáforos (leve)

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

struct task_t *a1, *a2, *b1, *b2;	// descritores de tarefas
int s1, s2;				// descritores de semáforos

// corpo da tarefa A
void body_a(void *arg)
{
    int status;

    for (int i = 0; i < 5; i++)
    {
        status = sem_down(s1);
        if (status == ERROR)
            task_exit(1);

        printk("%5u ms: %s zig (%d)\n", time(), (char *)arg, i);
        task_sleep(1000);

        status = sem_up(s2);
        if (status == ERROR)
            task_exit(2);
    }

    task_exit(NOERROR);
}

// corpo da tarefa B
void body_b(void *arg)
{
    int status;

    for (int i = 0; i < 5; i++)
    {
        status = sem_down(s2);
        if (status == ERROR)
            task_exit(1);

        printk("%5u ms: %s zag (%d)\n", time(), (char *)arg, i);
        task_sleep(1000);

        status = sem_up(s1);
        if (status == ERROR)
            task_exit(2);
    }
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

    // inicia semáforos
    s1 = sem_create(1);
    assert(s1 >= 0);
    s2 = sem_create(0);
    assert(s2 >= 0);

    // cria tarefas
    a1 = task_create("a1", body_a, "A1");
    assert(a1);
    a2 = task_create("a2", body_a, "\tA2");
    assert(a2);
    b1 = task_create("b1", body_b, "\t\t\tB1");
    assert(b1);
    b2 = task_create("b2", body_b, "\t\t\t\tB2");
    assert(b2);

    // aguarda a1 encerrar sem erro
    status = task_wait(a1);
    assert(status == NOERROR);

    // destrói semáforos
    status = sem_destroy(s1);
    assert(status == NOERROR);
    status = sem_destroy(s2);
    assert(status == NOERROR);

    // aguarda a2, b1 e b2 encerrarem com erro
    status = task_wait(a2);
    assert(status != NOERROR);
    status = task_wait(b1);
    assert(status != NOERROR);
    status = task_wait(b2);
    assert(status != NOERROR);

    // destrói as tarefas
    status = task_destroy(a1);
    assert(status == NOERROR);
    status = task_destroy(a2);
    assert(status == NOERROR);
    status = task_destroy(b1);
    assert(status == NOERROR);
    status = task_destroy(b2);
    assert(status == NOERROR);

    printk("%5u ms: %s fim\n", time(), name);

    task_exit(NOERROR);
}
