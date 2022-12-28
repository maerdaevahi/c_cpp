// #include <signal.h>
// typedef void (*sighandler_t)(int);
// sighandler_t signal(int signum, sighandler_t handler);

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef void (*sighandler_t)(int);
void handle(int signo);
void determine_wstatus(int pid, int wstatus);

int main(int argc, char * argv[], char * envp[]) {
    sighandler_t signal_ret = signal(SIGTSTP, handle);
    if (signal_ret == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    signal_ret = signal(SIGCHLD, handle);
    if (signal_ret == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        int times = 5;
        pid = getpid();
        do {
            sleep(2);
            printf("process [%d] count down: %d\n", pid, times);
        } while (--times);
    } else {
        while (1) ;
    }
    return 0;
}

void handle(int signo) {
    fprintf(stdout, "%s %d\n", __func__, signo);
    if (signo == SIGCHLD) {
        int wstatus;
        pid_t pid;
        while ((pid = waitpid(-1, &wstatus, WNOHANG | WUNTRACED | WCONTINUED)) && pid != -1) {
            determine_wstatus(pid, wstatus);
        }
        if (pid == -1) {
            perror("waitpid");
        }
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


