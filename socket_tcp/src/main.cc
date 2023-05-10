#include <stdio.h>
#include <string.h>
#include "service.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int parse_args(int argc, char ** const argv);
void test_read();
void test_write(char * buf, int count);
void test_strstr(char * str);
void test_strtok(char * str);

int parse_args(int argc, char ** const argv) {
    if (argc > 2) {
        fprintf(stderr, "./prog [1/2]\n");
        exit(1);
    }
    int type;
    if (argc == 1) {
        type = 1;
    } else if (argc == 2) {
        type = atoi(argv[1]);
    }
    return type;
}


int readn(int fd, char * buf, int count) {
    for (; count > 0;) {
        int ret = read(fd, buf, count);
        if (ret == -1) {
            if (errno == EINTR) {
                printf("eintr\n");
                continue;
            }
            perror("read");
            exit(1);
        } else if (!ret) {
            printf("file ends\n");
            close(fd);
            return count;
        } else {
            buf += ret;
            count -= ret;
        }
    }
    return count;
}

void test_open(const char * path_name) {
    int fd = open(path_name, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    char buf[10240] = {0}; 
    int ret = readn(fd, buf, 3000);
    printf("->%d\n", ret);
    exit(0);
}

void test_read() {
    FILE * file = fopen("../a.txt", "r+b");
    if (!file) {
        printf("--------------\n");
    }
    char buf[BUFSIZ] = {0};
    fread(buf, sizeof(buf), 1, file);
}

void test_write(char * buf, int count) {
    FILE * file = fopen("../a.txt", "r+b");
    if (!file) {
        printf("--------------\n");
    }
    fwrite(buf, count, 1, file);
    fflush(file);
}

void test_strstr(char * str) {
    char * ptr = strstr(str, "\r\n\r\n");
    printf("%ld\n", ptr - str);
}

void test_strtok(char * str) {
    char * ret = strtok(str, "\r\n");
    do {
        printf("%s\n", ret);
    } while (ret = strtok(NULL, "\r\n"));
}

int main(int argc, char ** const argv, char ** const envp) {
    // test_open("../a.txt");
    int type = parse_args(argc, argv);
    return service(type);
}