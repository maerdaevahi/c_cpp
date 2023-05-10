#include "select.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "common_macro.h"
// void FD_CLR(int fd, fd_set *set);
// int  FD_ISSET(int fd, fd_set *set);
// void FD_SET(int fd, fd_set *set);
// void FD_ZERO(fd_set *set);
#define CLIENT_FDS_SIZE (FD_SETSIZE - 4)

typedef struct select_info {
    int listen_fd;
    fd_set readfds;
    int max_fd;
    int client_fds[CLIENT_FDS_SIZE];
    int max_pos;
    acqure ac;
    communicate com;
} select_info;

void init_select_info(select_info * si, const net_app * na) {
    si->listen_fd = na->fd;
    si->ac = na->ac;
    si->com = na->com;
    FD_ZERO(&si->readfds);
    FD_SET(na->fd, &si->readfds);
    si->max_fd = na->fd;
    for (int i = 0; i < CLIENT_FDS_SIZE; ++i) {
        si->client_fds[i] = -1; 
    }
    si->max_pos = -1;
}

void add_select_info(select_info * si, int client_fd) {
    FD_SET(client_fd, &si->readfds);
    if (si->max_fd < client_fd) {
        si->max_fd = client_fd;
    }
    int i;
    for (i = 0; i < CLIENT_FDS_SIZE; ++i) {
        if (si->client_fds[i] == -1) {
            si->client_fds[i] = client_fd;
            if (si->max_pos < i) {
                si->max_pos = i;
            }
            break;
        } 
    }
    if (i == CLIENT_FDS_SIZE) {
        fprintf(stderr, "too many client\n");
        exit(1);
    }
}

void remove_select_info(select_info * si, int i) {
    close(si->client_fds[i]);
    FD_CLR(si->client_fds[i], &si->readfds);
    si->client_fds[i] = -1;
}

int process_listen_fd_if_necessary(select_info * si, fd_set * readfds_param, int nready) {
    if (FD_ISSET(si->listen_fd, readfds_param)) {
        int client_fd = si->ac(si->listen_fd);
        add_select_info(si, client_fd);
        --nready;
    }
    return nready;
}

void process_client_fd_if_necessary(select_info * si, fd_set * readfds_param, int nready) {
    for (int i = 0; nready > 0 && i <= si->max_pos; ++i) {
        if (si->client_fds[i] == -1) {
            continue;
        }
        if (FD_ISSET(si->client_fds[i], readfds_param)) {
            char buf[BUFSIZ] = "";
            int count;
            if ((count = read(si->client_fds[i], buf, sizeof(buf))) > 0) {
                printf("%s", buf);
                count = write(si->client_fds[i], buf, count);
                if (count == -1) {
                    perror("write");
                    remove_select_info(si, i);
                }
            }
            if (count == -1) {
                perror("read");
                remove_select_info(si, i);
            } else if (count == 0) {
                printf("peer close\n");
                remove_select_info(si, i);
            }
        }
        --nready;       
    }
}

void work_select(const net_app * proto)  {
    printf("%s\n", __func__);
    select_info si;
    init_select_info(&si, proto);
    for (;;) {
        fd_set readfds_param = si.readfds;
        int nready = select(si.max_fd + 1, &readfds_param, NULL, NULL, NULL);
        if (nready == -1) {
            PERROR_EXIT("select", 1);
        } else if (!nready) {
            continue;
        } else if (nready > 0) {
            nready = process_listen_fd_if_necessary(&si, &readfds_param, nready);
            process_client_fd_if_necessary(&si, &readfds_param, nready);
        } else {
            fprintf(stderr, "unknown error\n");
            exit(1);
        }
    } 
}