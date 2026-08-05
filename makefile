# PingPongOS - PingPong Operating System
# © Prof. Carlos A. Maziero, DINF UFPR
# Versão 2.1 -- 06/2026

# ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
# ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

# flags de compilação e ligação
CC       = gcc
CPPFLAGS =
CFLAGS   = -std=c99 -Wall -Wextra -O0 -g3 -Wno-unused-parameter
LDFLAGS  = -z noexecstack
LDLIBS   = -lrt -lm
export CC CPPFLAGS CFLAGS LDFLAGS LDLIBS

# arquivo .tgz para envio
TGZ = ppos-${USER}.tgz

# estes alvos não são arquivos
.PHONY: all help debug clean purge hardware lib kernel reset

# regra default
help:
	@echo "Makefile para compilação do PPOS"
	@echo "Uso:"
	@echo "  make p1          : compila o projeto p1 (idem para p2, ...)"
	@echo "  make debug p1    : idem, com o flag -DDEBUG"
	@echo "  make clean       : remove objetos e backups"
	@echo "  make purge       : remove objetos, backups e executáveis"
	@echo "  make tgz         : gera um arquivo .tgz para entrega"
	@echo "  make reset       : reinicia o conteúdo do disco simulado"

# programas de teste de cada projeto
P1  = pingpong-task1 pingpong-task2 pingpong-task3
P2  = pingpong-dispatcher
P3  = pingpong-scheduler
P4  = pingpong-preempcao
P5  = pingpong-contab pingpong-contab-prio pingpong-contab-stress
P6  = pingpong-wait pingpong-wait-stress
P7  = pingpong-sleep
P8  = pingpong-semaphore pingpong-semaphore-stress
P9  = pingpong-prodcons
P10 = pingpong-mqueue
P11 = pingpong-memoria pingpong-mqueue
P12 = pingpong-disco pingpong-disco-stress
P13 = pingpong-disco-stress
P14 = pingpong-cache-on pingpong-cache-off

BIN = $(P1) $(P2) $(P3) $(P4) $(P5) $(P6) $(P7) $(P8) \
      $(P9) $(P10) $(P11) $(P12) $(P13) $(P14)

# alvos: testes de cada projeto; como criar uma receita genérica aqui?
p1:  $(P1)
p2:  $(P2)
p3:  $(P3)
p4:  $(P4)
p5:  $(P5)
p6:  $(P6)
p7:  $(P7)
p8:  $(P8)
p9:  $(P9)
p10: $(P10)
p11: $(P11)
p12: $(P12)
p13: $(P13)
p14: $(P14)

# compila e liga um programa de teste ping*
ping%: hardware kernel lib
	@echo "=== Compilando $@ ==="
	$(MAKE) -C test $@
	@echo "=== Ligando $@ ==="
	$(eval OBJS = $(wildcard hardware/*.o kernel/*.o lib/*.o))
	$(CC) $(LDFLAGS) $(OBJS) test/$@.o -o $@ $(LDLIBS)

# compila o hardware virtual, bibliotecas e kernel
hardware kernel lib:
	@echo "=== Compilando $@ ==="
	-test -s $@/makefile && $(MAKE) -C $@

# ativa modo de depuração
debug:
	$(eval CPPFLAGS += -DDEBUG)

# limpa arquivos temporários
clean:
	-rm -f *.o *~ */*.o */*~

# reinicia o conteúdo do disco virtual
reset:
	-test -s hardware/makefile && $(MAKE) -C hardware reset

# limpa tudo, deixa só o o código-fonte
purge: clean
	-rm -f $(BIN) $(TGZ)

# gera arquivo .tgz para entrega
tgz: purge
	-rm -f $(TGZ)
	-tar --ignore-failed-read -czvf $(TGZ) lib/ kernel/ test/$(P9).c
	@echo ""
	@echo "Arquivo $(TGZ) pronto para entrega"
