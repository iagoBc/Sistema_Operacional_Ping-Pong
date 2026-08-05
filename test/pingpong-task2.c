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

static struct task_t *task[NUMTASKS];

// corpo das tarefas
void body_task(void *arg)
{
    int id;
    int next;

    id = task_id(NULL);
    printk("\tIniciei  tarefa %5d\n", id);

    // passa o controle para a proxima tarefa
    next = ((long)arg + 1) % NUMTASKS;
    printk("\tVou mudar para a tarefa %d\n", task_id(task[next]));
    task_switch(task[next]);

    printk("\tEncerrei tarefa %5d\n", id);

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

    // inicia tarefas
    for (long i = 0; i < NUMTASKS; i++)
    {
        task[i] = task_create(NULL, body_task, (void *)i);
        assert(task[i]);
        printk("%s: criei a tarefa %d\n", name, task_id(task[i]));
    }

    // passa o controle para cada uma delas em sequencia
    for (long i = 0; i < NUMTASKS; i++)
    {
        printk("%s: vou ativar a tarefa %d\n", name, task_id(task[i]));
        status = task_switch(task[i]);
        assert(status == NOERROR);
    }

    // destrói as tarefas
    for (long i = 0; i < NUMTASKS; i++)
    {
        printk("%s: vou destruir a tarefa %d\n", name, task_id(task[i]));
        status = task_destroy(task[i]);
        assert(status == NOERROR);
    }

    printk("%5u ms: %s fim\n", time(), name);

    task_switch(NULL);
}
