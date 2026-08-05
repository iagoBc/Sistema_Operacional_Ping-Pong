// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Emulação de alguns mecanismos do hardware da CPU

#ifndef __HW_CPU__
#define __HW_CPU__

// Interrupções virtuais
#define IRQ_RESERVED 0   // reservada (não usar)
#define IRQ_TIMER 1      // IRQ gerada pelo temporizador
#define IRQ_DISK 2       // IRQ gerada pelo controlador de disco
#define IRQ_UART 3       // IRQ gerada pela UART (serial)
#define IRQ_NET 4        // IRQ gerada pela interface de rede

// Define a função de tratamento de uma IRQ; a função "handle"
// será chamada sempre que a IRQ indicada ocorrer. Caso "handle"
// seja nulo, desfaz a definição anterior.
// Retorno: ERROR ou NOERROR
int hw_irq_handle(int irq, void (*handle)(int));

// Habilita (enable != 0) ou desabilita (enable == 0) as IRQs.
void hw_irq_enable(int enable);

// Arma o temporizador para disparar após "first" ms e depois
// repetir a cada "next" ms. Se first == next == 0, desarma-o.
// Ao disparar, o temporizador gera a IRq IRQ_TIMER.
// Retorno: ERROR ou NOERROR
int hw_timer(int first, int next);

// "Wait for Interrupt": suspende a CPU até a próxima IRq,
// para economizar energia.
void hw_wfi();

// Desliga o hardware; se error != 0, encerra com status de erro
// (útil quando depurar com o GDB).
void hw_poweroff(int error);

#endif
