// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Emulação de uma porta serial com operações de E/S de caracteres.

/*
Esta é uma implementação trivial usando a syscall "write"; deve ser
substituída depois por uma emulação mais detalhada de uma UART
(Universal Asynchronous Receiver-Transmitter) ligada à linha serial.
*/

#include <unistd.h>
#include "serial.h"

#define NOERROR 0
#define ERROR -1

// static buffer for the "write" UNIX syscall
static char write_buf;

//----------------------------------------------------------------------

int hw_putchar(char c)
{
    // este código pode gerar alguns erros "falso-positivos" no Valgrind,
    // como mostra o exemplo abaixo:
    // ==69319== Syscall param write(buf) points to uninitialised byte(s)
    // ==69319==    at 0x4903687: __internal_syscall_cancel (cancellation.c:64)
    // ==69319==    by 0x49036AC: __syscall_cancel (cancellation.c:75)
    // ==69319==    by 0x4978975: write (write.c:26)

    // usar a syscall write com um buffer estático, para evitar condições
    // de disputa em buffers do userspace.
    write_buf = c;
    write(STDOUT_FILENO, &write_buf, 1);

    return(0);
}

//----------------------------------------------------------------------

char hw_getchar()
{
    // ... a implementar
    return (0);
}

//----------------------------------------------------------------------
