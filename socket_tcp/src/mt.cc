#include <pthread.h>
#include "mt.h"
#include "common_macro.h"
#include "http.h"

void work_multiple_thread(const net_app * proto)  {
    int listen_fd = proto->fd;
    acqure ac = proto->ac;
    communicate com = proto->com;
    printf("%s\n", __func__);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    for (;;) {
        int client_fd = ac(listen_fd);
        pthread_t newthread;
        int ret = pthread_create(&newthread, &attr, (void *(*)(void *))com, (void *)(long)client_fd);
        HANDLE_ERROR_CONDITIONAL(ret, "pthread_create", (void *)1);
        printf("thread %ld started\n", newthread);
    } 
}