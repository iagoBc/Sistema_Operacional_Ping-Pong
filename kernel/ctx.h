// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 07/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

/*
Rotinas de manipulação de contextos no userspace, inspiradas de
https://github.com/kaniini/libucontext/blob/master/arch/x86_64
https://graphitemaster.github.io/fibers/

Estas rotinas são específicas para CPUs x86_64 (Intel/AMD 64 bits)
no sistema operacional Linux. Não funcionam em outras plataformas
sem as devidas adaptações.

To-do:
- aceitar stack == NULL para não alterar a pilha do contexto.
- salvar/restaurar registradores de ponto flutuante %xmm*
- SEGV se função do contexto encerrar sem ctx_switch (falta uc_link).
*/

#ifndef __CTX__
#define __CTX__

// verificação de processador
#ifndef __x86_64__
#error "This library is designed specifically for x86_64 processors."
#endif

// verificação de sistema operacional
#ifndef __linux__
#error "This library is designed specifically for Linux OS."
#endif

// verificação de compilador (flags de otimização -O)
#ifdef __OPTIMIZE__
#error "Optimizations (-O*) may interfere with the context switching"
#endif

//----------------------------------------------------------------------

#include <stdint.h>

#define ERROR -1
#define NOERROR 0

/*
Este struct permite salvar um contexto, ou seja, um estado da CPU
definido pelos valores de seus principais registradores.
Ajuste os offsets dos registradores em ctx.s se alterar algo aqui.
Este struct deve ser alocado com alinhamento de 16 bytes.
*/
struct ctx_t
{
    // instruction pointer
    uint64_t rip;

    // stack pointer & base
    uint64_t rsp, rbp;

    // registradores gerais
    uint64_t rdi, rsi, rax, rbx, rcx, rdx;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;

    // endereço e tamanho da pilha associada a este contexto
    void *stack;
    unsigned long size;
} __attribute__((aligned(16)));

//----------------------------------------------------------------------

/*
Define um contexto com uma função de entrada e uma pilha própria:
 - entry: função a ser executada ao ativar este contexto;
 - arg:   argumento para a função "entry" quando ela iniciar;
 - stack: endereço inicial (mais baixo) da pilha; deve ser
          previamente alocado e alinhado em 16 bytes;
 - size:  tamanho da pilha, deve ter ao menos 4.096 bytes.
Retorno:  0 em sucesso, -1 em erro (vide ctx.s).
*/
int ctx_create(struct ctx_t *ctx, void(entry)(void *),
               void *arg, void *stack, uint64_t size);

/*
Troca contextos: salva o estado atual da CPU em ctx1 (se não for
NULL) e carrega na CPU o estado salvo em ctx2 (se não for NULL).
Retorno: 0 em sucesso, -1 em erro (vide ctx.s).
*/
int ctx_switch(struct ctx_t *ctx1, struct ctx_t *ctx2);

#endif
