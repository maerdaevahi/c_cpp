#include "http_select_stub.h"

http_select_stub::http_select_stub(selector_improved * si, int client_fd) : si(si), fd(client_fd), state(INIT), count(0), hres(fd) {
}

http_select_stub::~http_select_stub() {}

void http_select_stub::handle_read() {
    read_data();
    parse_request();
}
void http_select_stub::handle_write() {
    complete_response();
    write_data();      
}

void http_select_stub::process_business() {
    string str;
    auto & h = hreq.headers;
    for (auto & p : h) {
        str.append("<p>").append(p.first).append(SEP).append(p.second).append("<p/><hr/>");
    }
    char tmp[BUFSIZ];
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, hres.body.c_str(), str.c_str()); 
    printf("%s\n", tmp);
    hres.body = tmp;
    hres.headers.find("Content-Length")->second = to_string(hres.body.length());
}

int http_select_stub::read_data() {
    int n, m = count;
    if (state == INIT || state == HEAD_PARSED) {
        while ((n = read(fd, buf + count, sizeof(buf) - count)) > 0) {
            count += n;
            buf[count] = 0;
            printf("%s", buf);
        }
        if (n == -1 && errno != EINTR && errno != EAGAIN) {
            perror("read");
            si->collect_useless_fds(fd);
        } else if (count == 0) {
            printf("peer close\n");
            si->collect_useless_fds(fd);
        }
    }
    return count - m;
}

void http_select_stub::parse_request() {
    if (state == INIT) {
        char * ptr = strstr(buf, HEAD_TAIL);
        if (ptr) {
            head_end = ptr + strlen(HEAD_TAIL);
            hreq.parse_head(buf, head_end);
            int ret = hreq.parse_request_body_if_necessary(head_end, buf + count);
            if (ret) {
                change_state();
            } else {
                state = HEAD_PARSED;
            }
        }
    } else if (state == HEAD_PARSED) {
        int ret = hreq.parse_request_body_if_necessary(head_end, buf + count);
        if (ret) {
            change_state();
        }
    }   
}

void http_select_stub::complete_response() {
    if (state == REQUEST_PARSED) {
        string str = hres.encode();
        count = str.length();
        strncpy(buf, str.c_str(), count);
        buf[count] = 0;
        state = RESPONSE_ENCODED;
    }
}

int http_select_stub::write_data() {
    char * pos = buf;
    char * end = buf + count;
    if (state == RESPONSE_ENCODED) {
        int n;
        while (pos < end && (n = write(fd, pos, end - pos)) > 0) {
            pos += n;
        }
        if (pos == end) {
            printf("response completed\n");
            si->collect_useless_fds(fd);
        } else if (n == -1 && errno != EINTR && errno != EAGAIN) {
            perror("write");
            si->collect_useless_fds(fd);
        } else {
            count = end - pos;
            for (int i = 0; i < count; ++i) {
                buf[i] = pos[i];
            }
        }
    }
    return pos - buf;
}

void http_select_stub::change_state() {
    state = REQUEST_PARSED;
    count = 0;
    si->remove_rdfd(fd);
    //si->add_wrfd(fd);
    si->collect_fds_to_process(fd);
}