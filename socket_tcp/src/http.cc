#include "http.h"
#include "base_socket.h"
#include <unistd.h>
#include <algorithm>
http_request::http_request(int fd) : fd(fd) {}

void http_request::parse_request_line(char * str_to_save) {
    method = strtok_r(str_to_save, " \r\n", &str_to_save);
    uri = strtok_r(str_to_save, " \r\n", &str_to_save);
    protocol = strtok_r(str_to_save, " \r\n", &str_to_save);
}

void http_request::parse_request_header(char * str_to_save) {
    char * name = strtok_r(str_to_save, ": ", &str_to_save);
    char * value = strtok_r(str_to_save, "\r\n", &str_to_save);
    headers.insert(pair<string, string>(name, value));
}

void http_request::parse_head(char * start, char * end) {
    int n = end - start + 1;
    char * tmp = new char[n];
    strncpy(tmp, start, n);
    tmp[n - 1] = 0;
    char * str_to_save = tmp; 
    char * str = strtok_r(str_to_save, "\r\n", &str_to_save);
    parse_request_line(str);
    while (str = strtok_r(str_to_save, "\r\n", &str_to_save)) {
        parse_request_header(str);
    }
    delete[] tmp;   
}

int http_request::parse_request_body_if_necessary(char * tmp, char * end) {
    if (!strcasecmp(method.c_str(), "POST")) {
        map<string, string>::iterator it = headers.find("Content-Length");
        if (it == headers.end()) {
            return 0;
        }
        int content_length = stoi(it->second);
        if (tmp + content_length == end) {
            body = string(tmp, content_length);
            return 1;
        }
        return 0;
    }
    return 1;
}

int http_request::decode() {
    char tmp[BUFSIZ];
    int count = readln(fd, tmp, sizeof(tmp));
    if (!count) {
        return 0;
    }
    tmp[count] = 0;
    parse_request_line(tmp);
    while ((count = readln(fd, tmp, sizeof(tmp))) > 0) {
        if (!count) {
            return 0;
        }
        tmp[count] = 0;
        if (!strcmp(tmp, "\r\n")) {
            printf("********************\n");
            break;
        } else {
            printf("<<%s>>\n", tmp);
            parse_request_header(tmp);
        }
    }
    if (!strcasecmp(method.c_str(), "POST")) {
        map<string, string>::iterator it = headers.find("Content-Length");
        if (it == headers.end()) {
            return 0;
        }
        int content_length = stoi(it->second);
        int n = readn(fd, tmp, content_length);
        if (n != content_length) {
            return 0;
        }
        body = string(tmp, n);
    }
    return 1;
}


string http_response::encode() {
    string str;
    str.append(protocol).append(BLANK_SPACE).append(code).append(BLANK_SPACE).append(status).append(CRLF);
    for_each(headers.begin(), headers.end(), [&](pair<string, string> p) {
        str.append(p.first).append(SEP).append(p.second).append(CRLF);
    });
    str.append(CRLF);
    str.append(body);
    return str;
}

http_response::http_response() {
    protocol = "HTTP/1.1";
    code = "200";
    status = "OK";
    headers = map<string, string>();
    headers.insert(pair<string, string>("Content-Type", "text/html;charset=UTF-8"));
    body = "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>hello world</title></head><body>%s</body></html>";
    headers.insert(pair<string, string>("Content-Length", to_string(body.length())));
}

http_response::http_response(int fd) : http_response() {
    this->fd = fd;
}

void http_response::write() {
    string str = encode();
    writen(fd, str.c_str(), str.length());
    printf("end\n");
}

void do_http(int fd) {
    http_request hreq(fd);
    hreq.decode();
    http_response hres(fd);
    hres.write();
    close(fd);
}
void mk_http(net_app * app, const char * ip, int port) {
    app->fd = open_ipv4_tcp_listen_socket(ip, port);
    app->ac = accept_connection;
    app->com = do_http;
}