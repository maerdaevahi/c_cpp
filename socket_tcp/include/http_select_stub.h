#ifndef CLIENT_CONTEXT
#define CLIENT_CONTEXT
#include "selector_improved.h"
#define HEAD_TAIL "\r\n\r\n"

enum stage { INIT = 1, HEAD_PARSED, REQUEST_PARSED, RESPONSE_ENCODED, RESPONSE_SEND};

class http_select_stub : public stub {
    int fd;
    int state;
    char buf[BUFSIZ];
    int count;
    selector_improved * si;
    http_request hreq;
    http_response hres;
    char * head_end;
    int read_data();
    int write_data();
    void parse_request();
    void complete_response();
    void change_state();
public:
    http_select_stub(selector_improved * si, int client_fd);
    ~http_select_stub();
    virtual void handle_read();
    virtual void handle_write();
    virtual void process_business();
};
#endif