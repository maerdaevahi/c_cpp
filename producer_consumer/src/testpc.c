#include <stdlib.h>
#include "test_cond.h"
#include "test_sem.h"

int main(int argc, char ** const argv, char ** const envp) {
    if (argc != 2) {
        exit(1);
    }
    int i = atoi(argv[1]); 
    if (i == 1) {
        test_cond();
    } else if (i == 2) {
        test_sem();
    } else if (i == 3) {
        test_pthread_cond_timedwait();
    } else {
        test_pthread_mutex_trylock();
    }
    return 0;
}