#ifndef COMMON_MACRO
#define COMMON_MACRO
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define PERROR_EXIT(msg, exit_code) \
        do {\
            perror(msg);\
            exit(exit_code);\
        } while (0)
#define PERROR_EXIT_CONDITIONAL(ret, msg, exit_code) \
        do {\
            if (ret == -1) {\
                PERROR_EXIT(msg, exit_code);\
            }\
        } while (0)
#define HANDLE_ERROR_CONDITIONAL(ret, msg, exit_code) \
        do {\
            if (ret) {\
                fprintf(stderr, "%s: %s", msg, strerror(ret));\
                pthread_exit(exit_code);\
            }\
        } while (0)        
#endif