// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência básica de tarefas.

#include "macros.h"
#include "lib/pplibc.h"
#include "task.h"
#include "memory.h"

#define STACKSIZE 32 * 1024 

struct task_t task_kernel; // Tarefa para o fluxo de execução do núcleo
struct task_t* task_current = NULL; // Struct para armazenar a tarefa atual em execução
unsigned int ids = 1; // Contador de IDs

void task_init(){ 
    task_kernel.name = "kernel";
    task_kernel.id = 0;
    task_current = &task_kernel;
    task_kernel.stats = RUNNING;
    task_kernel.parent = NULL; // A tarefa do kernel não tem pai

    ppos_debug("subsystem task initiated\n");
}

struct task_t * task_create(char *name, void (*entry)(void *), void *arg){
    struct task_t *task = mem_alloc(sizeof(struct task_t));
    if(!task) return NULL;

    task->name = name;
    task->id = ids++; 
    task->stats = READY; // Tarefa pronta para ser executada

    task->stack = mem_alloc(STACKSIZE); // Aloca memoria para a pilha da tarefa
    if(!task->stack){
        mem_free(task);
        return NULL;
    }

    if(ctx_create(&task->context, entry, arg, task->stack, STACKSIZE) == ERROR){ // Inicializa o contexto da tarefa 
        mem_free(task->stack);
        mem_free(task);
        return NULL;
    }

    task->parent = task_current; // Define a tarefa atual como pai da nova tarefa

    ppos_debug("task %d (%s) create task %d (%s)\n", task->parent->id, task->parent->name, task->id, task->name);

    return task;
}

int task_destroy(struct task_t *task){
    mem_free(task->stack);
    mem_free(task);
    return NOERROR;
}

int task_id(struct task_t *task){
    if(!task) return task_current->id;
    return task->id; 
}

char *task_name(struct task_t *task){
    if(!task) return task_current->name;
    return task->name;
}

int task_switch(struct task_t *task){
    struct task_t *current = task_current;
    struct task_t *new; 

    if (!task) new = task_current->parent; // Se o parametro = NULL, a tarefa atual vai ser trocada pela tarefa pai
    else new = task; 

    if (new == current) return ERROR;

    current->stats = READY;

    // Troca de tarefa para a nova tarefa
    task_current = new;
    task_current->stats = RUNNING;

    ppos_debug("task %i (%s) switch to task %i (%s)\n", current->id, current->name, task_current->id, task_current->name);

    return ctx_switch(&current->context, &new->context); 
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

