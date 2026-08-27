# Ping-Pong Operating System — PPOS

**Versão 2.1** · Julho de 2026

Este repositório contém o código-fonte do **Sistema Operacional Ping-Pong (PPOS)**, um projeto didático ("toy operating system") desenvolvido para fins de aprendizado.

## Sobre o projeto

Diferente de projetos similares, o PPOS executa inteiramente dentro de um processo Linux. Rodar o SO em modo usuário permite o uso de ferramentas padrão de depuração, como **Valgrind** e **GDB**, reduzindo a curva de aprendizado.

O PPOS segue a estrutura típica de um **RTOS** (Real-Time Operating System): um sistema operacional com espaço de memória único, no qual núcleo e aplicações são compilados juntos em uma única imagem binária.

## Funcionalidades

### Suportadas

- Tarefas com troca de contexto
- Escalonador FCFS e por prioridades
- Preempção por tempo (quantum)
- Gestão do tempo e contabilização
- Sincronização de tarefas
- Semáforos genéricos
- Filas de mensagens
- Alocador de memória heap
- Driver de disco
- Escalonador de E/S

### Em desenvolvimento

- Cache de blocos para o driver de disco
- Sistema de arquivos (inodes, diretórios, nomes)
- Driver de interface de rede

### Não suportadas

- Memória virtual
- Separação userspace/kernelspace
- Chamadas de sistema usando traps

## Organização do desenvolvimento

O desenvolvimento do PPOS é organizado em **projetos incrementais**, cada um implementando uma funcionalidade do sistema. Em geral, cada projeto depende de funcionalidades desenvolvidas em projetos anteriores.

Detalhes de cada projeto: [página do docente](http://wiki.inf.ufpr.br/maziero).

## Estrutura de diretórios

```
hardware/   hardware virtual (emuladores)
lib/        bibliotecas de apoio
kernel/     código-fonte do PPOS, a ser desenvolvido pelo aluno
test/       programas de teste dos projetos
makefile    para compilar o sistema
```

## Observações importantes

- Alguns arquivos — sobretudo headers, arquivos em `hardware/`, a mini-biblioteca C e os makefiles — **já estão prontos e não devem ser alterados** (restrição indicada no início de cada arquivo).

---

**Prof. Carlos A. Maziero**, DINF UFPR
