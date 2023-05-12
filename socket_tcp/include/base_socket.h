#ifndef BASE_SOCKET
#define BASE_SOCKET
#define BACK_LOG 128
#ifdef __cplusplus
extern "C" {
#endif
int open_ipv4_tcp_listen_socket(const char * ip, int port);

int open_ipv4_tcp_socket();

int create_socket(int domain, int type, int protocol);

void enable_addr_reusable(int listen_fd);

void bind_addr(int fd, const char * ip, int port);

void set_listen(int fd, int n);

int accept_connection(int fd);

int readn(int fd, char * buf, int count);

int writen(int fd, const char * buf, int count);

int readln(int fd, char * buf, int count);

void set_nonblock(int client_fd);
#ifdef __cplusplus
}
#endif
#endif