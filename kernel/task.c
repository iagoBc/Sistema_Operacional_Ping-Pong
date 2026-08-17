// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência básica de tarefas.

#include "macros.h"
#include "lib/pplibc.h"
#include "task.h"
#include "memory.h"

struct task_t task_kernel;
struct task_t* task_atual = NULL;

// tamanho de pilha das tarefas 
#define STACKSIZE 32 * 1024

unsigned long int ids = 1; // Variavel global para nao repetir os ids

// inicializa o subsistema de tarefas.
// (chamada pelo núcleo na inicialização).
void task_init(){
    task_kernel.name = "kernel";
    task_kernel.id = 0;
    task_atual = &task_kernel;
    task_kernel.status = EXECUCAO;

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
    struct task_t *tarefa = mem_alloc(sizeof(struct task_t));
    if(!tarefa) return NULL;
    tarefa->name = name;
    tarefa->id = ids++;
    tarefa->status = PRONTO;

    tarefa->stack = mem_alloc(STACKSIZE);
    if(!tarefa->stack){
        mem_free(tarefa);
        return NULL;
    }

    if(ctx_create(&tarefa->context, entry, arg, tarefa->stack, STACKSIZE) == ERROR){
        mem_free(tarefa->stack);
        mem_free(tarefa);
        return NULL;
    }

    tarefa->parent = task_atual;

    ppos_debug("task %d (%s) create task %d (%s)\n", tarefa->parent->id, tarefa->parent->name, tarefa->id, tarefa->name);

    return tarefa;
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
    if(!task) return task_atual->id;
    return task->id;
}

// informa o nome de uma tarefa (ou da tarefa atual se task == NULL)
char *task_name(struct task_t *task){
    if(!task) return task_atual->name;
    return task->name;
}

//  Esta é a operação básica de troca de contexto entre tarefas, que usa a função ctx_switch da biblioteca de contextos. Ela será chamada sempre que for necessária uma troca de contexto.
// Caso task seja nulo, a CPU deve ser transferida para a tarefa que criou a tarefa atual, ou seja, a tarefa que estava ativa quando task_create criou a tarefa que está executando neste momento. 
// a tarefa atual libera a CPU e volta para a fila de prontas; a execução
// retorna ao núcleo/dispatcher.
int task_switch(struct task_t *task){
    struct task_t *atual = task_atual;
    struct task_t *nova;

    if (!task) nova = task_atual->parent;
    else nova = task;

    if (nova == atual) return ERROR;

    if (atual->status == EXECUCAO)
        atual->status = PRONTO;

    task_atual = nova;
    task_atual->status = EXECUCAO;

    ppos_debug("task %i (%s) switch to task %i (%s)\n",
               atual->id, atual->name, task_atual->id, task_atual->name);

    return ctx_switch(&atual->context, &nova->context);
}

void task_yield(){
    return;
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
    return;
}

