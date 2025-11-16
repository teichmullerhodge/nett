#include "suspend.h"


#include <stdatomic.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


#define SUSPEND_THREAD_POOL_SIZE 12
sem_t thread_semaphore;
atomic_int sem_initialized = 0;

void init_semaphore_once() {

  if(sem_initialized == 0) {
   atomic_fetch_add(&sem_initialized, 1);
   sem_init(&thread_semaphore, 0, SUSPEND_THREAD_POOL_SIZE); 
  }
}

Suspend *init_suspend(){

  Suspend *s = malloc(sizeof(Suspend));
  if(s == NULL) {
    perror("Malloc failed at init_suspend");
    return NULL;
  }

  s->status = SUSPEND_IDLE;
  s->cb = NULL;
  s->data = NULL;
  return s;
}

void suspend_destroy(Suspend *s){
  if(s != NULL) free(s);
}


void *suspend_run(void *args) {
  Suspend *s = (Suspend*)args;
  s->data = s->cb();
  sem_post(&thread_semaphore);
  s->status = SUSPEND_DONE;
  return s;
}

int suspend_parallel(Suspend *s){


  init_semaphore_once();
  sem_wait(&thread_semaphore);
  pthread_t thread_id;
  int code = pthread_create(&thread_id, NULL, suspend_run, s);
  if(code != 0) {
    s->status = SUSPEND_ERROR;
    perror("Error in pthread_create");
    return 1;
  }
  s->status = SUSPEND_AWAITING;
  pthread_detach(thread_id);
  return 0;

}


void await(pthread_t id){
  pthread_join(id, NULL);
}

Eventually suspend(Suspend *s){

  init_semaphore_once();
  sem_wait(&thread_semaphore);
  pthread_t thread_id;
  int code = pthread_create(&thread_id, NULL, suspend_run, s);
  if(code != 0) {
    s->status = SUSPEND_ERROR;
    perror("Error in pthread_create");
    return (Eventually){0};
  }
  s->status = SUSPEND_AWAITING;
  return (Eventually){ thread_id, await };
}

