// #include <sys/types.h>
// #include <sys/socket.h>
// int socket(int domain, int type, int protocol);
// int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// int listen(int sockfd, int backlog);
// int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
// #include <unistd.h>
// pid_t fork(void);
// #include <string.h>
// void *memset(void *s, int c, size_t n);
// #include <signal.h>
// typedef void (*sighandler_t)(int);
// sighandler_t signal(int signum, sighandler_t handler);

// cat /proc/sys/net/ipv4/tcp_fin_timeout

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "testsockettcp.h"

typedef void (*sighandler_t)(int);


void do_communication(int cfd);
void determine_wstatus(int pid, int wstatus);
void do_wait_child(int signo);
void handle_upper_or_lowwer(char * ptr, int n);
void test_port_reuse(int lfd);

int main(int argc, char * argv[], char * envp[]) {
    int signum = SIGCHLD;
    sighandler_t sighandler = signal(signum, do_wait_child);
    if (sighandler == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    int domain = AF_INET;
    int type = SOCK_STREAM;
    int protocol = 0;
    int lfd = socket(domain, type, protocol);
    if (lfd == -1) {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t addrlen = sizeof(addr);
    test_port_reuse(lfd);
    int bind_ret = bind(lfd, (struct sockaddr *)&addr, addrlen);
    if (bind_ret == -1) {
        perror("bind");
        exit(1);
    }
    int listen_ret = listen(lfd, 128);
    if (listen_ret == -1) {
        perror("listen");
        exit(1);
    }
    printf("server has been listenning\n");
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    while (1) {
        int cfd = accept(lfd, (struct sockaddr *)&client_addr, &client_addr_len);
        char buf[64] = "";
        const char * ptr = inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, buf, sizeof(buf));
        printf("%s:%u connected\n", buf, ntohs(client_addr.sin_port));
        if (ptr == NULL) {
            perror("inet_ntop");
            exit(1);
        }
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            close(lfd);
            do_communication(cfd);
        } else {
            close(cfd);            
        }
    }
    return 0;
}

void test_port_reuse(int lfd) {
    int optval;
    socklen_t optlen = sizeof(optval);
    int ret = getsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, &optlen);
    if (ret == -1) {
        perror("getsockopt1");
        exit(1); 
    }
    printf("getsockopt1 %d\n", optval);
    optval = 1;
    ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (ret == -1) {
        perror("setsockopt");
        exit(1); 
    }
    int optval1;
    socklen_t optlen1 = sizeof(optval1);
    ret = getsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval1, &optlen1);
    if (ret == -1) {
        perror("getsockopt2");
        exit(1); 
    }
    printf("getsockopt2 %d\n", optval1);
}

void do_communication(int cfd) {
    char buf[BUFSIZ] = "";
    while (1) {
        ssize_t n = read(cfd, buf, sizeof(buf));
        if (n < 0) {
            perror("read");
            close(cfd);
            exit(1);
        } else if (n == 0) {
            printf("client closed\n");
            close(cfd);
            exit(1);
        } else {
            printf("server receives: %s\n", buf);
            handle_upper_or_lowwer(buf, n);
            n = write(cfd, buf, n);
            if (n == -1) {
                perror("write");
                close(cfd);
                exit(1);
            }
            memset(buf, 0, sizeof(buf));
        }
    }
}

void handle_upper_or_lowwer(char * ptr, int n) {
    for (int i = 0; i < n; ++i) {
        if (ptr[i] >= 'a' && ptr[i] <= 'z') {
            ptr[i] += DIFF;
        } else if (ptr[i] >= 'A' && ptr[i] <= 'Z') {
            ptr[i] -= DIFF;
        }
    }
}

void do_wait_child(int signo) {
    fprintf(stdout, "%s %d\n", __func__, signo);
    int wstatus;
    pid_t pid;
    while ((pid = waitpid(-1, &wstatus, WNOHANG | WUNTRACED | WCONTINUED)) && pid != -1) {
        determine_wstatus(pid, wstatus);
    }
    if (pid == -1) {
        perror("waitpid");
    }
}

void determine_wstatus(int pid, int wstatus) {
    if (WIFEXITED(wstatus)) {
        printf("process [%d] exit with exit code [%d]\n", pid, WEXITSTATUS(wstatus));
    } else if (WIFSIGNALED(wstatus)) {
        printf("process [%d] signaled by signal [%d]\n", pid, WTERMSIG(wstatus));
    } else if (WIFSTOPPED(wstatus)) {
        printf("process [%d] stopped by signal [%d]\n", pid, WSTOPSIG(wstatus));
    } else if (WIFCONTINUED(wstatus)) {
        printf("process [%d] continued\n", pid);
    }
}
