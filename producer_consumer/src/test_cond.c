#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "test_cond.h"
#include "handle_error.h"
void test_cond() {
    cond_resource resrc;
    init_cond_resource(&resrc);
    pthread_t thread;
    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);
    handle_error(ret, "pthread_attr_init");
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int n = 2;
    while (n--) {
        ret = pthread_create(&thread, &attr, produce, &resrc);
        handle_error(ret, "pthread_create");
    }
    n = 3;
    while (n--) {
        ret = pthread_create(&thread, &attr, consume, &resrc);
        handle_error(ret, "pthread_create");    
    }
    
    printf("thread [%ld] %s %p\n", pthread_self(), __func__, &resrc);
    pause();
    destroy_cond_resource(&resrc);
}

void * produce(void * arg) {
    cond_resource * resrc = (cond_resource *)arg;
    pthread_t thr_id = pthread_self();
    printf("thread [%ld] %s %p\n", thr_id, __func__, resrc);
    static int init = 0;
    while (1) {
        pthread_mutex_lock(&resrc->mutex);
        while (test_cycle_queue_full(&resrc->cq)) {
            pthread_cond_wait(&resrc->not_full, &resrc->mutex);
        }
        add_into_cycle_queue(&resrc->cq, ++init);
        printf("%ld write %d: %d\n", thr_id, resrc->cq.end, init);
        pthread_cond_signal(&resrc->not_empty);
        pthread_mutex_unlock(&resrc->mutex);
        sleep(2);
    }
}

void * consume(void * arg) {
    cond_resource * resrc = (cond_resource *)arg;
    pthread_t thr_id = pthread_self();
    printf("thread [%ld] %s %p\n", thr_id, __func__, resrc);
    while (1) {
        pthread_mutex_lock(&resrc->mutex);
        while (test_cycle_queue_empty(&resrc->cq)) {
            pthread_cond_wait(&resrc->not_empty, &resrc->mutex);
        }
        int e = remove_from_cycle_queue(&resrc->cq);
        printf("%ld read %d: %d\n", thr_id, resrc->cq.start, e);
        pthread_cond_signal(&resrc->not_full);
        pthread_mutex_unlock(&resrc->mutex);
    }
}



void init_cond_resource(cond_resource * resrc) {
    init_cycle_queue(&resrc->cq);
    pthread_mutexattr_t mutexattr;
    int ret = pthread_mutexattr_init(&mutexattr);
    handle_error(ret, "pthread_mutexattr_init");
    ret = pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_PRIVATE);
    handle_error(ret, "pthread_mutexattr_setpshared");
    ret = pthread_mutex_init(&resrc->mutex, &mutexattr);
    handle_error(ret, "pthread_mutex_init");
    ret = pthread_mutexattr_destroy(&mutexattr);
    handle_error(ret, "pthread_mutexattr_destroy");

    pthread_condattr_t condattr;
    ret = pthread_condattr_init(&condattr);
    handle_error(ret, "pthread_condattr_init");
    ret = pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_PRIVATE);
    handle_error(ret, "pthread_condattr_setpshared");
    pthread_cond_t not_empty, not_full;
    ret = pthread_cond_init(&resrc->not_empty, &condattr);
    handle_error(ret, "pthread_cond_init");
    ret = pthread_cond_init(&resrc->not_full, &condattr);
    handle_error(ret, "pthread_cond_init");
    ret = pthread_condattr_destroy(&condattr);
    handle_error(ret, "pthread_condattr_destroy");    
}

void destroy_cond_resource(cond_resource * resrc) {
    pthread_mutex_destroy(&resrc->mutex);
    pthread_cond_destroy(&resrc->not_empty);
    pthread_cond_destroy(&resrc->not_full);
}