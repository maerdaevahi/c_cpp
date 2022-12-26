// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// int open(const char *pathname, int flags);
// int open(const char *pathname, int flags, mode_t mode);
// int creat(const char *pathname, mode_t mode);
// int openat(int dirfd, const char *pathname, int flags);
// int openat(int dirfd, const char *pathname, int flags, mode_t mode);
// ssize_t write(int fd, const void *buf, size_t count);
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "testopen.h"

int main(int argc, char ** const argv, char ** const envp) {
    // readOpen();
    // readOpenWithCreate();
    // writeOpen();
    // writeOpenWithCreate();
    // writeOpenWithCreateExcl();
    // readOpenWithCreateExcl();
    // readWriteOpen();
    // readOpenAndWrite();
    writeOpenAndRead();
    return 0;
}

/**
 * @brief 以只读方式打开文件，若文件存在则正常打开(打开权限包含于文件权限)，若文件不存在则报错(No such file or directory)
 * 
 */
void readOpen() {
    const char * pathname = "a.txt";
    int flags = O_RDONLY;
    int fd = open(pathname, flags);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
}

/**
 * @brief 以只读方式打开文件，若文件存在则正常打开(打开权限包含于文件权限)，若文件不存在则创建并打开
 * 
 */
void readOpenWithCreate() {
    const char * pathname = "a.txt";
    int flags = O_RDONLY | O_CREAT;
    int fd = open(pathname, flags, 0777);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
}

/**
 * @brief 以只写方式打开文件，若文件存在则正常打开(打开权限包含于文件权限)，若文件不存在则报错(No such file or directory)
 * 
 */
void writeOpen() {
    const char * pathname = "a.txt";
    int flags = O_WRONLY;
    int fd = open(pathname, flags);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
}

/**
 * @brief 以只写方式打开文件，若文件存在则正常打开，若文件不存在则创建并打开
 * 
 */
void writeOpenWithCreate() {
    const char * pathname = "a.txt";
    int flags = O_WRONLY | O_CREAT;
    int fd = open(pathname, flags, 0777);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
}

/**
 * @brief 以只读方式打开文件，若文件存在则报错(File exists)，若文件不存在则创建并打开
 * 
 */
void readOpenWithCreateExcl() {
    const char * pathname = "a.txt";
    int flags = O_RDONLY | O_CREAT | O_EXCL;
    int fd = open(pathname, flags, 0777);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
}

/**
 * @brief 以只写方式打开文件，若文件存在则报错(File exists)，若文件不存在则创建并打开
 * 
 */
void writeOpenWithCreateExcl() {
    const char * pathname = "a.txt";
    int flags = O_WRONLY | O_CREAT | O_EXCL;
    int fd = open(pathname, flags, 0777);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
}

/**
 * @brief 以读写方式打开文件，若文件存在则正常打开(打开权限包含于文件权限)，若文件不存在则报错(No such file or directory)
 *
 */
void readWriteOpen() {
    const char * pathname = "a.txt";
    int flags = O_RDWR;
    int fd = open(pathname, flags);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
}

/* Bad file descriptor */
void readOpenAndWrite() {
    const char * pathname = "a.txt";
    int flags = O_RDONLY;
    int fd = open(pathname, flags);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    const char * content = "13579";
    ssize_t n = write(fd, content, strlen(content));
    if (n = -1) {
        perror("write");
        exit(1);
    }
}

/* Bad file descriptor */
void writeOpenAndRead() {
    const char * pathname = "a.txt";
    int flags = O_WRONLY;
    int fd = open(pathname, flags);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    char buf[128] = "";
    ssize_t n = read(fd, buf, sizeof(buf));
    if (n = -1) {
        perror("read");
        exit(1);
    }
    printf("%s: %s\n", __func__, buf);
}