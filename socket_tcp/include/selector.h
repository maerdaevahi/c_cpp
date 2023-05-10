#ifndef SELECTOR
#define SELECTOR
#include "app.h"

#define CLIENT_FDS_SIZE (FD_SETSIZE - 4)

struct selector {
    int listen_fd;
    fd_set readfds;
    int max_fd;
    int client_fds[CLIENT_FDS_SIZE];
    int max_pos;
    acqure ac;
    communicate com;
    selector(const net_app * na);
    void add_select_info(int client_fd);
    void remove_select_info(int i);
    int process_listen_fd_if_necessary(fd_set * readfds_param, int nready);
    void process_client_fd_if_necessary(fd_set * readfds_param, int nready);
    void work();
};
#endif