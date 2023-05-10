#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "common_macro.h"
#include "selector.h"
// void FD_CLR(int fd, fd_set *set);
// int  FD_ISSET(int fd, fd_set *set);
// void FD_SET(int fd, fd_set *set);
// void FD_ZERO(fd_set *set);

selector::selector(const net_app * na) {
    listen_fd = na->fd;
    ac = na->ac;
    com = na->com;
    FD_ZERO(&readfds);
    FD_SET(listen_fd, &readfds);
    max_fd = listen_fd;
    for (int i = 0; i < CLIENT_FDS_SIZE; ++i) {
        client_fds[i] = -1; 
    }
    max_pos = -1;
}

void selector::add_select_info(int client_fd) {
    FD_SET(client_fd, &readfds);
    if (max_fd < client_fd) {
        max_fd = client_fd;
    }
    int i;
    for (i = 0; i < CLIENT_FDS_SIZE; ++i) {
        if (client_fds[i] == -1) {
            client_fds[i] = client_fd;
            if (max_pos < i) {
                max_pos = i;
            }
            break;
        } 
    }
    if (i == CLIENT_FDS_SIZE) {
        fprintf(stderr, "too many client\n");
        exit(1);
    }
}

void selector::remove_select_info(int i) {
    close(client_fds[i]);
    FD_CLR(client_fds[i], &readfds);
    client_fds[i] = -1;
}

int selector::process_listen_fd_if_necessary(fd_set * readfds_param, int nready) {
    if (FD_ISSET(listen_fd, readfds_param)) {
        int client_fd = ac(listen_fd);
        add_select_info(client_fd);
        --nready;
    }
    return nready;
}

void selector::process_client_fd_if_necessary(fd_set * readfds_param, int nready) {
    for (int i = 0; nready > 0 && i <= max_pos; ++i) {
        if (client_fds[i] == -1) {
            continue;
        }
        if (FD_ISSET(client_fds[i], readfds_param)) {
            char buf[BUFSIZ] = "";
            int count;
            if ((count = read(client_fds[i], buf, sizeof(buf))) > 0) {
                printf("%s", buf);
                count = write(client_fds[i], buf, count);
                if (count == -1) {
                    perror("write");
                    remove_select_info(i);
                }
            }
            if (count == -1) {
                perror("read");
                remove_select_info(i);
            } else if (count == 0) {
                printf("peer close\n");
                remove_select_info(i);
            }
        }
        --nready;       
    }
}

void selector::work()  {
    printf("%s\n", __func__);
    for (;;) {
        fd_set readfds_param = readfds;
        int nready = select(max_fd + 1, &readfds_param, NULL, NULL, NULL);
        if (nready == -1) {
            PERROR_EXIT("select", 1);
        } else if (!nready) {
            continue;
        } else if (nready > 0) {
            nready = process_listen_fd_if_necessary(&readfds_param, nready);
            process_client_fd_if_necessary(&readfds_param, nready);
        } else {
            fprintf(stderr, "unknown error\n");
            exit(1);
        }
    } 
}