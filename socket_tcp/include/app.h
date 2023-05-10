#ifndef APP
#define APP
#include <stdlib.h>
typedef int (*acqure)(int);
typedef void (*communicate)(int);

typedef struct net_app {
    int fd;
    acqure ac;
    communicate com;
    communicate rd;
    communicate wr;
} net_app;
#endif