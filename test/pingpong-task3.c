// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 07/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste da gestão básica de tarefas

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

#define NUMTASKS 512

static struct task_t *task;

// corpo das tarefas
void body_task(void *)
{
    printk("\tEstou na tarefa %5d\n", task_id(NULL));
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

    for (int i = 0; i < NUMTASKS; i++)
    {
        // cria a tarefa
        task = task_create(NULL, body_task, NULL);
        assert(task);

        // ativa a tarefa
        status = task_switch(task);
        assert(status == NOERROR);

        // após retornar, destrói a tarefa
        status = task_destroy(task);
        assert(status == NOERROR);
    }

    printk("%5u ms: %s fim\n", time(), name);

    task_switch(NULL);
}
