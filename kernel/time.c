// PingPongOS - PingPong Operating System

// Este arquivo PODE/DEVE ser alterado.

// Gerência básica do tempo.

#include "hardware/cpu.h"
#include "task.h"
#include "tcb.h"
#include "time.h"

#define NULL 0

unsigned int system_time = 0; // Variável global para armazenar o tempo do sistema

extern struct task_t *current_task;

void handle(int irq){
    system_time++;
    if(current_task->type == USER && current_task->quantum > 0) current_task->quantum--;
    if(current_task->quantum == 0) task_yield();
}

// inicia o subsistema de gestão do tempo
// (chamada pelo núcleo na inicialização).
void time_init(){
    hw_irq_handle(IRQ_TIMER, handle);
    hw_timer(TICK, TICK);
}

void time_term(){
    hw_timer(0, 0);
    hw_irq_handle(IRQ_TIMER, NULL);
}

unsigned int time(){
    return system_time;
}
