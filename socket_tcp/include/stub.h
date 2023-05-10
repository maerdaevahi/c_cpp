#ifndef STUB
#define STUB
struct stub {
    virtual void handle_read() = 0;
    virtual void handle_write() = 0;
    virtual void process_business() = 0;
    //虚析构最好这样写
    virtual ~stub() {};
};
#endif