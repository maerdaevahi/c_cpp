#ifndef BASE_SOCKET
#define BASE_SOCKET
#define BACK_LOG 128
#ifdef __cplusplus
extern "C" {
#endif
int open_ipv4_tcp_socket(const char * ip, int port);

int create_ipv4_tcp_socket();

int create_socket(int domain, int type, int protocol);

void enable_addr_reusable(int listen_fd);

void bind_addr(int fd, const char * ip, int port);

void set_listen(int fd, int n);

int accept_connection(int fd);

int readn(int fd, char * buf, int count);

int writen(int fd, const char * buf, int count);

int readln(int fd, char * buf, int count);

void set_nonblock(int client_fd);

typedef struct operation_context {
    int fd;
    int error_no;
    char buf[1024];
    int size;
    int count;
    int ret;
    const char * msg;
} operation_context;

void init_oc(operation_context * oc, int fd);

int read_line(operation_context * oc, char * buf, int count);

#ifdef __cplusplus
}
#endif
#endif