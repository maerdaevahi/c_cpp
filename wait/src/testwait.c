// #include <sys/types.h>
// #include <sys/wait.h>
// pid_t wait(int *wstatus);
// pid_t waitpid(pid_t pid, int *wstatus, int options);
// int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);
// #include <sys/types.h>
// #include <unistd.h>
// pid_t fork(void);

// /usr/include/asm-generic/errno-base.h

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "testwait.h"
void do_parent_work(int op);
void do_child_work(int seq);
int create_child_process();
void test_wait(int argc, char * argv[], char * envp[]);
void determine_wstatus(int pid, int wstatus);
int main(int argc, char * argv[], char * envp[]) {
    test_wait(argc, argv, envp);
    return 0;
}

void test_wait(int argc, char * argv[], char * envp[]) {
    if (argc != 2) {
        printf("argc != 2\n");
        exit(1);
    }
    int op = atoi(argv[1]);
    const pid_t parent_id = getpid();
    int i = create_child_process();
    if (getpid() == parent_id) {
        do_parent_work(op);
    } else {
        do_child_work(i);
    }
}

int create_child_process() {
    const int count = 5;
    int i = 0;
    for (; i < count; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            printf("child process [%d], [%d], [%d]\n", i, getpid(), getppid());
            break;
        }
    }
    return i;
}

void do_parent_work(int op) {
    printf("parent process [%d], [%d]\n", getpid(), getppid());
    int wstatus;
    pid_t pid;
    if (op == WAIT) {
        while ((pid = wait(&wstatus)) != -1) {
            determine_wstatus(pid, wstatus);
        }
    } else {
        while ((pid = waitpid(-1, &wstatus, WNOHANG | WUNTRACED | WCONTINUED)) != -1) {
            determine_wstatus(pid, wstatus);
        }
    }
    perror("wait");
}

void determine_wstatus(int pid, int wstatus) {
    if (pid == 0) {
        //printf("no process changes status\n");
        return;
    } 
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

void do_child_work(int seq) {
    printf("child process [%d], pid [%d], ppid [%d]\n", seq, getpid(), getppid());
    int times = 10;
    do {
        sleep(2);
    } while (--times);
    printf("child process [%d] will exit, pid [%d], ppid [%d]\n", seq, getpid(), getppid());
    exit(seq);
}
