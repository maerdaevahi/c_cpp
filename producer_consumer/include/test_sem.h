#ifndef __TEST_SEM__
#define __TEST_SEM__
#include "cycle_queue.h"
#include <semaphore.h>
typedef struct semaphore_resource {
    cycle_queue cq;
    sem_t mutex, used, unused;
} semaphore_resource;
void * produce_sem(void * arg);
void * consume_sem(void * arg);
void init_semaphore_resource(semaphore_resource * resrc);
void destroy_semaphore_resource(semaphore_resource * resrc);
void test_sem();
#endif