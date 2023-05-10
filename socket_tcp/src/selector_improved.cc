#include <sys/time.h>
#include <sys/types.h>
#include "common_macro.h"
#include "selector_improved.h"
#include "base_socket.h"
#include <algorithm>


// void FD_CLR(int fd, fd_set *set);
// int  FD_ISSET(int fd, fd_set *set);
// void FD_SET(int fd, fd_set *set);
// void FD_ZERO(fd_set *set);

selector_improved::selector_improved(const net_app * na) {
    listen_fd = na->fd;
    ac = na->ac;
    com = na->com;
    FD_ZERO(&readfds);
    add_rdfd(listen_fd);
    FD_ZERO(&writefds);
}

void selector_improved::collect_useless_fds(int fd) {
    fds_to_close.push_back(fd);
}

void selector_improved::collect_fds_to_process(int fd) {
    fds_to_process.push_back(fd);
}

void selector_improved::add_rdfd(int fd) {
    FD_SET(fd, &readfds);
}

void selector_improved::add_wrfd(int fd) {
    FD_SET(fd, &writefds);
}

void selector_improved::remove_rdfd(int fd) {
    FD_CLR(fd, &readfds);
}

void selector_improved::remove_wrfd(int fd) {
    FD_CLR(fd, &writefds);
}

void selector_improved::add_select_info(int client_fd) {
    add_rdfd(client_fd);
    stub * cc = new_stub(client_fd);
    ctx.insert(pair<int, stub *>(client_fd, cc));
}

void selector_improved::remove_select_info(int fd) {
    map<int, stub *>::iterator it = ctx.find(fd);
    if (it != ctx.end()) {
        delete it->second;
        ctx.erase(it);
    }
    close(fd);
    remove_rdfd(fd);
    remove_wrfd(fd);
}

void selector_improved::process_listen_fd() {
    if (FD_ISSET(listen_fd, &readfds_io)) {
        int client_fd = ac(listen_fd);
        set_nonblock(client_fd);
        add_select_info(client_fd);
        --nready;
    }
}

void selector_improved::process_read_client_fd() {
    for (auto p = ctx.begin(); nready > 0 && p != ctx.end(); ++p) {
        if (FD_ISSET(p->first, &readfds_io)) {
            p->second->handle_read();    
            --nready;
        }
    }
}

void selector_improved::process_write_client_fd() {
    for (auto p = ctx.begin(); nready > 0 && p != ctx.end(); ++p) {
        if (FD_ISSET(p->first, &writefds_io)) {
            p->second->handle_write();    
            --nready;
        }
    }
}

void selector_improved::close_useless_fds() {
    for_each(fds_to_close.begin(), fds_to_close.end(), [=](int fd) {
        remove_select_info(fd);
    });
    fds_to_close.clear();
}

void selector_improved::process() {
    for_each(fds_to_process.begin(), fds_to_process.end(), [=](int fd) {
        printf("------------------business processing------------------\n");
        auto p = ctx.find(fd);
        if (p != ctx.end()) {
            printf("------------------%d------------------\n", p->first);
            p->second->process_business();
        }
        printf("------------------business processing------------------\n");
        add_wrfd(fd);
    });
    fds_to_process.clear();
}

int selector_improved::get_max_fd() {
    auto it = ctx.rbegin();
    int ctx_max_fd = it != ctx.rend() ? it->first : -1;
    return ctx_max_fd > listen_fd ? ctx_max_fd : listen_fd;
}

void selector_improved::work()  {
    printf("%s\n", __func__);
    for (;;) {
        readfds_io = readfds;
        writefds_io = writefds;
        int max_fd = get_max_fd();
        nready = select(max_fd + 1, &readfds_io, &writefds_io, NULL, NULL);
        if (nready == -1) {
            PERROR_EXIT("select", 1);
        } else if (!nready) {
            continue;
        } else if (nready > 0) {
            process_listen_fd();
            process_read_client_fd();
            process_write_client_fd();
            close_useless_fds();
            process();
        } else {
            fprintf(stderr, "unknown error\n");
            exit(1);
        }
    } 
}