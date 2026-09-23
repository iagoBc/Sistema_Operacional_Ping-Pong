// GRR20244409 Iago Cardoso Bariuka
// PingPongOS - PingPong Operating System
// Este arquivo PODE/DEVE ser alterado.
// Gerência básica do tempo.

#include "hardware/cpu.h"
#include "task.h"
#include "tcb.h"
#include "time.h"
#include "lib/pplibc.h"

#define NULL 0

unsigned int system_time = 0; // Variável global para armazenar o tempo do sistema

extern struct task_t *current_task;

// Função de tratamento da interrupção do timer
void handle(int irq){
    system_time++;
    if(!current_task) return;
    if(current_task->type == USER){
        current_task->cpu++;                                                // Incrementa o tempo de CPU da tarefa atual
        if(current_task->quantum > 0) current_task->quantum--;              // Decrementa o quantum da tarefa atual
        if(current_task->quantum == 0) task_yield();                        // A tarefa atual libera a CPU e volta para a fila de prontas
    }
}

// inicia o subsistema de gestão do tempo
// (chamada pelo núcleo na inicialização).
void time_init(){
    hw_irq_handle(IRQ_TIMER, handle);               // Registra a função de tratamento da interrupção do timer
    hw_timer(TICK, TICK);                           // Configura o timer para gerar interrupções a cada TICK milissegundos
}

void time_term(){
    hw_timer(0, 0);
    hw_irq_handle(IRQ_TIMER, NULL);
}

unsigned int time(){
    return system_time;                             // Retorna o tempo do sistema em milissegundos
}
