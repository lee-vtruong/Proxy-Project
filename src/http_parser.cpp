#include "../include/common_lib.h"
#include "../include/http_parser.h"

// ----------------- HttpRequest methods -----------------
void HttpRequest::addHeader(const std::string& key, const std::string& value) {
    headers[key] = value; 
}

std::string HttpRequest::toString() const {
    std::string request = method + " " + url + " " + httpVersion + "\r\n";
    for (const auto& [key, value] : headers) {
        request += key + ": " + value + "\r\n";
    }
    request += "\r\n" + body;
    return request;
}

// This function is used to get the url from the request
std::string HttpRequest::getHeader(const std::string& key) {
    auto it = headers.find(key); 
    if (it != headers.end()) { 
        std::string headerValue = it->second;

        size_t pos = headerValue.find(':'); 
        if (pos != std::string::npos) {
            if (headerValue.substr(pos + 1) == "443") {
                isEncrypted = true; 
            } else {
                isEncrypted = false;
            }
            return headerValue.substr(0, pos); 
        }
        return headerValue;
    }
    return ""; 
}


// ----------------- HttpResponse methods -----------------
void HttpResponse::addHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}
std::string HttpResponse::toString() const {
    std::string response = httpVersion + " " + std::to_string(statusCode) + " " + reasonPhrase + "\r\n";
    for (const auto& [key, value] : headers) {
        response += key + ": " + value + "\r\n";
    }
    response += "\r\n" + body;
    return response;
}


// ----------------- ConnectionInfo methods -----------------
void ConnectionInfo::addTransaction(const HttpRequest& request, const HttpResponse& response) {
    transactions.push_back({request, response});
}

void ConnectionInfo::printTransactions() const {
    for (const auto& tran : transactions) {
        std::cout << "Request: " << tran.request.toString() << "\n";
        std::cout << "Response: " << tran.response.toString() << "\n";
    }
}


// ----------------- Utility functions -----------------
bool isSSLorTLS(const std::string &message) {
    if (message.size() > 0 && (unsigned char)message[0] == 0x16) {
        if (message.size() > 1 && (unsigned char)message[1] == 0x03) {
            return true;
        }
    }
    return false;
}

HttpRequest parseHttpRequest(const std::string& rawMessage) {
    HttpRequest request;
    request.rawRequest = rawMessage;
    request.isEncrypted = isSSLorTLS(rawMessage);
    if (!request.isEncrypted) {
        std::istringstream stream(rawMessage);
        std::string line;

        if (std::getline(stream, line)) {
            std::istringstream lineStream(line);
            lineStream >> request.method >> request.url >> request.httpVersion;
            if (request.httpVersion.find("HTTPS") != std::string::npos) {
                request.isEncrypted = true;
            }
        }

        while (std::getline(stream, line) && line != "\r") {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                trimNewlineChars(value);
                request.headers[key] = value;
            }
        }

        std::string body;
        while (std::getline(stream, line)) {
            body += line + "\n";
        }
        if (!body.empty()) {
            body.pop_back();
        }
        request.body = body;
    }

    return request;
}

void trimNewlineChars(std::string& str) {
    size_t end = str.find_last_not_of("\r\n");
    if (end != std::string::npos) {
        str = str.substr(0, end + 1);
    } else {
        str.clear();
    }
}

HttpResponse parseHttpResponse(const std::string& rawMessage) {
    HttpResponse response;
    response.rawResponse = rawMessage;
    response.isEncrypted = isSSLorTLS(rawMessage);
    if (!response.isEncrypted) {
        std::istringstream stream(rawMessage);
        std::string line;

        if (std::getline(stream, line)) {
            std::istringstream lineStream(line);
            lineStream >> response.httpVersion >> response.statusCode;
            std::getline(lineStream, response.reasonPhrase);
            response.reasonPhrase.erase(0, response.reasonPhrase.find_first_not_of(" "));
            if (response.httpVersion.find("HTTPS") != std::string::npos) {
                response.isEncrypted = true;
            }
        }

        while (std::getline(stream, line) && line != "\r") {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                response.headers[key] = value;
            }
        }

        std::string body;
        while (std::getline(stream, line)) {
            body += line + "\n";
        }
        if (!body.empty()) {
            body.pop_back();
        }
        response.body = body;
    }

    return response;
}

bool isValidHttpMethod(const std::string method) {
    const std::unordered_set<std::string> validMethods = {"GET", "POST", "PUT", "DELETE", "PATCH", "HEAD", "OPTIONS", "TRACE", "CONNECT"};

    std::string upperMethod = method;
    std::transform(upperMethod.begin(), upperMethod.end(), upperMethod.begin(), ::toupper);

    return validMethods.find(upperMethod) != validMethods.end();
}

bool isValidHttpVersion(const std::string version) {
    const std::regex versionPattern(R"(HTTP\/([1-3](\.\d)?))");
    return std::regex_match(version, versionPattern);
}

std::string ConnectionInfoToString(const ConnectionInfo& connection) {
    std::ostringstream oss;
    oss << "Client IP: " << connection.client.ip << ":" << connection.client.port << "\n";
    oss << "Server IP: " << connection.server.ip << ":" << connection.server.port << "\n";
    oss << "Time: " << std::ctime(&connection.time) << "\n";

    for (size_t i = 0; i < connection.transactions.size(); ++i) {
        const Transaction& transaction = connection.transactions[i];
        oss << "Transaction " << i + 1 << ":\n";
        oss << "  Request Method: " << transaction.request.method << "\n";
        oss << "  Request url: " << transaction.request.url << "\n";
        oss << "  Request HTTP Version: " << transaction.request.httpVersion << "\n";
        oss << "  Raw Request: " << transaction.request.rawRequest << "\n\n";
        oss << "  Response Status Code: " << transaction.response.statusCode << "\n";
        oss << "  Response Reason: " << transaction.response.reasonPhrase << "\n";
        oss << "  Raw Response: " << transaction.response.rawResponse << "\n";
    }

    return oss.str();
}

// ----------------- Logging functions -----------------
void log_request(HttpRequest request, FILE* f) {
    fprintf(f, "Raw Request: %s\n", request.toString().c_str());
}

void log_response(HttpResponse response, FILE* f) {
    fprintf(f, "Raw Response: %s\n", response.toString().c_str());
}

void log_connection(ConnectionInfo connection, FILE* f) {
    fprintf(f, "Connection Info: %s\n", ConnectionInfoToString(connection).c_str());
}

void log_connection_to_file(ConnectionInfo connection, const char* filename) {
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        perror("Error opening file");
        return;
    }
    log_connection(connection, f);
    fclose(f);
}