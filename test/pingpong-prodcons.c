#include "lib/pplibc.h"
#include "syscall.h"
#include "kernel/semaphore.h"
#include "kernel/task.h"
#include "lib/queue.h"
#include "kernel/memory.h"
#include "kernel/time.h"

int s_vaga, s_mutex, s_item;                                                        // Semáforos
struct queue_t *buffer;                                                             // Buffer compartilhado entre produtores e consumidores

#define BUFFER_SIZE 5

// Função que simula o comportamento de um produtor
void produtor(void *){
   while(true){
      task_sleep(1000);                                                             // Simula um tempo de produção

      unsigned int *item = mem_alloc(sizeof(unsigned int));
      if(!item){
         printk("Erro ao alocar memória para item\n");
         task_exit(ERROR);
      }

      *item = randnum() % 100;                                                      // Gera um número aleatório entre 0 e 99

      if(sem_down(s_vaga) == ERROR){
         mem_free(item);
         task_exit(ERROR);
      }

      if(sem_down(s_mutex) == ERROR){
         sem_up(s_vaga);
         mem_free(item);
         task_exit(ERROR);
      }

      if(queue_add(buffer, item) == ERROR){                                         // Adiciona o item ao buffer
         sem_up(s_mutex);
         sem_up(s_vaga);
         mem_free(item);
         task_exit(ERROR);
      }

      printk("%s produziu %u\n", task_name(NULL), *item);

      if(sem_up(s_mutex) == ERROR) task_exit(ERROR);
      if(sem_up(s_item)== ERROR) task_exit(ERROR);
      
   }
}


// Função que simula o comportamento de um consumidor
void consumidor(void *){
   while(true){                                      
      if(sem_down(s_item) == ERROR) task_exit(ERROR);
      if(sem_down(s_mutex) == ERROR) task_exit(ERROR);
      
      unsigned int *item = queue_head(buffer);                                      // Pega o item do buffer
      if(!item){                                                                    // Verifica se o item é nulo
         if(sem_up(s_mutex) == ERROR || sem_up(s_item) == ERROR) task_exit(ERROR);  
         continue;
      }

      if(queue_del(buffer, item) == ERROR){                                         // Remove o item do buffer
         if(sem_up(s_mutex) == ERROR || sem_up(s_item) == ERROR) task_exit(ERROR);  
         continue;
      }

      if(sem_up(s_mutex) == ERROR) task_exit(ERROR);
      if(sem_up(s_vaga) == ERROR) task_exit(ERROR);
      
      printk("                   %s consumiu %u\n", task_name(NULL), *item);
      if(mem_free(item) == ERROR) task_exit(ERROR);
      
      task_sleep(1000);                                                             // Simula um tempo de consumo
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
   s_mutex = sem_create(1);                                                         // Cria semáforo de buffer com valor inicial 1
   s_item = sem_create(0);                                                          // Cria semáforo de item com valor inicial 0
   if(s_vaga == ERROR || s_mutex == ERROR || s_item == ERROR){
      if(s_vaga != ERROR) sem_destroy(s_vaga);
      if(s_mutex != ERROR) sem_destroy(s_mutex);
      if(s_item != ERROR) sem_destroy(s_item);
      queue_destroy(buffer);
      task_exit(ERROR);
   }

   struct task_t *p1 = task_create("p1", produtor, NULL);
   struct task_t *p2 = task_create("p2", produtor, NULL);
   struct task_t *p3 = task_create("p3", produtor, NULL);
   struct task_t *c1 = task_create("c1", consumidor, NULL);
   struct task_t *c2 = task_create("c2", consumidor, NULL);

   if(p1 == NULL || p2 == NULL || p3 == NULL || c1 == NULL || c2 == NULL) task_exit(ERROR);
   
   task_exit(NOERROR);
}