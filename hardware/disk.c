// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Emulação do disco virtual, que simula um disco rígido.

// para depurar a operação do disco
//#define DEBUG_DISK 1

// padrão de API UNIX a usar (para sigaction)
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "cpu.h"
#include "disk.h"

#define NOERROR 0
#define ERROR -1

// parâmetros de operação do disco simulado
#define DISK_BLOCK_SIZE 64   // tamanho de cada bloco, em bytes
#define DISK_DELAY_MIN 30    // atraso mínimo, em milissegundos
#define DISK_DELAY_MAX 300   // atraso máximo, em milissegundos
#define DISK_SIGNAL SIGRTMIN // sinal a ser usado no timer interno

//----------------------------------------------------------------------

// estrutura com os dados internos do disco (estado inicial desconhecido)
struct disk_t
{
    int status;               // estado do disco
    char *file;               // nome do arquivo que simula o disco
    int fd;                   // descritor do arquivo que simula o disco
    int numblocks;            // numero de blocos do disco
    int blocksize;            // tamanho dos blocos em bytes
    char *buffer;             // buffer da próxima operação (read/write)
    int prev_block;           // bloco da ultima operação
    int next_block;           // bloco da próxima operação
    int delay_min, delay_max; // tempos de acesso mínimo e máximo
    int count_read,           // contador de leituras
        count_write;          // contador de escritas
    timer_t timer;            // timer que simula o tempo de acesso
    struct itimerspec delay;  // struct do timer de tempo de acesso
    struct sigevent sigev;    // evento associado ao timer
    struct sigaction signal;  // tratador de sinal do timer
};

// disco
static struct disk_t disk = {.status = DISK_STATUS_UNKNOWN};

//----------------------------------------------------------------------

// arma o timer que simula o tempo de acesso ao disco;
// ao disparar, ele gera um sinal DISK_SIGNAL
static void disk_settimer()
{
    int time_ms;

    // tempo no intervalo [DISK_DELAY_MIN ... DISK_DELAY_MAX], proporcional a
    // distancia entre o próximo bloco a ler (next_block) e a ultima leitura
    // (prev_block), somado a um pequeno fator aleatorio
    time_ms = abs(disk.next_block - disk.prev_block) *
                  (disk.delay_max - disk.delay_min) /
                  disk.numblocks +
              disk.delay_min +
              random() % (disk.delay_max - disk.delay_min) / 10;

    #ifdef DEBUG_DISK
    printf("DISK: [From block %d to block %d in %d ms]\n",
                   disk.prev_block, disk.next_block, time_ms);
    #endif

    // primeiro disparo, em nanossegundos,
    disk.delay.it_value.tv_nsec = time_ms * 1000000;

    // primeiro disparo, em segundos
    disk.delay.it_value.tv_sec = time_ms / 1000;

    // próximos disparos não ocorrem (disparo único)
    disk.delay.it_interval.tv_nsec = 0;
    disk.delay.it_interval.tv_sec = 0;

    // arma o timer
    if (timer_settime(disk.timer, 0, &disk.delay, NULL) == -1)
    {
        perror("DISK:");
        abort();
    }
    #ifdef DEBUG_DISK
    printf("DISK: timer is set\n");
    #endif
}

//----------------------------------------------------------------------

// trata o sinal do timer que simula o tempo de acesso ao disco
static void disk_sighandle(int)
{
    #ifdef DEBUG_DISK
    printf("DISK: signal received\n");
    printf("fd: %d, block %d, size %d\n", disk.fd, disk.next_block,
           disk.blocksize);
    #endif

    // verificar qual a operação pendente e realizá-la
    switch (disk.status)
    {
    case DISK_STATUS_READ:
        // faz a leitura previamente agendada
        pread(disk.fd, disk.buffer, disk.blocksize,
              disk.next_block * disk.blocksize);
        disk.count_read++;
        break;

    case DISK_STATUS_WRITE:
        // faz a escrita previamente agendada
        pwrite(disk.fd, disk.buffer, disk.blocksize,
              disk.next_block * disk.blocksize);
        disk.count_write++;
        break;

    default:
        // erro: estado desconhecido
        perror("DISK: unknown disk state");
        abort();
    }

    // guarda numero de bloco da ultima operação
    disk.prev_block = disk.next_block;

    // disco se torna ocioso novamente
    disk.status = DISK_STATUS_IDLE;

    // gera uma IRQ virtual para o "kernel" do usuário
    raise(IRQ_DISK + SIGRTMIN);
}

//----------------------------------------------------------------------

// inicia o disco virtual contido em "disk_image"
// retorno: 0 (sucesso) ou -1 (erro)
static int disk_init(char *disk_image)
{
    // o disco já foi iniciado ?
    if (disk.status != DISK_STATUS_UNKNOWN)
        return (ERROR);

    // estado atual do disco
    disk.status = DISK_STATUS_IDLE;
    disk.next_block = disk.prev_block = 0;

    // abre o arquivo no disco (leitura/escrita, síncrono)
    disk.file = disk_image;
    disk.fd = open(disk.file, O_RDWR | O_SYNC);
    if (disk.fd < 0)
    {
        perror("DISK: ");
        abort();
    }

    // define seu tamanho em blocos
    disk.blocksize = DISK_BLOCK_SIZE;
    disk.numblocks = lseek(disk.fd, 0, SEEK_END) / disk.blocksize;

    // ajusta atrasos mínimo e máximo de acesso no disco
    disk.delay_min = DISK_DELAY_MIN;
    disk.delay_max = DISK_DELAY_MAX;

    // contadores de operações
    disk.count_read  = 0;
    disk.count_write = 0;

    // associa sinal do timer interno ao handle apropriado
    disk.signal.sa_handler = disk_sighandle;
    sigemptyset(&disk.signal.sa_mask);
    disk.signal.sa_flags = SA_NODEFER;
    sigaction(DISK_SIGNAL, &disk.signal, 0);

    // cria o timer que simula o tempo de acesso ao disco
    disk.sigev.sigev_notify = SIGEV_SIGNAL;
    disk.sigev.sigev_signo = DISK_SIGNAL;
    if (timer_create(CLOCK_REALTIME, &disk.sigev, &disk.timer) == -1)
    {
        perror("DISK:");
        abort();
    }

    #ifdef DEBUG_DISK
    printf("DISK: initialized\n");
    #endif

    return (NOERROR);
}

//----------------------------------------------------------------------

// função que implementa a interface de acesso ao disco virtual
int hw_disk(int cmd, int block, void *buffer)
{
    #ifdef DEBUG_DISK
    printf("DISK: received command %d\n", cmd);
    #endif

    switch (cmd)
    {
    // inicia o disco
    case DISK_CMD_INIT:
        return (disk_init(buffer));

    // solicita status do disco
    case DISK_CMD_STATUS:
        return (disk.status);

    // solicita tamanho do disco
    case DISK_CMD_DISKSIZE:
        if (disk.status == DISK_STATUS_UNKNOWN)
            return (ERROR);
        return (disk.numblocks);

    // solicita tamanho de bloco
    case DISK_CMD_BLOCKSIZE:
        if (disk.status == DISK_STATUS_UNKNOWN)
            return (ERROR);
        return (disk.blocksize);

    // solicita atraso mínimo
    case DISK_CMD_DELAYMIN:
        if (disk.status == DISK_STATUS_UNKNOWN)
            return (ERROR);
        return (disk.delay_min);

    // solicita atraso máximo
    case DISK_CMD_DELAYMAX:
        if (disk.status == DISK_STATUS_UNKNOWN)
            return (ERROR);
        return (disk.delay_max);

    // solicita contador de leituras
    case DISK_CMD_COUNTREAD:
        if (disk.status == DISK_STATUS_UNKNOWN)
            return (ERROR);
        return (disk.count_read);

    // solicita contador de escritas
    case DISK_CMD_COUNTWRITE:
        if (disk.status == DISK_STATUS_UNKNOWN)
            return (ERROR);
        return (disk.count_write);

    // solicita operação de leitura ou de escrita
    case DISK_CMD_READ:
    case DISK_CMD_WRITE:
        if (disk.status != DISK_STATUS_IDLE)
            return (ERROR);
        if (!buffer)
            return (ERROR);
        if (block < 0 || block >= disk.numblocks)
            return (ERROR);

        // registra que ha uma operação pendente
        disk.buffer = buffer;
        disk.next_block = block;
        if (cmd == DISK_CMD_READ)
            disk.status = DISK_STATUS_READ;
        else
            disk.status = DISK_STATUS_WRITE;

        // arma o timer que simula o atraso do disco
        disk_settimer();

        return (NOERROR);

    default:
        return (ERROR);
    }
}

//----------------------------------------------------------------------
