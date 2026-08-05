// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Emulação do disco virtual, que simula um disco rígido.

// Atenção: deve ser usado o flag de ligação -lrt, para ligar com a
// biblioteca POSIX de tempo real, pois o disco simulado usa timers POSIX.

// verifica sistema operacional
#ifndef __linux__
#error "Disk emulator library is designed specifically for Linux OS."
#endif

#ifndef __HW_DISK__
#define __HW_DISK__

// estados internos do disco
#define DISK_STATUS_UNKNOWN 0 // disco não iniciado
#define DISK_STATUS_IDLE 1    // disco livre
#define DISK_STATUS_READ 2    // disco ocupado fazendo leitura
#define DISK_STATUS_WRITE 3   // disco ocupado fazendo escrita

// operações oferecidas pelo disco
#define DISK_CMD_INIT 0       // inicia o disco
#define DISK_CMD_READ 1       // leitura de bloco do disco
#define DISK_CMD_WRITE 2      // escrita de bloco do disco
#define DISK_CMD_STATUS 3     // consulta status do disco
#define DISK_CMD_DISKSIZE 4   // consulta tamanho do disco em blocos
#define DISK_CMD_BLOCKSIZE 5  // consulta tamanho de bloco em bytes
#define DISK_CMD_DELAYMIN 6   // consulta tempo resposta mínimo (ms)
#define DISK_CMD_DELAYMAX 7   // consulta tempo resposta máximo (ms)
#define DISK_CMD_COUNTREAD 8  // consulta contador de leituras
#define DISK_CMD_COUNTWRITE 9 // consulta contador de escritas

// solicita uma operação ao disco virtual
int hw_disk(int cmd, int block, void *buffer);

/*
Este código simula a operação e interface de um disco rígido de computador,
que permite a leitura e escrita sequencial de blocos de dados, usando a
interface descrita acima. O conteúdo do disco simulado é armazenado em um
arquivo no sistema operacional subjacente, sendo portanto preservado de
uma execução para outra.

O disco opera sequencialmente (realiza somente uma operação por vez) e a
maioria das operações é síncrona, retornando o resultado ao concluir.
Todavia, as operações de leitura e escrita de blocos de dados são atendidas
de forma assíncrona, ou seja: o disco responde à solicitação imediatamente,
mas a operação de E/S em si demora um pouco mais; quando ela for completada,
o disco irá informar isso através de uma interrupção (IRQ). Essa interrupção
deve então ser tratada pelo driver de disco, para acordar a tarefa que
solicitou a operação de E/S.

Enquanto o disco estiver ocupado em uma leitura/escrita, o status do disco
será DISK_STATUS_READ ou DISK_STATUS_WRITE; solicitações de leitura ou
escrita com o disco ocupado retornarão com erro.

Exemplos de uso:

inicia um disco (operação síncrona)
int disk_cmd(DISK_CMD_INIT, 0, char *disk_image);
disk_image: caminho do arquivo que contém a imagem do disco
result < 0: erro
result = 0: disco corretamente iniciado

consulta status do disco (operação síncrona)
int disk_cmd(DISK_CMD_STATUS, 0, 0);
result = 0: erro (disco não iniciado ou inexistente)
result = DISK_STATUS_IDLE: disco livre
result = DISK_STATUS_READ: disco ocupado realizando leitura
result = DISK_STATUS_WRITE: disco ocupado realizando escrita

consulta tamanho do disco (operação síncrona)
int disk_cmd(DISK_CMD_DISKSIZE, 0, 0);
result <  0: erro
result >= 0: tamanho do disco (em blocos)

consulta tamanho de cada bloco (operação síncrona)
int disk_cmd(DISK_CMD_BLOCKSIZE, 0, 0);
result <  0: erro
result >= 0: tamanho de cada bloco (em bytes)

consulta tempo de resposta mínimo (operação síncrona)
int disk_cmd(DISK_CMD_DELAYMIN, 0, 0);
result <  0: erro
result >= 0: tempo de resposta mínimo do disco (em ms)

consulta tempo de resposta máximo (operação síncrona)
int disk_cmd(DISK_CMD_DELAYMAX, 0, 0);
result <  0: erro
result >= 0: tempo de resposta máximo do disco (em ms)

agenda a leitura de um bloco de disco (operação assíncrona)
int disk_cmd(DISK_CMD_READ, int block, void *buffer);
block: número do bloco a ser lido
buffer: endereço do buffer para os dados lidos
result < 0: erro
result = 0: ok (leitura agendada, IRQ_DISK será gerada ao completar)

agenda a escrita de um bloco de disco (operação assíncrona)
int disk_cmd(DISK_CMD_WRITE, int block, void *buffer);
block: número do bloco a ser escrito
buffer: endereço do buffer com os dados a escrever
result < 0: erro
result = 0: ok (escrita agendada, IRQ_DISK será gerada ao completar)
*/

#endif
