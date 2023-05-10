#ifndef HTTP
#define HTTP
#include <string>
#include <map>
#include <utility>
using namespace std;
#include "app.h"
#include <string.h>

#define BLANK_SPACE " "
#define CRLF "\r\n"
#define SEP ": "

struct http_request {
    int fd;
    string method;
    string uri;
    string protocol;
    map<string, string> headers;
    string body;
    http_request() = default;
    http_request(int fd);
    int decode();
    void parse_request_line(char * str);
    void parse_request_header(char * str);
    int parse_request_body_if_necessary(char * tmp, char * end);
    void parse_head(char * start, char * end);
};

struct http_response {
    int fd;
    string protocol;
    string code;
    string status; 
    map<string, string> headers;
    string body;
    http_response();
    http_response(int fd);
    string encode();
    void write();
};

void mk_http(net_app * proto, const char * ip, int port);
#endif