// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste da contabilização com muitas tarefas (stress)

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

#define WORKLOAD 5000
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
    for (int i = 0; i < 3; i++)
        hardwork(WORKLOAD);
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
    printk("Creating %d tasks\n", NUMTASKS);
    for (int i = 0; i < NUMTASKS; i++)
    {
        task[i] = task_create(NULL, body, NULL);
        assert(task[i]);
    }

    printk("%5u ms: %s fim\n", time(), name);

    task_exit(NOERROR);
}
