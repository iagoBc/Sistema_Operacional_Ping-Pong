// PingPongOS - PingPong Operating System
// © Prof. Carlos A. Maziero, DINF UFPR
// Versão 2.1 -- 06/2026

// ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
// ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

// Gerência de um dispositivo orientado a blocos.

#ifndef __PPOS_BLOCK__
#define __PPOS_BLOCK__

// inicia o subsistema de gestão do disco virtual armazenado em "disk_image"
// (chamada pelo núcleo na inicialização).
void block_init(char *disk_image);

// encerra o subsistema de gestão do disco virtual
// (chamada pelo núcleo no encerramento).
void block_term(char *disk_image);

// retorna o tamanho de cada bloco do disco, em bytes
int block_size();

// retorna o tamanho do disco, em blocos
int block_blocks();

// leitura de um bloco, do disco para o buffer
int block_read(int block, void *buffer);

// escrita de um bloco, do buffer para o disco
int block_write(int block, void *buffer);

#endif
