# PingPongOS - PingPong Operating System
# © Prof. Carlos A. Maziero, DINF UFPR
# Versão 2.1 -- 06/2026

# ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
# ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

# flags são importados do makefile superior

# flags adicionais
CPPFLAGS += -I../ -I../lib/

# arquivos a compilar
HDR = $(wildcard *.h)			# headers
SRC = $(wildcard *.c)			# fontes C
ASM = $(wildcard *.s)			# fontes assembly
OBJ = $(patsubst %.c,%.o,$(SRC))	# objetos .o
OBJ+= $(patsubst %.s,%.o,$(ASM))	# idem

# regra default: reconstruir tudo
all   : $(OBJ)
$(OBJ): $(SRC) $(HDR)
