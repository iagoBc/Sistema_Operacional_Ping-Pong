// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Funções básicas/iniciais do PPOS

#ifndef __PPOS__
#define __PPOS__

// este código deve ser compilado em sistemas UNIX-like
#ifndef __linux__
#error "Este codigo foi planejado para ambientes Linux."
#endif

// módulos do sistema operacional
#include "macros.h"
#include "tcb.h"
#include "task.h"
#include "dispatcher.h"
#include "scheduler.h"
#include "time.h"
#include "memory.h"
#include "semaphore.h"
#include "mqueue.h"
#include "block.h"

// algumas funções que não devem ser usadas
#define printf DONT_USE_THIS_FUNCTION
#define fprintf DONT_USE_THIS_FUNCTION
#define fputc DONT_USE_THIS_FUNCTION
#define putc DONT_USE_THIS_FUNCTION
#define putchar DONT_USE_THIS_FUNCTION
#define fputs DONT_USE_THIS_FUNCTION
#define puts DONT_USE_THIS_FUNCTION
#define putwchar DONT_USE_THIS_FUNCTION
#define fputwc DONT_USE_THIS_FUNCTION
#define fputws DONT_USE_THIS_FUNCTION
#define fwrite DONT_USE_THIS_FUNCTION
#define pwrite DONT_USE_THIS_FUNCTION
#define writev DONT_USE_THIS_FUNCTION
#define write DONT_USE_THIS_FUNCTION
#define scanf DONT_USE_THIS_FUNCTION
#define fscanf DONT_USE_THIS_FUNCTION
#define fgetc DONT_USE_THIS_FUNCTION
#define getc DONT_USE_THIS_FUNCTION
#define getchar DONT_USE_THIS_FUNCTION
#define fgets DONT_USE_THIS_FUNCTION
#define vscanf DONT_USE_THIS_FUNCTION
#define vfscanf DONT_USE_THIS_FUNCTION
#define fread DONT_USE_THIS_FUNCTION
#define pread DONT_USE_THIS_FUNCTION
#define readv DONT_USE_THIS_FUNCTION
#define read DONT_USE_THIS_FUNCTION
#define signal DONT_USE_THIS_FUNCTION
#define sigaction DONT_USE_THIS_FUNCTION
#define sigprocmask DONT_USE_THIS_FUNCTION
#define sigsuspend DONT_USE_THIS_FUNCTION
#define sigevent DONT_USE_THIS_FUNCTION
#define raise DONT_USE_THIS_FUNCTION
#define siginterrupt DONT_USE_THIS_FUNCTION
#define timer_create DONT_USE_THIS_FUNCTION
#define timer_settime DONT_USE_THIS_FUNCTION
#define clock_gettime DONT_USE_THIS_FUNCTION
#define setitimer DONT_USE_THIS_FUNCTION
#define timer_delete DONT_USE_THIS_FUNCTION
#define timerfd_create DONT_USE_THIS_FUNCTION

#endif
