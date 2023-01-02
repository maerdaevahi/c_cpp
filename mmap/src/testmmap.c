// #include <sys/mman.h>
// void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
// int munmap(void *addr, size_t length);
// sudo sysctl -w kernel.core_pattern=core.%p.%s.%c.%d.%P.%E
// dd if=/dev/zero of=a.txt bs=1024 count=5
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "testmmap.h"

static void testCommonMmap(int argc, char ** const argv);
static void testAnonymousMmap(int argc, char ** const argv);
/**
 * @brief a.txt的文件大小为0会报Bus error(超出文件大小4K倍数上限，因为文件系统也是分块的), mmap的prot参数隐含PROT_READ虽然pmap看不到PROT_READ
 * 超出length4K倍数上限会报段错误，在文件大小4K倍数上限和文件大小之间的部分操作会无效
 * 
 * @param argc 
 * @param argv 
 * @param envp 
 * @return int 
 */
int main(int argc, char ** const argv, char ** const envp) {
    testCommonMmap(argc, argv);
    //testAnonymousMmap(argc, argv);
    return 0;
}

static void testCommonMmap(int argc, char ** const argv) {
    if (argc != 3) {
        printf("argc != 3\n");
        exit(1);
    }
    
    size_t length = 1 K;

    int prot;
    switch (atoi(argv[1])) {
        case PR:
            prot = PROT_READ;
            break;
        case PW:
            prot = PROT_WRITE;
            break;
        case PRW:
            prot = PROT_READ | PROT_WRITE;
            break;
        default:
            prot = PROT_READ;
            break;
    }

    int flags = MAP_SHARED;

    int flag;
    switch (atoi(argv[2])) {
        case R:
            flag = O_RDONLY;
            break;
        case W:
            flag = O_WRONLY;
            break;
        case RW:
            flag = O_RDWR;
            break;
        default:
            flag = O_RDONLY;
            break;
    }

    int fd = open("a.txt", flag);
    
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    off_t offset = 0;
    char * const ptr = mmap(NULL, length, prot, flags, fd, offset);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    for (int i = 0; i < length; ++i) {
        printf("%c", ptr[i]);
        ptr[i] = 'b';
    }
    ptr[4 K - 1] = 'a';
    ptr[4 K] = 'a';
    int ret = munmap(ptr, length);
}


static void testAnonymousMmap(int argc, char ** const argv) {
    if (argc != 2) {
        printf("argc != 2\n");
        exit(1);
    }

    size_t length = 10;

    int prot;
    switch (atoi(argv[1])) {
        case PR:
            prot = PROT_READ;
            break;
        case PW:
            prot = PROT_WRITE;
            break;
        case PRW:
            prot = PROT_READ | PROT_WRITE;
            break;
        default:
            prot = PROT_READ;
            break;
    }

    int flags = MAP_SHARED | MAP_ANONYMOUS;

    int fd = -1;
    off_t offset = 0;
    char * const ptr = mmap(NULL, length, prot, flags, fd, offset);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    for (int i = 0; i < length; ++i) {
        printf("%c", ptr[i]);
        ptr[i] = 'b';
    }
    int ret = munmap(ptr, length);
}
