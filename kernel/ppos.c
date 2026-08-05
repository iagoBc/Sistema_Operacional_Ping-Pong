// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Funções iniciais do PPOS

#include "hardware/cpu.h"
#include "lib/pplibc.h"
#include "ppos.h"

//----------------------------------------------------------------------

// Inicia o sistema operacional
static void ppos_init()
{
    printk("PPOS: system initializing\n");

    // inicia os subsistemas
    mem_init();
    task_init();
    dispatcher_init();
    sched_init();
    time_init();
    sem_init();
    mqueue_init();
    block_init("hardware/disk.dat");

    printk("PPOS: system ready (uptime %u ms)\n", time());
}

//----------------------------------------------------------------------

// Encerra o sistema operacional
void ppos_term()
{
    printk("PPOS: system terminating\n", time());

    // encerra os subsistemas (em ordem contrária ao ppos_init)
    block_term("hardware/disk.dat");
    mqueue_term();
    sem_term();
    time_term();
    sched_term();
    dispatcher_term();
    task_term();
    mem_term();

    printk("PPOS: system terminated (uptime %u ms)\n", time());
}

//----------------------------------------------------------------------

int main()
{
    ppos_init();
    dispatcher();
    ppos_term();
    hw_poweroff(0);
}

//----------------------------------------------------------------------
