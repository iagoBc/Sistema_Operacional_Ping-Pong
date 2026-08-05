// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Teste de semáforos (stress)

#include <assert.h>
#include "lib/pplibc.h"
#include "syscall.h"

#define NUMTASKS 64
#define NUMSTEPS 1000000

struct task_t *task[NUMTASKS];
int s;
static int soma = 0;

// corpo das tarefas
void body()
{
    int status;

    for (int i = 0; i < NUMSTEPS; i++)
    {
        // incrementa contador (seção crítica)
        status = sem_down(s);
        if (status == ERROR)
            break;

        soma += 1;

        status = sem_up(s);
        if (status == ERROR)
            break;
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

    // inicia semáforo em 0 (bloqueado)
    s = sem_create(0);
    assert(s >= 0);

    printk("%d tarefas somando %d vezes cada, aguarde...\n",
           NUMTASKS, NUMSTEPS);

    // cria tarefas
    for (int i = 0; i < NUMTASKS; i++)
    {
        task[i] = task_create(NULL, body, "Task");
        assert(task[i]);
    }

    // espera um pouco para liberar o semáforo; isso faz com que todas
    // as tarefas tenham sido criadas e possam competir pelo semáforo
    // em pé de igualdade.
    task_sleep(100);
    status = sem_up(s);
    assert(status == NOERROR);

    // aguarda as tarefas encerrarem
    for (int i = 0; i < NUMTASKS; i++)
    {
        status = task_wait(task[i]);
        assert(status == NOERROR);
    }

    // verifica se a soma está correta
    if (soma == (NUMTASKS * NUMSTEPS))
        printk("A soma deu %d, valor correto!\n", soma);
    else
        printk("A soma deu %d, mas deveria dar %d!\n",
               soma, NUMTASKS * NUMSTEPS);

    // destrói o semáforo
    status = sem_destroy(s);
    assert(status == NOERROR);

    // destrói as tarefas
    for (int i = 0; i < NUMTASKS; i++)
    {
        status = task_destroy(task[i]);
        assert(status == NOERROR);
    }

    printk("%5u ms: %s fim\n", time(), name);

    task_exit(NOERROR);
}
