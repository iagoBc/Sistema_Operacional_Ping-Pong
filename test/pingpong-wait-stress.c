// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste da função task_wait() com muitas tarefas

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

#define WORKLOAD 3000
#define NUMTASKS 512

struct task_t *task[NUMTASKS];

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
void body()
{
    int tid = task_id(NULL);
    int max = 1 + randnum() % 5;

    for (int i = 0; i < max; i++)
        hardwork(WORKLOAD + randnum() % WORKLOAD);

    printk("task %3d: fim\n", tid);
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

    // cria tarefas
    printk("Creating %d tasks\n", NUMTASKS);
    for (int i = 0; i < NUMTASKS; i++)
    {
        task[i] = task_create(NULL, body, NULL);
        assert(task[i]);
    }

    // espera tarefas encerrarem
    for (int i = 0; i < NUMTASKS; i++)
    {
        printk("Waiting for task %d to finish\n", task_id(task[i]));
        status = task_wait(task[i]);
        assert(status == task_id(task[i]));
    }

    // destrói as tarefas
    for (int i = 0; i < NUMTASKS; i++)
    {
        status = task_destroy(task[i]);
        assert(status == NOERROR);
    }

    printk("%5u ms: %s fim\n", time(), name);

    task_exit(NOERROR);
}
