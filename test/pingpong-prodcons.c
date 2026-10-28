#include "lib/pplibc.h"
#include "syscall.h"
#include "kernel/semaphore.h"
#include "kernel/task.h"
#include "lib/queue.h"
#include "kernel/memory.h"
#include "kernel/time.h"

int s_vaga, s_buffer, s_item;                                                       // Semáforos
struct queue_t *buffer;                                                             // Buffer compartilhado entre produtores e consumidores

#define BUFFER_SIZE 5

void produtor(void *arg){
   while(true){
      task_sleep(1000);
      unsigned int *item = mem_alloc(sizeof(unsigned int));
      if(!item){
         printk("Erro ao alocar memória para item\n");
         task_exit(ERROR);
      }
      *item = randnum() % 100;                                                      // Gera um número aleatório entre 0 e 99
      int s = sem_down(s_vaga);                                                     // Decrementa o semáforo de vaga
      if(s == ERROR){
         printk("Erro ao decrementar semáforo de vaga\n");
         mem_free(item);
         task_exit(ERROR);
      }
      s = sem_down(s_buffer);                                                       // Decrementa o semáforo de buffer
      if(s == ERROR){
         printk("Erro ao decrementar semáforo de buffer\n");
         sem_up(s_vaga);
         mem_free(item);
         task_exit(ERROR);
      }
      s = queue_add(buffer, item);                                                  // Adiciona o item ao buffer
      if(s == ERROR){
         printk("Erro ao adicionar item ao buffer\n");
         sem_up(s_buffer);
         sem_up(s_vaga);
         mem_free(item);
         task_exit(ERROR);
      }

      printk("%s produziu %u\n", (char*)arg, *item);

      s = sem_up(s_buffer);                                                         // Incrementa o semáforo de buffer
      if(s == ERROR){
         printk("Erro ao incrementar semáforo de buffer\n");
         task_exit(ERROR);
      }
      s = sem_up(s_item);                                                           // Incrementa o semáforo de item
      if(s == ERROR){
         printk("Erro ao incrementar semáforo de item\n");
         task_exit(ERROR);
      }
   }
}

void consumidor(void *arg){
   while(true){
      int s = sem_down(s_item);                                                     // Decrementa o semáforo de item
      if(s == ERROR){
         printk("Erro ao decrementar semáforo de item\n");
         task_exit(ERROR);
      }
      s = sem_down(s_buffer);                                                       // Decrementa o semáforo de buffer
      if(s == ERROR){
         printk("Erro ao decrementar semáforo de buffer\n");
         task_exit(ERROR);
      }

      unsigned int *item = queue_head(buffer);                                      // Pega o item do buffer
      if(item == NULL) {
         printk("Erro: buffer vazio\n");
         if(sem_up(s_buffer) == ERROR || sem_up(s_item) == ERROR)
            task_exit(ERROR);
         continue;
      }

      s = queue_del(buffer, item);                                                  // Remove o item do buffer
      if(s == ERROR){
         printk("Erro ao remover item do buffer\n");
         if(sem_up(s_buffer) == ERROR || sem_up(s_item) == ERROR)
            task_exit(ERROR);
         continue;
      }

      s = sem_up(s_buffer);                                                         // Incrementa o semáforo de buffer
      if(s == ERROR){
         printk("Erro ao incrementar semáforo de buffer\n");
         task_exit(ERROR);
      }

      s = sem_up(s_vaga);                                                           // Incrementa o semáforo de vaga
      if(s == ERROR){
         printk("Erro ao incrementar semáforo de vaga\n");
         task_exit(ERROR);
      }
      printk("                   %s consumiu %u\n", (char*)arg, *item);
      if(mem_free(item) == ERROR){
         printk("Erro ao liberar memória do item\n");
         task_exit(ERROR);
      }
      task_sleep(1000);
   }
}


// corpo da tarefa principal
void user_main(void *arg){
   randseed(time());                                                                // Inicializa o gerador de números aleatórios com o tempo atual
   buffer = queue_create();
   if(!buffer){
      printk("Erro ao criar buffer\n");
      task_exit(ERROR);
   }

   s_vaga = sem_create(BUFFER_SIZE);                                                // Cria semáforo de vaga com valor inicial igual ao tamanho do buffer
   s_buffer = sem_create(1);                                                        // Cria semáforo de buffer com valor inicial 1
   s_item = sem_create(0);                                                          // Cria semáforo de item com valor inicial 0
   if(s_vaga == ERROR || s_buffer == ERROR || s_item == ERROR){
      printk("Erro ao criar semáforos\n");
      if(s_vaga != ERROR) sem_destroy(s_vaga);
      if(s_buffer != ERROR) sem_destroy(s_buffer);
      if(s_item != ERROR) sem_destroy(s_item);
      queue_destroy(buffer);
      task_exit(ERROR);
   }

   struct task_t *p1 = task_create("p1", produtor, "p1");
   struct task_t *p2 = task_create("p2", produtor, "p2");
   struct task_t *p3 = task_create("p3", produtor, "p3");
   struct task_t *c1 = task_create("c1", consumidor, "c1");
   struct task_t *c2 = task_create("c2", consumidor, "c2");

   if(p1 == NULL || p2 == NULL || p3 == NULL || c1 == NULL || c2 == NULL){
      printk("Erro ao criar tarefas\n");
      task_exit(ERROR);
   }


   task_exit(NOERROR);
}