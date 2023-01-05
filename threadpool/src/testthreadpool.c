#include <unistd.h>
#include "thread_pool.h"
void modulate_multithreads_commit_task(thread_pool * tp);
void * produce(void * arg);
pthread_mutex_t mtx;
static int i = 0;
__attribute__((constructor)) void init();
__attribute__((destructor)) void des();

int main(int argc, char ** const argv, char ** const envp) {
    thread_pool tp;
    init_thread_pool(&tp, 3, CAPACITY, CAPACITY);
    modulate_multithreads_commit_task(&tp);
    join_thread(&tp);
    destroy_thread_pool(&tp);
    return 0;
}

void * foo1(void * arg) {
    printf("%s %lu\n", __func__, (unsigned long)arg);
    return NULL;
}

void * foo2(void * arg) {
    unsigned long i = (unsigned long)arg;
    printf("%s %lu\n", __func__, i * i);
    return NULL;
}

void modulate_multithreads_commit_task(thread_pool * tp) {
    pthread_t thread_id;
    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);
    handle_error(ret, "pthread_attr_init");
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int n = 10;
    while (n--) {
        ret = pthread_create(&thread_id, &attr, produce, tp);
        handle_error(ret, "pthread_create");
    }
}

void * produce(void * arg) {
    thread_pool * resrc = (thread_pool *)arg;
    while (1) {
        task t;
        pthread_mutex_lock(&mtx);
        if (i % 2) {
            init_task(&t, foo1, (void *)i++);
        } else {
            init_task(&t, foo2, (void *)i++);
        }
        pthread_mutex_unlock(&mtx);
        commit_task(resrc, &t);
        //测试用
        sleep(5);
    }
}

void init() {
    pthread_mutexattr_t mutexattr;
    int ret = pthread_mutexattr_init(&mutexattr);
    handle_error(ret, "pthread_mutexattr_init");
    ret = pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_PRIVATE);
    handle_error(ret, "pthread_mutexattr_setpshared");
    ret = pthread_mutex_init(&mtx, &mutexattr);
    handle_error(ret, "pthread_mutex_init");
    ret = pthread_mutexattr_destroy(&mutexattr);
    handle_error(ret, "pthread_mutexattr_destroy");
}

void des() {
    pthread_mutex_destroy(&mtx);
}