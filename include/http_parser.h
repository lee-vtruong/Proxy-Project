#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include "common_lib.h"

typedef struct {
    std::string field_name;
    std::string value;
} header_filed;

typedef struct {
    short method;
    std::string url;         
    short version;
    std::vector<header_filed> header;
    std::string body;
} http_request;

typedef struct {
    short version;
    std::string response_code;          
    std::vector<header_filed> header;
    std::string body;
} http_response;

typedef struct {
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    std::vector<http_request> requests;
    std::vector<http_response> responses;
} http_connection;

http_response parser_response(char* buffer);
http_request parser_request(char* buffer);
void log_request(http_request request, FILE* f = stderr);
void log_response(http_response response , FILE* f = stderr);
void log_connection_information(http_connection connection, FILE* f = stderr);

#endif // HTTP_PARSER_H
