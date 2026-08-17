// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência básica de tarefas.

#include "macros.h"
#include "lib/pplibc.h"
#include "task.h"
#include "memory.h"

#define STACKSIZE 32 * 1024 // Tamanho de pilha das tarefas

struct task_t task_kernel; // Tarefa do kernel (inicial)
struct task_t* task_atual = NULL; // Tarefa atual (em execução)
unsigned long int ids = 1; // Variavel global para nao repetir os ids

void task_init(){
    task_kernel.name = "kernel";
    task_kernel.id = 0;
    task_atual = &task_kernel;
    task_kernel.status = EXECUCAO; // A tarefa do kernel está em execução
    task_kernel.parent = NULL; // A tarefa do kernel não tem pai

    ppos_debug("subsystem task initiated\n");
}

struct task_t * task_create(char *name, void (*entry)(void *), void *arg){
    struct task_t *tarefa = mem_alloc(sizeof(struct task_t));
    if(!tarefa) return NULL;

    tarefa->name = name;
    tarefa->id = ids++; // Atribui um ID único à tarefa
    tarefa->status = PRONTO; // A tarefa criada está pronta para ser executada

    tarefa->stack = mem_alloc(STACKSIZE); // Aloca a pilha da tarefa
    if(!tarefa->stack){
        mem_free(tarefa);
        return NULL;
    }

    if(ctx_create(&tarefa->context, entry, arg, tarefa->stack, STACKSIZE) == ERROR){ // Cria o contexto da tarefa
        mem_free(tarefa->stack);
        mem_free(tarefa);
        return NULL;
    }

    tarefa->parent = task_atual; // Define a tarefa atual como pai da nova tarefa

    ppos_debug("task %d (%s) create task %d (%s)\n", tarefa->parent->id, tarefa->parent->name, tarefa->id, tarefa->name);

    return tarefa;
}

int task_destroy(struct task_t *task){
    mem_free(task->stack);
    mem_free(task);
    return NOERROR;
}

int task_id(struct task_t *task){
    if(!task) return task_atual->id;
    return task->id; // Retorna o ID da tarefa especificada ou da tarefa atual se task for NULL
}

char *task_name(struct task_t *task){
    if(!task) return task_atual->name;
    return task->name; // Retorna o nome da tarefa especificada ou da tarefa atual se task for NULL
}

int task_switch(struct task_t *task){
    struct task_t *atual = task_atual;
    struct task_t *nova; 

    if (!task) nova = task_atual->parent; // Se task for NULL, alterna para a tarefa pai
    else nova = task; // Caso contrário, alterna para a tarefa especificada

    if (nova == atual) return ERROR;

    if (atual->status == EXECUCAO) atual->status = PRONTO;

    task_atual = nova;
    task_atual->status = EXECUCAO;

    ppos_debug("task %i (%s) switch to task %i (%s)\n", atual->id, atual->name, task_atual->id, task_atual->name);

    return ctx_switch(&atual->context, &nova->context); // Troca o contexto entre a tarefa atual e a nova tarefa
}

void task_term(){
    return;
}

void task_yield(){
    return;
}

int task_wait(struct task_t *task){
    return 0;
}

void task_sleep(int t){
    return;
}

void task_exit(int exit_code){
    return;
}

