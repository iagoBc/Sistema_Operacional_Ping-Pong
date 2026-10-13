// GRR20244409 Iago Cardoso Bariuka
// PingPongOS - PingPong Operating System
// Este arquivo PODE/DEVE ser alterado.
// Dispatcher: gerencia os estados das tarefas.

#include "task.h"
#include "time.h"
#include "macros.h"
#include "lib/queue.h"
#include "scheduler.h"

extern void user_main(void *arg);
extern void task_switch(struct task_t *task);


struct queue_t *ready_queue;                                    // Fila de tarefas prontas para execução
struct queue_t *suspended_queue;                                // Fila de tarefas suspensas
struct queue_t *sleeping_queue;                                 // Fila de tarefas em espera

extern struct task_t kernel_task;                               // Tarefa para o fluxo de execução do núcleo
extern struct task_t* current_task;                             // Struct para armazenar a tarefa atual em execução


// inicia o subsistema dispatcher
// (chamada pelo núcleo na inicialização).
void dispatcher_init(){
    ready_queue = queue_create();
    suspended_queue = queue_create();
    sleeping_queue = queue_create();
    ppos_debug("subsystem dispatcher initiated\n");
}

// encerra o subsistema dispatcher
// (chamada pelo núcleo no encerramento).
void dispatcher_term(){
    queue_destroy(ready_queue);
    queue_destroy(suspended_queue);
    queue_destroy(sleeping_queue);
    ppos_debug("dispatcher stopping, no more user tasks\n");
}

// executa a tarefa indicada: retira-a da fila de prontas, muda seu status
// para RODANDO e transfere a CPU para ela.
void task_run(struct task_t *task){
    queue_del(ready_queue, task);
    task->state = RUNNING;
    task->quantum = QUANTUM;
    task_switch(task);
}

// suspende a tarefa atual: retira-a da fila de prontas, muda seu status para
// SUSPENSA, a insere na fila "queue" (se não for NULL) e retorna ao dispatcher.
void task_suspend(struct queue_t *queue){
    hw_irq_enable(0);
    if(current_task->state != SLEEPING) current_task->state = SUSPENDED;
    if(queue != NULL) queue_add(queue, current_task);
    task_switch(&kernel_task);
    hw_irq_enable(1);
}

// acorda uma tarefa: retira-a da fila onde se encontra suspensa (se estiver
// em uma fila), muda seu status para PRONTA e a insere na fila de prontas,
// para retomar (ou iniciar) sua execução.
void task_awake(struct task_t *task){
    queue_del(suspended_queue, task);
    queue_del(sleeping_queue, task);
    task->state = READY;
    queue_add(ready_queue, task);
}

// acorda todas as tarefas que estão em espera e cujo tempo de espera já
// expirou, ou seja, cujo tempo de espera é menor ou igual ao tempo atual.
void wake_sleeping(){
    hw_irq_enable(0);                                           // Desabilita interrupções para evitar condições de corrida
    unsigned int now = time();
    struct task_t *task = queue_head(sleeping_queue);
    
    while(task){
        if((int)(now - task->sleep_time) >= 0){                      // Verifica se o tempo de sleep da tarefa expirou
            task_awake(task);                                   // Acorda a tarefa, removendo-a da fila de tarefas em espera e adicionando-a à fila de prontas
            task = queue_head(sleeping_queue);                  // Atualiza a tarefa atual para a próxima na fila de tarefas em espera
        } 
        
        else task = queue_next(sleeping_queue);                 // Se a tarefa ainda não expirou, passa para a próxima tarefa na fila de tarefas em espera
        
    }
    hw_irq_enable(1);               // Habilita interrupções novamente
}

// executa o dispatcher (chamada pelo núcleo após a inicialização).
void dispatcher(){
    // cria a tarefa inicial de usuário, que executará user_main()
    struct task_t *task_user = task_create("user_main", user_main, NULL);

    // enquanto houver tarefas de usuário
    while((queue_size(ready_queue) > 0) ||
          (queue_size(suspended_queue) > 0) ||
          (queue_size(sleeping_queue) > 0)){
        hw_irq_enable(1);                       // habilita interrupções para o núcleo
        wake_sleeping();

        // escolhe a próxima tarefa a executar, se houver alguma pronta
        struct task_t *next = NULL;
        if(queue_size(ready_queue) > 0) next = scheduler(ready_queue);

        // escalonador escolheu uma tarefa?      
        if(next != NULL){
            // transfere controle para a próxima tarefa
            task_run(next);
         
            // ao voltar ao dispatcher, trata a tarefa de acordo com seu estado
            switch(next->state){
                case READY:
                    break;
                case RUNNING:
                    break;
                case TERMINATED:
                    printk("PPOS: task   %d (%s),     %d ms run,     %d ms cpu,     %d acts, exit code   %d\n",
                            next->id, next->name, next->run, next->cpu, next->acts, next->exit);
                case SUSPENDED:
                    break;
                default:
                    break;
            }
        } 
        else if(queue_size(sleeping_queue) > 0) hw_wfi();
        
    }

    // destrói a tarefa inicial do usuário
    task_destroy(task_user);

    printk("PPOS: task   %d (%s), %d ms run,     %d ms cpu,  %d acts, exit code   %d\n",
        kernel_task.id, kernel_task.name, time(), kernel_task.cpu, kernel_task.acts, kernel_task.exit);
}
