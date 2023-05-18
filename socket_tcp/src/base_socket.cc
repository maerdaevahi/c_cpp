#include "base_socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "common_macro.h"
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#ifdef __cplusplus
extern "C" {
#endif
int open_ipv4_tcp_listen_socket(const char * ip, int port) {
    int fd = open_ipv4_tcp_socket();
    enable_addr_reusable(fd);
    bind_addr(fd, ip, port);
    set_listen(fd, BACK_LOG);
    return fd;
}
int open_ipv4_tcp_socket() {
    int domain = AF_INET, type = SOCK_STREAM, protocol = 0;
    int fd = socket(domain, type, protocol);
    PERROR_EXIT_CONDITIONAL(fd, "socket", 1);
    return fd;
}

void enable_addr_reusable(int fd) {
    int option = 1;
    int setsockopt_ret = setsockopt (fd,  SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    PERROR_EXIT_CONDITIONAL(setsockopt_ret, "setsockopt", 1);
}

void bind_addr(int fd, const char * ip, int port) {
    struct sockaddr_in addr;
    addr.sin_family= AF_INET;
    addr.sin_port = htons(port);
    int inet_pton_ret = inet_pton (AF_INET, ip, &addr.sin_addr.s_addr);
    PERROR_EXIT_CONDITIONAL(inet_pton_ret, "inet_pton", 1);
    int bind_ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    PERROR_EXIT_CONDITIONAL(bind_ret, "bind", 1);
}

void set_listen(int fd, int n) {
    PERROR_EXIT_CONDITIONAL(listen(fd, n), "listen", 1);
}

int accept_connection(int listen_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_length = sizeof(client_addr);
    int client_fd = accept (listen_fd, (struct sockaddr *)&client_addr, &client_addr_length);
    PERROR_EXIT_CONDITIONAL(client_fd, "accept", 1);
    char buf[16] = "";
    printf("accept connection from ip: %s, port: %d\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, buf, sizeof(buf)), ntohs(client_addr.sin_port));
    return client_fd;
}


int read_n(int peer_fd, char * buf, int n) {
    char * pos = buf;
    char * end = buf + n;
    while (pos < end) {
        int count = recv(peer_fd, pos, end - pos, 0);
        if (count > 0) {
            pos += count;
        } else if (!count) {
            close(peer_fd);
            break;
        } else if (count == -1 && (errno == EAGAIN|| errno == EINTR)) {
            continue;
        }
    }
    return pos - buf;
}

int readn(int fd, char * buf, int count) {
    int left_count = count;
    char * pos = buf;
    for (; left_count > 0;) {
        int ret = read(fd, pos, left_count);
        if (ret == -1) {
            if (errno == EINTR) {
                printf("eintr\n");
                continue;
            } else if (errno == EAGAIN) {
                break;
            }
            perror("read");
            return -1;
        } else if (!ret) {
            printf("fd ends\n");
            close(fd);
            break;
        } else {
            pos += ret;
            left_count -= ret;
        }
    }
    return count - left_count;
}

int writen(int fd, const char * buf, int count) {
    int left_count = count;
    const char * pos = buf;
    for (; left_count > 0;) {
        int ret = write(fd, pos, left_count);
        if (ret == -1) {
            if (errno == EINTR) {
                printf("eintr\n");
                continue;
            } else if (errno == EPIPE) {
                printf("fd ends\n");
                close(fd);
                break;
            } else if (errno == EAGAIN) {
                break;
            }
            perror("write");
            return -1;
        } else if (ret > 0) {
            pos += ret;
            left_count -= ret;
        }
    }
    return count - left_count;
}

int readln(int fd, char * buf, int count) {
    char * pos = buf;
    int left_count = count;
    while (left_count > 0) {
        ssize_t ret = recv(fd, pos, left_count, MSG_PEEK);
        if (ret == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                break;
            }
            perror("recv");
            return -1;
        } else if (!ret) {
            printf("fd ends\n");
            close(fd);
            break;
        }
        int i = 0;
        for (; i < ret; ++i) {
            if (pos[i] == '\n') {
                // int n = readn(fd, pos, i + 1);
                // if (n == -1 || n != i + 1) {
                //     return -1;
                // }
                // pos += n;
                // left_count -= n;
                // return count - left_count;
                break;
            }
        }
        // int n = readn(fd, pos, ret);
        // if (n == -1 || n != ret) {
        //     return -1;
        // }
        // pos += n;
        // left_count -= n;
        int mount = i < ret ? i + 1 : ret;
        int n = readn(fd, pos, mount);
        if (n == -1 || n != mount) {
            return -1;
        }
        pos += n;
        left_count -= n;
        if (i < ret) {
            break;
        }
    }
    return count - left_count; 
}

void set_nonblock(int client_fd) {
    int flag = fcntl(client_fd, F_GETFL);
    if (flag == -1) {
        perror("fcntl");
    }
    flag |= O_NONBLOCK;
    int ret = fcntl(client_fd, F_SETFL, flag);
    if (ret == -1) {
        perror("fcntl");
    }
}

void set_block(int client_fd) {
    int flag = fcntl(client_fd, F_GETFL);
    if (flag == -1) {
        perror("fcntl");
    }
    flag &= ~O_NONBLOCK;
    int ret = fcntl(client_fd, F_SETFL, flag);
    if (ret == -1) {
        perror("fcntl");
    }
}
#ifdef __cplusplus
}
#endif