#include "service.h"
#include "http.h"
#include "mp.h"
#include "mt.h"
#include "sel_imp.h"
#include "select.h"
void work_selector_improved(const net_app * na);

#define ANY_IP_ADDR "0.0.0.0"
#define DEFAULT_PORT 8080


int service(int type) {
    net_app app;
    mk_http(&app, ANY_IP_ADDR, DEFAULT_PORT);
    typedef void (*fun)(const net_app *);
    fun ivk = type == 1 ? work_multiple_process : type == 2 ? work_select : type == 3 ? work_selector_improved : work_multiple_thread;
    ivk(&app);
    return 0;
}