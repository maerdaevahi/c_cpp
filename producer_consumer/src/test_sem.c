#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "test_sem.h"
#include "handle_error.h"
void * produce_sem(void * arg) {
    semaphore_resource * resrc = (semaphore_resource *)arg;
    pthread_t thr_id = pthread_self();
    printf("thread [%ld] %s %p\n", thr_id, __func__, resrc);
    static int init = 0;
    while (1) {
        sem_wait(&resrc->unused);
        sem_wait(&resrc->mutex);
        add_into_cycle_queue(&resrc->cq, ++init);
        printf("%ld write %d: %d\n", thr_id, resrc->cq.end, init);
        sem_post(&resrc->mutex);
        sem_post(&resrc->used);
        sleep(2);
    }
}

void * consume_sem(void * arg) {
    semaphore_resource * resrc = (semaphore_resource *)arg;
    pthread_t thr_id = pthread_self();
    printf("thread [%ld] %s %p\n", thr_id, __func__, resrc);
    while (1) {
        sem_wait(&resrc->used);
        sem_wait(&resrc->mutex);
        int e = remove_from_cycle_queue(&resrc->cq);
        printf("%ld read %d: %d\n", thr_id, resrc->cq.start, e);
        sem_post(&resrc->mutex);
        sem_post(&resrc->unused);
    }
}

void init_semaphore_resource(semaphore_resource * resrc) {
    init_cycle_queue(&resrc->cq);
    sem_init(&resrc->mutex, 0, 1);
    sem_init(&resrc->used, 0, 0);
    sem_init(&resrc->unused, 0, CAPACITY);
}

void destroy_semaphore_resource(semaphore_resource * resrc) {
    sem_destroy(&resrc->mutex);
    sem_destroy(&resrc->used);
    sem_destroy(&resrc->unused);
}

void test_sem() {
    semaphore_resource resrc;
    init_semaphore_resource(&resrc);
    pthread_t thread;
    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);
    handle_error(ret, "pthread_attr_init");
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int n = 2;
    while (n--) {
        ret = pthread_create(&thread, &attr, produce_sem, &resrc);
        handle_error(ret, "pthread_create");
    }
    n = 3;
    while (n--) {
        ret = pthread_create(&thread, &attr, consume_sem, &resrc);
        handle_error(ret, "pthread_create");    
    }
    
    printf("thread [%ld] %s %p\n", pthread_self(), __func__, &resrc);
    pause();
    destroy_semaphore_resource(&resrc);
}