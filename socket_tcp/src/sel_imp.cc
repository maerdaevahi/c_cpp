#include "selector_improved.h"
#include "http_select_stub.h"

class selector_improved_http : public selector_improved {
public:
    selector_improved_http(const net_app * na) : selector_improved(na) {}
    virtual stub * new_stub(int client_fd) {
        return new http_select_stub(this, client_fd);
    }
};
void work_selector_improved(const net_app * na) {
    selector_improved * sim = new selector_improved_http(na);
    sim->work();
    delete sim;
}