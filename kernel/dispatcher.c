// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Dispatcher: gerencia os estados das tarefas.

#include "task.h"
#include "macros.h"
extern void user_main(void *arg);


void dispatcher_init(){
    ppos_debug("subsystem dispatcher initiated\n");
}

void dispatcher_term(){
    ppos_debug("dispatcher stopping, no more user tasks\n");
}

void dispatcher(){
    ppos_debug("dispatcher started\n");
    struct task_t *task_user = task_create("user_main", user_main, NULL);
    task_switch(task_user);
    task_destroy(task_user);
    
}

