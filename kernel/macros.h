// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

#ifndef __MACROS__
#define __MACROS__

// este código deve ser compilado em sistemas UNIX-like
#ifndef __linux__
#error "Este codigo foi planejado para ambientes Linux."
#endif

#include "hardware/cpu.h"
#include "lib/pplibc.h"

// códigos de cor ANSI (terminal)
#define FONT_NORMAL  "\x1B[0m"
#define FONT_BLACK   "\x1B[30m"
#define FONT_RED     "\x1B[31m"
#define FONT_GREEN   "\x1B[32m"
#define FONT_YELLOW  "\x1B[33m"
#define FONT_BLUE    "\x1B[34m"
#define FONT_MAGENTA "\x1B[35m"
#define FONT_CYAN    "\x1B[36m"
#define FONT_WHITE   "\x1B[37m"
#define BACK_BLACK   "\x1B[40m"
#define BACK_RED     "\x1B[41m"
#define BACK_GREEN   "\x1B[42m"
#define BACK_YELLOW  "\x1B[43m"
#define BACK_BLUE    "\x1B[44m"
#define BACK_MAGENTA "\x1B[45m"
#define BACK_CYAN    "\x1B[46m"
#define BACK_WHITE   "\x1B[47m"

// Imprime mensagem de depuração se DEBUG estiver definido;
// Uso igual ao printk: ppos_debug(format, arg1, arg2, ...)
#ifdef DEBUG
#define ppos_debug(...)                   \
    do                                    \
    {                                     \
        printk(FONT_CYAN "DEBUG: ");      \
        printk(__VA_ARGS__);              \
        printk(FONT_NORMAL);              \
    } while (0)
#else
#define ppos_debug(...)  // não faz nada
#endif

// Imprime mensagem de aviso, com arquivo e linha do código.
// Uso igual ao printk: ppos_warn(format, arg1, arg2, ...)
#define ppos_warn(...)                                        \
    do                                                        \
    {                                                         \
        printk(BACK_YELLOW "WARN (%s:%d)" FONT_NORMAL ": ",   \
               __FILE__, __LINE__);                           \
        printk(__VA_ARGS__);                                  \
    } while (0)

// Imprime mensagem de erro com arquivo e linha do código e encerra.
// Uso igual ao printk: ppos_panic(format, arg1, arg2, ...)
#define ppos_panic(...)                                              \
    do                                                               \
    {                                                                \
        printk(BACK_RED "PANIC (%s:%d)" FONT_NORMAL ": ",            \
               __FILE__, __LINE__);                                  \
        printk(__VA_ARGS__);                                         \
        hw_poweroff(1);                                              \
    } while (0)

// Testa parâmetros de funções, imprime mensagem e retorna erro.
// Uso: check_parm(value < 0, "invalid value", NULL);
#define check_parm(error_cond, error_msg, ret_val)      \
    if (error_cond)                                     \
    {                                                   \
        printk(BACK_YELLOW);                            \
        printk("Error in %s():", __func__);             \
        printk(FONT_NORMAL);                            \
        printk(" %s\n", error_msg);                     \
        return ret_val;                                 \
    }

#endif
