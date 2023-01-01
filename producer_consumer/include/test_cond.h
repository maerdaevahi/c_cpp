#ifndef __TEST_COND__
#define __TEST_COND__
#include <pthread.h>
#include "cycle_queue.h"
typedef struct cond_resource {
    cycle_queue cq;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty, not_full;
} cond_resource;
void * produce(void * arg);
void * consume(void * arg);
void init_cond_resource(cond_resource * resrc);
void destroy_cond_resource(cond_resource * resrc);
void test_cond();
void test_pthread_cond_timedwait();
void test_pthread_mutex_trylock();
#endif