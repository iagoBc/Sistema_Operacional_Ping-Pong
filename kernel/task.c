// GRR20244409 Iago Cardoso Bariuka
// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência básica de tarefas.

#include "macros.h"
#include "lib/pplibc.h"
#include "lib/queue.h"
#include "task.h"
#include "memory.h"

#define STACKSIZE 32 * 1024 

struct task_t kernel_task; // Tarefa para o fluxo de execução do núcleo
struct task_t* current_task = NULL; // Struct para armazenar a tarefa atual em execução

extern struct queue_t *ready_queue; // Fila de tarefas prontas para execução

unsigned int ids = 1; // Contador de IDs

// inicializa o subsistema de tarefas.
// (chamada pelo núcleo na inicialização).
void task_init(){ 
    kernel_task.name = "kernel";
    kernel_task.id = 0;
    current_task = &kernel_task;
    kernel_task.state = RUNNING;
    kernel_task.parent = NULL; // A tarefa do kernel não tem pai

    ppos_debug("subsystem task initiated\n");
}

// encerra o subsistema de tarefas.
// (chamada pelo núcleo no encerramento).
void task_term(){
    return;
}

// cria uma nova tarefa: "name" é o nome da tarefa, "entry" é a função que
// ela irá executar e "arg" aponta para o valor recebido por "entry" ao
// iniciar (pode ser NULL).
// Retorno: ptr para a tarefa ou NULL se houver erro.
struct task_t * task_create(char *name, void (*entry)(void *), void *arg){
    struct task_t *task = mem_alloc(sizeof(struct task_t));
    if(!task) return NULL;

    task->name = name;
    task->id = ids++; 
    task->state = READY; // Tarefa pronta para ser executada

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

    task->parent = current_task; // Define a tarefa atual como pai da nova tarefa

    queue_add(ready_queue, task); // Adiciona a nova tarefa na fila de prontas
    ppos_debug("task %d (%s) create task %d (%s)\n", task->parent->id, task->parent->name, task->id, task->name);

    return task;
}

// destrói uma tarefa e libera seus recursos; somente deve atuar sobre tarefas
// terminadas. Retorno: NOERROR (0) ou ERROR (<0).
int task_destroy(struct task_t *task){
    mem_free(task->stack);
    mem_free(task);
    return NOERROR;
}

// informa o ID de uma tarefa (ou da tarefa atual se task == NULL)
int task_id(struct task_t *task){
    if(!task) return current_task->id;
    return task->id; 
}

// informa o nome de uma tarefa (ou da tarefa atual se task == NULL)
char *task_name(struct task_t *task){
    if(!task) return current_task->name;
    return task->name;
}

// transfere a CPU da tarefa atual para outra tarefa; se task_id == 0, 
// transfere para o núcleo. Ignora sem erro se "task" já tiver terminado.
// Retorno: NOERROR (0) ou ERROR (<0)
int task_switch(struct task_t *task){
    struct task_t *current = current_task;
    struct task_t *new; 

    if (!task) new = current_task->parent; // Se o parametro = NULL, a tarefa atual vai ser trocada pela tarefa pai
    else new = task; 

    if (new == current) return ERROR;

    current->state = READY;

    // Troca de tarefa para a nova tarefa
    current_task = new;
    current_task->state = RUNNING;

    ppos_debug("task %i (%s) switch to task %i (%s)\n", current->id, current->name, current_task->id, current_task->name);

    return ctx_switch(&current->context, &new->context); 
}

// a tarefa atual libera a CPU e volta para a fila de prontas; a execução
// retorna ao núcleo/dispatcher.
void task_yield(){
    current_task->state = READY;
    queue_add(ready_queue, current_task);
    task_switch(&kernel_task);
}

// suspende a tarefa atual até que a tarefa task termine; a execução retorna
// ao núcleo/dispatcher. Se a tarefa task já terminou, retorna sem suspender.
// Retorno: exit code tarefa que terminou ou ERROR.
int task_wait(struct task_t *task){
    return 0;
}

// suspende a tarefa atual por t milissegundos; a execução retorna ao
// núcleo/dispatcher.
void task_sleep(int t){
    return;
}

// encerra a execução da tarefa atual, informando um código de encerramento
// (exit_code); a execução retorna ao núcleo/dispatcher.
void task_exit(int exit_code){
    current_task->state = TERMINATED;
    task_switch(&kernel_task);
}

