#include "../include/common_lib.h"
#include "../include/http_parser.h"

bool isSSLorTLS(const std::string &message) {
    if (message.size() > 0 && (unsigned char)message[0] == 0x16) {
        if (message.size() > 1 && (unsigned char)message[1] == 0x03) {
            return true;
        }
    }
    return false;
}

void HttpRequest::addHeader(const std::string& key, const std::string& value) {
    headers[key] = value; 
}

std::string HttpRequest::toString() const {
    std::string request = method + " " + uri + " " + httpVersion + "\r\n";
    for (const auto& [key, value] : headers) {
        request += key + ": " + value + "\r\n";
    }
    request += "\r\n" + body;
    return request;
}

void HttpResponse::addHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

std::string HttpRequest::getHeader(const std::string& key) {
    auto it = headers.find(key);  // Tìm kiếm header trong map
    if (it != headers.end()) {    // Nếu tìm thấy header
        std::string headerValue = it->second;

        // Kiểm tra xem header có chứa dấu ':' hay không và xử lý giá trị sau dấu ':'
        size_t pos = headerValue.find(':');
        if (pos != std::string::npos) {
            // Xử lý cổng nếu cần (giả sử cổng sẽ là số 443 cho HTTPS)
            if (headerValue.substr(pos + 1) == "443") {
                isEncrypted = true;  // Đánh dấu kết nối là HTTPS
            } else {
                isEncrypted = false;  // Không phải HTTPS (HTTP thường là 80)
            }
            return headerValue.substr(0, pos);  // Trả về phần sau dấu ":"
        }
        return headerValue;  // Trả về toàn bộ giá trị nếu không có dấu ":"
    }
    return "";  // Trả về chuỗi rỗng nếu không tìm thấy key
}
std::string HttpResponse::toString() const {
    std::string response = httpVersion + " " + std::to_string(statusCode) + " " + reasonPhrase + "\r\n";
    for (const auto& [key, value] : headers) {
        response += key + ": " + value + "\r\n";
    }
    response += "\r\n" + body;
    return response;
}

void ConnectionInfo::addTransaction(const HttpRequest& request, const HttpResponse& response) {
    transactions.push_back({request, response});
}

void ConnectionInfo::printTransactions() const {
    for (const auto& tran : transactions) {
        std::cout << "Request: " << tran.request.toString() << "\n";
        std::cout << "Response: " << tran.response.toString() << "\n";
    }
}


HttpRequest parseHttpRequest(const std::string& rawMessage) {
    HttpRequest request;
    request.rawRequest = rawMessage;
    request.isEncrypted = isSSLorTLS(rawMessage);
    if (!request.isEncrypted) {
        std::istringstream stream(rawMessage);
        std::string line;

        // Parse dòng đầu tiên (Request Line)
        if (std::getline(stream, line)) {
            std::istringstream lineStream(line);
            lineStream >> request.method >> request.uri >> request.httpVersion;
            // Kiểm tra HTTPS
            if (request.httpVersion.find("HTTPS") != std::string::npos) {
                request.isEncrypted = true;
            }
        }

        // Parse headers
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

        // Parse body (nếu có)
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
    // Loại bỏ '\r' và '\n' ở cuối chuỗi
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

        // Parse dòng đầu tiên (Status Line)
        if (std::getline(stream, line)) {
            std::istringstream lineStream(line);
            lineStream >> response.httpVersion >> response.statusCode;
            std::getline(lineStream, response.reasonPhrase);
            response.reasonPhrase.erase(0, response.reasonPhrase.find_first_not_of(" "));
            // Kiểm tra HTTPS
            if (response.httpVersion.find("HTTPS") != std::string::npos) {
                response.isEncrypted = true;
            }
        }

        // Parse headers
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

        // Parse body (nếu có)
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
    oss << "Server IP: " << connection.server.ip << ":" << connection.server.port << "\n\n";

    for (size_t i = 0; i < connection.transactions.size(); ++i) {
        const Transaction& transaction = connection.transactions[i];
        oss << "Transaction " << i + 1 << ":\n";
        oss << "  Request Method: " << transaction.request.method << "\n";
        oss << "  Request URI: " << transaction.request.uri << "\n";
        oss << "  Request HTTP Version: " << transaction.request.httpVersion << "\n\n";
        oss << "  Response Status Code: " << transaction.response.statusCode << "\n";
        oss << "  Response Reason: " << transaction.response.reasonPhrase << "\n\n";
    }

    return oss.str();
}