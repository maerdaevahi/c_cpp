// #include <stdio.h>
// FILE *fopen(const char *pathname, const char *mode);
// FILE *fdopen(int fd, const char *mode);
// FILE *freopen(const char *pathname, const char *mode, FILE *stream);
// #include <stdio.h>
// size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
// size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "testfopen.h"

int main(int argc, char ** const argv, char ** const envp) {
    //readOpen();
    //readWriteOpen();
    if (argc != 4) {
        printf("argc != 4\n");
        return 1;
    }
    const char * pathname = argv[1];
    const char * mode = argv[2];
    int operation = atoi(argv[3]);
    FILE * file = operateFopen(pathname, mode);
    switch (operation) {
        case R:
            read(file);
            break;
        case W:
            write(file);
            break;
        default:
            break;
    }
    fclose(file);
    return 0;
} 

void readOpen() {
    const char * pathname = "a.txt";
    const char * mode = "r";
    FILE * file = fopen(pathname, mode);
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }
    fclose(file);
}

void readWriteOpen() {
    const char * pathname = "a.txt";
    const char * mode = "r+";
    FILE * file = fopen(pathname, mode);
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }
    fclose(file);
}

FILE *  operateFopen(const char * pathname, const char * mode) {
    FILE * file = fopen(pathname, mode);
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }
    return file;
}

void read(FILE * file) {
    assert(file != NULL);
    char buf[BUFSIZ] = "";
    size_t n = fread(buf, sizeof(buf), 1, file);
    if (n <= 0) {
        perror("fread");
        exit(1);
    }
}

void write(FILE * file) {
    assert(file != NULL);
    char buf[BUFSIZ] = "3333333333333333";
    size_t n = fwrite(buf, strlen(buf), 1, file);
    if (n <= 0) {
        perror("fwrite");
        exit(1);
    }
}