// GRR20244409 Iago Cardoso Bariuka
// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Dispatcher: gerencia os estados das tarefas.

#include "task.h"
#include "macros.h"
#include "lib/queue.h"
#include "scheduler.h"

extern void user_main(void *arg);
extern void task_switch(struct task_t *task);


struct queue_t *ready_queue;
struct queue_t *suspended_queue;

extern struct task_t kernel_task;
extern struct task_t* current_task;


// inicia o subsistema dispatcher
// (chamada pelo núcleo na inicialização).
void dispatcher_init(){
    ready_queue = queue_create();
    suspended_queue = queue_create();
    ppos_debug("subsystem dispatcher initiated\n");
}

// encerra o subsistema dispatcher
// (chamada pelo núcleo no encerramento).
void dispatcher_term(){
    ppos_debug("dispatcher stopping, no more user tasks\n");
}

// executa a tarefa indicada: retira-a da fila de prontas, muda seu status
// para RODANDO e transfere a CPU para ela.
void task_run(struct task_t *task){
    queue_del(ready_queue, task);
    task->state = RUNNING;
    task_switch(task);
}

// suspende a tarefa atual: retira-a da fila de prontas, muda seu status para
// SUSPENSA, a insere na fila "queue" (se não for NULL) e retorna ao dispatcher.
void task_suspend(struct queue_t *queue){
    current_task->state = SUSPENDED;
    if(queue != NULL){
        queue_add(queue, current_task);
    }
    task_switch(&kernel_task);
}

// acorda uma tarefa: retira-a da fila onde se encontra suspensa (se estiver
// em uma fila), muda seu status para PRONTA e a insere na fila de prontas,
// para retomar (ou iniciar) sua execução.
void task_awake(struct task_t *task){
    if(!task) return;
    if(queue_has(suspended_queue, task)) queue_del(suspended_queue, task);
    task->state = READY;
    queue_add(ready_queue, task);
}

// executa o dispatcher (chamada pelo núcleo após a inicialização).
void dispatcher(){
    // cria a tarefa inicial de usuário, que executará user_main()
    struct task_t *task_user = task_create("user_main", user_main, NULL);

    // enquanto houver tarefas de usuário
    while((queue_size(ready_queue) > 0) ||(queue_size(suspended_queue) > 0)){
        // escolhe a próxima tarefa a executar
        struct task_t *next = scheduler(ready_queue);

        // escalonador escolheu uma tarefa?      
        if (next != NULL){
            // transfere controle para a próxima tarefa
            task_run(next);
         
            // ao voltar ao dispatcher, trata a tarefa de acordo com seu estado
            switch (next->state){
                case READY:
                    break;
                case RUNNING:
                    break;
                case TERMINATED:
                    break;
                case SUSPENDED:
                    break;
                default:
                    break;
            }
        }
    }

    // destrói a tarefa inicial do usuário
    task_destroy(task_user);
}

