#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include "mp.h"
#include "common_macro.h"
#include <errno.h>
void do_wait(int signo);

void do_wait(int signo) {
    printf("process received signal [%d]\n", signo);
    int w, wstatus;
    while ((w = waitpid(-1, &wstatus, WUNTRACED | WCONTINUED | WNOHANG)) > 0) {
        if (WIFEXITED(wstatus)) {
            printf("exited, status=%d\n", WEXITSTATUS(wstatus));
        } else if (WIFSIGNALED(wstatus)) {
            printf("killed by signal %d\n", WTERMSIG(wstatus));
        } else if (WIFSTOPPED(wstatus)) {
            printf("stopped by signal %d\n", WSTOPSIG(wstatus));
        } else if (WIFCONTINUED(wstatus)) {
            printf("continued\n");
        }
     }
     if (w == -1) {
        perror("waitpid");
    } else if (!w) {
        printf("children have not yet changed state\n");
    }
}

void work_multiple_process(const net_app * proto)  {
    int listen_fd = proto->fd;
    acqure ac = proto->ac;
    communicate com = proto->com;
    printf("%s\n", __func__);
    signal(SIGCHLD, do_wait);
    for (;;) {
        int client_fd = ac(listen_fd);
        pid_t pid = fork();
        if (pid == -1) {
            PERROR_EXIT("fork", 1);
        } else if (pid == 0) {
            PERROR_EXIT_CONDITIONAL(close(listen_fd), "close", 1);
            com(client_fd);
            break;
        } else {
            PERROR_EXIT_CONDITIONAL(close(client_fd), "close", 1);
        }
    } 
}