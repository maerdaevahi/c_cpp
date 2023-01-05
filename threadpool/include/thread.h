#ifndef __THREAD__
#define __THREAD__
#include <pthread.h>
typedef struct thread {
    pthread_t thread_id;
} thread;

void assign_thread(thread * t, thread * t1);
#endif