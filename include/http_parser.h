#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include "cross_platform.h"

struct HttpRequest {
    std::string rawRequest;
    std::string method;
    std::string uri;
    std::string httpVersion;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    bool isEncrypted = false; // Mặc định không mã hóa (HTTP)

    void addHeader(const std::string& key, const std::string& value);
    std::string getHeader(const std::string& key);
    std::string toString() const;
};

struct HttpResponse {
    std::string rawResponse;
    std::string httpVersion;
    int statusCode;
    std::string reasonPhrase;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    bool isEncrypted = false; // Mặc định không mã hóa (HTTP)

    void addHeader(const std::string& key, const std::string& value);
    std::string toString() const;
};


struct Host {
    char ip[INET_ADDRSTRLEN];
    unsigned short port;
};

struct Transaction {
    HttpRequest request;
    HttpResponse response;
};

struct ConnectionInfo {
    Host client;  // Địa chỉ client
    Host server;  // Địa chỉ server
    // Vector lưu tất cả request/response trong kết nối
    std::vector<Transaction> transactions;
    void addTransaction(const HttpRequest& request, const HttpResponse& response);
    void printTransactions() const;
};


bool isSSLorTLS(const std::string &message);
bool isValidHttpMethod(const std::string method);
bool isValidHttpVersion(const std::string version);
void trimNewlineChars(std::string& str);
HttpRequest parseHttpRequest(const std::string& rawMessage);
HttpResponse parseHttpResponse(const std::string& rawMessage);

void log_request(HttpRequest request, FILE* f = stderr);
void log_response(HttpResponse response , FILE* f = stderr);
void log_connection_information(ConnectionInfo connection, FILE* f = stderr);

#endif // HTTP_PARSER_H
