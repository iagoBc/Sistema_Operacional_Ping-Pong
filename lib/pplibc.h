// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Implementação simplificada de algumas funções básicas da biblioteca C,
// que devem ser definidas aqui para nos liberar da GLibC.

#ifndef __PPLIBC__
#define __PPLIBC__

// códigos de retorno das funções
#ifndef ERROR
#define ERROR -1
#define NOERROR 0
#endif

// ponteiro nulo
#ifndef NULL
#define NULL ((void *)0)
#endif

// Substitui o printf da LibC onde este arquivo for incluído.
//#define printf printk

// Copia "size" bytes da área de memória apontada por "orig"
// para a área apontada por "dest".
// Retorno: NOERROR ou ERROR.
int mem_copy(const char *orig, char *dest, int size);

// Envia um caractere para o console.
void putch(const char c);

// Envia uma string para o console.
void putst(const char *s);

// Imprime uma mensagem no console, de forma similar a printk.
// Suporta os formatos %d %i %u %c %s %p %%
// e os modificadores %NN (tamanho) e %- (alinhamento).
void printk(const char *fmt, ...);

// retorna o número sem sinal
int abs(int num);

// define a semente do gerador de aleatórios
void randseed(unsigned long seed);

// gera um inteiro aleatório entre 0 e MAX_INT
unsigned long randnum(void);

// Encerra o programa normalmente, com o exit code informado.
//void exit(int exit_code);

// Encerra o programa com erro
//void abort();

#endif
