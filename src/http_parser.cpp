#include "../include/http_parser.h"

http_response parser_response(char* buffer) {
    http_response response;
 
    // parse version
    char version[9] = {0};
    char* code_start = strchr(buffer, ' ');
    if (!code_start || code_start - buffer > 9) {
        response.body = buffer;
        return response;
    }
    memcpy(version, buffer, min(9, code_start - buffer));
    for (response.version = 0; response.version < n_VERSION; response.version++) {
        if (strcmp(version, HTTP_VERSION[response.version]) == 0) break;
    }
    if (response.version == n_VERSION) {
        response.body = buffer;
        return response; 
    }

    // parse response code
    code_start += 1;
    char* header_start = strchr(code_start, '\r');
    if (!header_start) {
        response.body = buffer;
        return response;
    }
    response.response_code.assign(code_start, header_start - code_start);


    header_start += 2;
    while (*header_start != '\r' && *header_start != '\0') {
        char* value_start = strchr(header_start, ':');
        if (!value_start) break;

        std::string field_name(header_start, value_start - header_start);
        value_start += 1;

        while (*value_start == ' ') value_start++;
        char* value_end = strchr(value_start, '\r');
        if (!value_end) break;

        std::string value(value_start, value_end - value_start);
        response.header.push_back({field_name, value});

        header_start = value_end + 2;
    }

    header_start += 2;
    response.body = header_start;
    return response;
}


http_request parser_request(char* buffer) {
    http_request request;

    // parse method
    char method[8] = {0};
    char* url_start = strchr(buffer, ' ') + 1;
    if (!url_start || url_start - buffer > 8) {
        request.body = buffer;
        return request;
    }
    memcpy(method, buffer, min(8, (url_start - buffer - 1) * sizeof(char)));
    for (request.method = 0; request.method < n_METHOD; request.method++) {
        if (strcmp(method, HTTP_METHOD[request.method]) == 0) break;
    }
    if (request.method == n_METHOD) {
        request.body = buffer;
        return request;
    }

    // parse url
    char* version_start = strchr(url_start, ' ') + 1;
    if (!version_start) {
        request.body = buffer;
        return request;
    }
    request.url.assign(url_start, version_start - url_start - 1);
// fprintf(stderr, "URL: %s\n", request.url);

    // parse version
    char version[9] = {0};
    char* header_start = strchr(version_start, '\r');
    if (!header_start) {
        request.body = buffer;
        return request;
    }
    memcpy(version, version_start, min(8, (header_start - version_start) * sizeof(char)));
    for (request.version = 0; request.version < n_VERSION; request.version++) {
        if (strcmp(version, HTTP_VERSION[request.version])) break;
    }
    if (request.version == n_VERSION) {
        request.body = buffer;
        return request;
    }

    header_start += 2;
    while (*header_start != '\r') {
        char* value_start = strchr(header_start, ':');
        if (!value_start) return request;
        
        std::string field_name(header_start, value_start - header_start);
        value_start += 1;

        char* value_end = strchr(value_start, '\r');
        if (!value_end) return request;

        std::string value(value_start, value_end - value_start);
        request.header.push_back({field_name, value});
        header_start = value_end + 2;
    }
    header_start += 2;
    request.body = header_start;
    return request;
}

void log_request(http_request request, FILE* f) {
    fprintf(f, "Request:\n");
    if (request.method < n_METHOD && request.version < n_VERSION){
        fprintf(f, "%s %s %s\r\n", HTTP_METHOD[request.method], request.url.c_str(), HTTP_VERSION[request.version]);
        int n = request.header.size();
        for (int i = 0; i < n; i++) {
            fprintf(f, "%s: %s\r\n", request.header[i].field_name.c_str(), request.header[i].value.c_str());
        }
    }
    fprintf(f, "\r\n%s\r\n", request.body.c_str());
}

void log_response(http_response response , FILE* f) {
    fprintf(f, "Response:\n");
    if (response.version < n_VERSION){
        fprintf(f, "%s %s\r\n", HTTP_VERSION[response.version], response.response_code.c_str());
        int n = response.header.size();
        for (int i = 0; i < n; i++) {
            fprintf(f, "%s: %s\r\n", response.header[i].field_name.c_str(), response.header[i].value.c_str());
        }
    }
    fprintf(f, "\r\n%s\r\n", response.body.c_str());
}

void log_connection_information(http_connection connection, FILE* f) {
    fprintf(f, "\n---------------CONNECTION---------------\n");
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &connection.client_addr.sin_addr, ip, INET_ADDRSTRLEN);
    fprintf(f, "|- Client IP: %s / Port: %d\n", ip, ntohs(connection.client_addr.sin_port));
    inet_ntop(AF_INET, &connection.server_addr.sin_addr, ip, INET_ADDRSTRLEN);
    fprintf(f, "|- Server IP: %s / Port: %d\n", ip, ntohs(connection.server_addr.sin_port));
    int n_request = connection.requests.size(), n_response = connection.responses.size();
    int n = std::min(n_request, n_response);
    int i = 0;
    for (; i < n; i++) {
        log_request(connection.requests[i], f);
        // log_response(connection.responses[i], f);
    }
    for (; i < n_request; i++) log_request(connection.requests[i], f);
    // for (; i < n_response; i++) log_response(connection.responses[i], f);
}