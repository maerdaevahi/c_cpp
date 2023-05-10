#ifndef SELECTOR
#define SELECTOR
#include "http.h"
#include <map>
#include <vector>
#include <unistd.h>
#include <stub.h>
using namespace std;
class selector_improved {
    int listen_fd;
    fd_set readfds;
    fd_set writefds;
    fd_set readfds_io;
    fd_set writefds_io;
    int nready;
    map<int, stub *> ctx;
    vector<int> fds_to_close;
    vector<int> fds_to_process;
    acqure ac;
    communicate com;

    void add_select_info(int client_fd);
    void remove_select_info(int i);
    void process_listen_fd();
    void process_read_client_fd();
    void process_write_client_fd();
    int get_max_fd();
public:
    selector_improved(const net_app * na);
    void work();
    void close_useless_fds();
    void add_rdfd(int fd);
    void add_wrfd(int fd);
    void remove_rdfd(int fd);
    void remove_wrfd(int fd);
    void collect_useless_fds(int fd);
    void collect_fds_to_process(int fd);
    void process();
protected:
    virtual stub * new_stub(int client_fd) = 0;
};
#endif