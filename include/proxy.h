#include "common_lib.h"
#include "domain_process.h"
#include "http_parser.h"
#include "cross_platform.h"


class Proxy {
public:
    Proxy(const std::string& host = "127.0.0.1", int port = 8080)
        : host(host), port(port), is_running(false) {}

    // Hàm bắt đầu proxy
    void start() {
        if (is_running) {
            std::cerr << "Proxy is already running." << std::endl;
            return;
        }

        // Khởi tạo socket
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0) {
            perror("Failed to create socket");
            return;
        }

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(host.c_str());
        server_addr.sin_port = htons(port);

        // Liên kết socket với địa chỉ và cổng
        if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Bind failed");
            close(server_socket);
            return;
        }

        // Lắng nghe kết nối
        if (listen(server_socket, 5) < 0) {
            perror("Listen failed");
            close(server_socket);
            return;
        }

        is_running = true;
        std::cout << "Proxy server started on " << host << ":" << port << std::endl;
        accept_connections();
    }

    // Hàm dừng proxy
    void stop() {
        if (!is_running) {
            std::cerr << "Proxy is not running." << std::endl;
            return;
        }

        is_running = false;
        close(server_socket);
        std::cout << "Proxy server stopped." << std::endl;
    }

    // Hàm để lấy danh sách kết nối hiện tại
    std::vector<std::string> get_connections() {
        std::lock_guard<std::mutex> lock(mtx);
        return connections;
    }

    // Tùy chỉnh lại filter list
    void set_filter_list(const std::vector<std::string>& new_filter_list) {
        std::lock_guard<std::mutex> lock(mtx);
        filter_list = new_filter_list;
    }

    // Tùy chỉnh lại cổng
    void set_port(int new_port) {
        port = new_port;
        std::cout << "Port updated to " << port << "." << std::endl;
        if (is_running) {
            stop();
            start();
        }
    }

private:
    std::string host;
    int port;
    bool is_running;
    int server_socket;
    std::vector<std::string> connections;
    std::vector<std::string> filter_list;
    std::mutex mtx;

    // Hàm xử lý các kết nối đến
    void accept_connections() {
        while (is_running) {
            sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

            if (client_socket < 0) {
                perror("Accept failed");
                continue;
            }

            std::string client_ip = inet_ntoa(client_addr.sin_addr);
            {
                std::lock_guard<std::mutex> lock(mtx);
                connections.push_back(client_ip);
            }

            std::cout << "New connection from " << client_ip << std::endl;

            // Kiểm tra nếu client không bị chặn
            if (is_connection_allowed(client_ip)) {
                std::thread(&Proxy::handle_client, this, client_socket, client_ip).detach();
            } else {
                std::cerr << "Connection from " << client_ip << " is blocked due to filter list." << std::endl;
                close(client_socket);
            }
        }
    }

    // Hàm xử lý kết nối client
    void handle_client(int client_socket, const std::string& client_ip) {
        char buffer[1024] = {0};

        // Đọc dữ liệu từ client
        int read_size = read(client_socket, buffer, sizeof(buffer));
        if (read_size > 0) {
            std::cout << "Received data from " << client_ip << ": " << buffer << std::endl;
            // Gửi lại phản hồi cho client
            std::string response = "HTTP/1.1 200 OK\n\nHello from Proxy Server";
            send(client_socket, response.c_str(), response.length(), 0);
        }

        close(client_socket);
        {
            std::lock_guard<std::mutex> lock(mtx);
            auto it = std::find(connections.begin(), connections.end(), client_ip);
            if (it != connections.end()) {
                connections.erase(it);
            }
        }
        std::cout << "Connection from " << client_ip << " closed." << std::endl;
    }

    // Kiểm tra nếu kết nối có bị lọc hay không
    bool is_connection_allowed(const std::string& client_ip) {
        for (const auto& filter_item : filter_list) {
            if (client_ip.find(filter_item) != std::string::npos) {
                return false;
            }
        }
        return true;
    }
};