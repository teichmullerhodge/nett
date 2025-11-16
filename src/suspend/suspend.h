#ifndef SUSPEND_H
#define SUSPEND_H 

#include <pthread.h>

typedef enum {

 SUSPEND_IDLE,
 SUSPEND_AWAITING,
 SUSPEND_ERROR,
 SUSPEND_DONE

} SuspendStatus;


typedef struct {

  pthread_t id;
  void (*await)(pthread_t id);

} Eventually;


typedef struct {

 
 void *data;
 void* (*cb)(void);
 SuspendStatus status;


} Suspend;


Suspend *init_suspend();
Eventually suspend(Suspend *s);
void suspend_destroy(Suspend *s);
void await(pthread_t id);



#endif 
