// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Implementação simplificada de algumas funções básicas da biblioteca C,
// que devem ser definidas aqui para nos liberar da GLibC.

#define __PPOS__

#ifdef __PPOS__
#include "hardware/cpu.h"
#include "hardware/serial.h"
#define PUTCHAR hw_putchar
#define IRQ_ENABLE hw_irq_enable
#else
#include <stdio.h>
#define PUTCHAR putchar
#define IRQ_ENABLE(x)
#endif

#include <stdbool.h>
#include "pplibc.h"

// macros do compilador para funções variádicas (usadas no printk)
#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg

// estado interno do gerador de números pseudoaleatórios
// https://en.wikipedia.org/wiki/Linear_congruential_generator
static unsigned int rand_number = 0;

//----------------------------------------------------------------------

void randseed(unsigned long seed)
{
    rand_number = seed;
}

//----------------------------------------------------------------------

unsigned long randnum(void)
{
    // gerador pseudo-aleatorio simples por congruência linear
    rand_number = (1664525L * rand_number + 1013904223L) % (1L << 31);
    return (rand_number);
}

//----------------------------------------------------------------------

int abs(int num)
{
    if (num < 0)
        return (-num);
    else
        return (num);
}

//----------------------------------------------------------------------

int mem_copy(const char *orig, char *dest, int size)
{
    if (!orig || !dest || size <= 0)
        return (ERROR);

    for (int i = 0; i < size; i++)
        dest[i] = orig[i];

    return (NOERROR);
}

//----------------------------------------------------------------------

void putch(char c)
{
    PUTCHAR(c);  // macro
}

//----------------------------------------------------------------------

void putst(const char *s)
{
    if (!s)
        s = "NULL";

    while (*s)
    {
        PUTCHAR(*s);  // macro
        s++;
    }
}

//----------------------------------------------------------------------

// adaptado de https://operating-system-in-1000-lines.vercel.app/en/
void printk(const char *fmt, ...)
{
    bool left_align;
    //bool fill_zeros = false;
    //bool signal_space = false;
    short width;
    va_list vargs;

    // inibe "interrupções" do hardware
    IRQ_ENABLE(0);

    va_start(vargs, fmt);

    while (*fmt)
    {
        // find a % mark, process it
        if (*fmt == '%')
        {
            // skip '%'
            fmt++;

            // space for signal (% )
            /*if (*fmt == ' ')
            {
                signal_space = true;
                fmt++;
            }*/

            // left alignment (%-)
            left_align = false;
            if (*fmt == '-')
            {
                left_align = true;
                fmt++;
            }

            // fill with zeros (%0)
            /*if (*fmt == '0')
            {
                fill_zeros = true;
                fmt++;
            }*/

            // width modifier (%NN)
            width = 0;
            while (*fmt && *fmt >= '1' && *fmt <= '9')
            {
                width = 10 * width + (*fmt - '0');
                fmt++;
            }

            switch (*fmt)
            {

            // "%%" or '%' at the end of the format string
            case '%':
            case '\0':
                putch('%');
                break;

            // %c: char
            case 'c':
            {
                int c = va_arg(vargs, int);
                putch(c);
                break;
            }

            // %s: string
            case 's':
            {
                const char *s = va_arg(vargs, const char *);
                char *aux;

                // null string
                if (!s)
                s = "(null)";

                // decrement width according to string size
                aux = (char *) s;
                while (*aux)
                {
                    width--;
                    aux++;
                }

                // print whitespaces before
                if (!left_align)
                    while (width > 0)
                    {
                        putch(' ');
                        width--;
                    }

                // print string chars
                while (*s)
                {
                    putch(*s);
                    s++;
                }

                // print whitespaces after
                if (left_align)
                    while (width > 0)
                    {
                        putch(' ');
                        width--;
                    }

                break;
            }

            // %d %i: signed integer, printed in decimal
            case 'i':
            case 'd':
            {
                bool negative;
                int divisor;
                int value = va_arg(vargs, int);

                width--;

                // treat negative number
                negative = false;
                if (value < 0)
                {
                    value = -value;
                    negative = true;
                    width--;
                }

                // calculate number of digits to print
                divisor = 1;
                while (value / divisor > 9)
                {
                    divisor *= 10;
                    width--;
                }

                // print whitespaces before
                if (!left_align)
                    while (width > 0)
                    {
                        putch(' ');
                        width--;
                    }

                // print number signal
                if (negative)
                    putch('-');

                // print number digits
                while (divisor > 0)
                {
                    putch('0' + value / divisor);
                    value   %= divisor;
                    divisor /= 10;
                }

                // print whitespaces after
                if (left_align)
                    while (width > 0)
                    {
                        putch(' ');
                        width--;
                    }

                break;
            }

            // %u: unsigned integer, printed in decimal
            case 'u':
            {
                int divisor;
                unsigned int value = va_arg(vargs, unsigned int);

                width--;

                // calculate number of digits to print
                divisor = 1;
                while (value / divisor > 9)
                {
                    divisor *= 10;
                    width--;
                }

                // print whitespaces before
                if (!left_align)
                    while (width > 0)
                    {
                        putch(' ');
                        width--;
                    }

                // print number digits
                while (divisor > 0)
                {
                    putch('0' + value / divisor);
                    value   %= divisor;
                    divisor /= 10;
                }

                // print whitespaces after
                if (left_align)
                    while (width > 0)
                    {
                        putch(' ');
                        width--;
                    }

                break;
            }

            // %p: long, printed in hexadecimal
            case 'p':
            {
                unsigned nibble;
                unsigned long value = va_arg(vargs, unsigned long);

                if (value)
                {
                    putst("0x");
                    for (int i = 15; i >= 0; i--)
                    {
                        nibble = (value >> (i * 4)) & 0xf;
                        putch("0123456789abcdef"[nibble]);
                    }
                }
                else
                    putst("(nil)");

                break;
            }

            // %? unrecognized
            default:
            {
                putst("%?");
            }
            }
        }
        // not a placeholder, just print it
        else
        {
            putch(*fmt);
        }

        // take the next char in format
        if (*fmt)
            fmt++;
    }
    va_end(vargs);

    // libera "interrupções" do hardware
    IRQ_ENABLE(1);
}

//----------------------------------------------------------------------
