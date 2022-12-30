// #include <signal.h>
// int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
// struct sigaction {
//     void     (*sa_handler)(int);
//     void     (*sa_sigaction)(int, siginfo_t *, void *);
//     sigset_t   sa_mask;
//     int        sa_flags;
//     void     (*sa_restorer)(void); 
// };

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
void handle_error(int ret, const char * msg);
void handle_sigint(int signo);
void handle_sigquit(int signo);

int main(int argc, char ** const argv, char ** const envp) {
    struct sigaction act, old_act;
    act.sa_handler = handle_sigint;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0; 
    int ret = sigaction(SIGINT, &act, &old_act);
    handle_error(ret, "sigaction");
    act.sa_handler = handle_sigquit;
    ret = sigaction(SIGQUIT, &act, &old_act);
    handle_error(ret, "sigaction");
    while (1) ;
    return 0;
}

void handle_error(int ret, const char * msg) {
    if (ret == -1) {
        perror(msg);
        exit(1);
    }
}

void handle_sigint(int signo) {
    printf("%s %d\n", __func__, signo);
}

void handle_sigquit(int signo) {
    printf("%s %d\n", __func__, signo);
}

