#include "common_lib.h"
#include "domain_process.h"
#include "http_parser.h"
#include "cross_platform.h"

class Proxy {
public:
    FilterList BLACK_LIST;
    std::vector<ConnectionInfo> connections;
    Proxy(int port) : BLACK_LIST(initFilterList("asset/blocked_domains.txt", "asset/blocked_ip.txt")), port(port), server_fd(-1), running(false) {}

    void stop() {
        running = false;
        if (server_fd != INVALID_SOCKET) {
            CLOSE_SOCKET(server_fd);
        }
        std::cout << "Proxy server stopped." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    bool setPort(int port) {
        this->port = port;
        if (!running) return false;
        stop();
        start();
        return true;
    }

    int getPort() const {
        return port;
    }

    int start() {
        running = true;
        INIT_SOCKET();
        setupServerSocket();
        std::thread(&Proxy::acceptConnections, this).detach();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return server_fd;    
    }

    void acceptConnections() {
        while (running) {
            sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            socket_t client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);

            if (client_fd == INVALID_SOCKET) {
                if (!running) break;
                perror("Accept failed");
                continue;
            }
            
            std::thread(&Proxy::handleClient, this, client_fd, client_addr).detach();
        }
    }

private:
    int port;
    socket_t server_fd;
    bool running;
    std::mutex connections_mutex;


    void updateConnections(ConnectionInfo conn_info) {
        std::lock_guard<std::mutex> lock(connections_mutex);
        connections.insert(connections.begin(), conn_info);
        if (connections.size() > 100) {
            connections.pop_back();
        }

        // Log thong tin ra file.
    }

    void setupServerSocket() {
        sockaddr_in server_addr;
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }

        int opt = 1;
        SETSOCKOPT(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, MAX_CONNECTIONS) < 0) {
            perror("Listen failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "Proxy server started on port " << port << std::endl;
    }

    void handleClient(socket_t client_fd, sockaddr_in client_addr) {
        char buffer[BUFFER_SIZE];
        socket_t remote_fd = -1;
        ConnectionInfo conn_info;
        
        inet_ntop(AF_INET, &(client_addr.sin_addr), conn_info.client.ip, INET_ADDRSTRLEN);
        conn_info.client.port = ntohs(client_addr.sin_port);
        std::cout << "Client IP: " << conn_info.client.ip << ", Port: " << conn_info.client.port << std::endl;

        try {
            ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if (bytes_read <= 0) {
                throw std::runtime_error("Failed to receive data from client");
            }

            buffer[bytes_read] = '\0';
            std::string rawRequest(buffer);

            HttpRequest request = parseHttpRequest(rawRequest);
            conn_info.parseServerPort(request);
            // std::cout << "\033[31mREQUEST:\033[0m " << request.rawRequest << "\n"; 
            HttpResponse response;
            
            sockaddr_in remote_addr;
            std::string host = request.getHeader("Host");

            remote_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (remote_fd == INVALID_SOCKET) {
                print_socket_error("Remote socket creation failed");
                throw std::runtime_error("Failed to connect server remote");
            }
                std::cout << host.c_str()  << "1\n";

            memset(&remote_addr, 0, sizeof(remote_addr));
            remote_addr.sin_family = AF_INET;
            remote_addr.sin_port = htons(conn_info.server.port);
            struct hostent* remote_host = gethostbyname(host.c_str());

            if (!remote_host) {
                print_socket_error("Failed to resolve host address");
                CLOSE_SOCKET(remote_fd);
                throw std::runtime_error("Failed to connect server remote");
            }
            memcpy(&remote_addr.sin_addr.s_addr, remote_host->h_addr, remote_host->h_length);
            
            if (unsigned(remote_host->h_length) > sizeof(remote_addr.sin_addr)) {
                fprintf(stderr, "Invalid host address length\n");
                CLOSE_SOCKET(remote_fd);
                throw std::runtime_error("Failed to connect server remote");
            }
                std::cout << host.c_str() << " : " << conn_info.server.port << "5\n";
            if (connect(remote_fd, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) < 0) {
                print_socket_error("Connect to remote server failed");
                CLOSE_SOCKET(remote_fd);
                throw std::runtime_error("Failed to connect server remote");
            }
            std::cout << host.c_str()  << "6\n";

            if (remote_fd == INVALID_SOCKET) {
                throw std::runtime_error("Failed to connect to remote server");
            }
            std::cout << "3\n";

            //Cho server remote
            inet_ntop(AF_INET, &(remote_addr.sin_addr), conn_info.server.ip, INET_ADDRSTRLEN);
            conn_info.server.port = ntohs(remote_addr.sin_port);
            // std::cout << "Server IP: " << conn_info.client.ip << ", Port: " << conn_info.client.port << std::endl;

            if (!isValidHttpMethod(request.method)) {
                fprintf(stderr, "Method is not valid!\n");
                const char* connect_response =  "HTTP/1.1 400 Bad Request\r\n"
                                                "Content-Type: text/plain\r\n"
                                                "Content-Length: 0\r\n\r\n";
                send(client_fd, connect_response, strlen(connect_response), 0);
                response = parseHttpResponse(connect_response);
                conn_info.addTransaction(request, response);
                updateConnections(conn_info);
                throw std::runtime_error("Method is not valid!\n");
            }

            if (!isValidHttpVersion(request.httpVersion)) {
                fprintf(stderr, "Version HTTP is not supported!\n");
                const char *connect_response =  "HTTP/1.1 505 HTTP Version Not Supported\r\n"
                                                "Content-Type: text/plain\r\n"
                                                "Content-Length: 0\r\n\r\n";
                send(client_fd, connect_response, strlen(connect_response), 0);
                response = parseHttpResponse(connect_response);
                conn_info.addTransaction(request, response);
                updateConnections(conn_info);
                throw std::runtime_error("Version HTTP is not supported!\n");
            }

            if (BLACK_LIST.isBlocked(host)) {
                const char *http_404_message =
                        "HTTP/1.1 404 Not Found\r\n"
                        "Content-Type: text/html; charset=UTF-8\r\n"
                        "Content-Length: 123\r\n"
                        "Server: Apache/2.4.41 (Ubuntu)\r\n"
                        "\r\n"
                        "<!DOCTYPE html>\n"
                        "<html>\n"
                        "<head><title>404 Not Found</title></head>\n"
                        "<body>\n"
                        "    <h1>404 Not Found</h1>\n"
                        "    <p>The requested resource was not found on this server.</p>\n"
                        "</body>\n"
                        "</html>";
                send(client_fd, http_404_message, strlen(http_404_message), 0);
                response = parseHttpResponse(http_404_message);
                conn_info.addTransaction(request, response);
                updateConnections(conn_info);
                throw std::runtime_error("THIS HOST IS BLCOKED!\n");
            }

            struct timeval timeout;
            timeout.tv_sec = 10;  // timeout sau 5 giây
            timeout.tv_usec = 0;

            if (request.method == "CONNECT") {
                char connect_response[] = "HTTP/1.1 200 Connection Established\r\n\r\n";
                send(client_fd, connect_response, strlen(connect_response), 0);
                response = parseHttpResponse(connect_response);
                conn_info.addTransaction(request, response);


                while (running) {
                    fd_set fds;
                    FD_ZERO(&fds);
                    FD_SET(client_fd, &fds);
                    FD_SET(remote_fd, &fds);
                    int max_fd = std::max(client_fd, remote_fd);

                    int activity = select(max_fd + 1, &fds, NULL, NULL, &timeout);
                    if (activity < 0) {
                        perror("Select error");
                        break;
                    } else if (activity == 0) {
                        // Timeout xảy ra, kết thúc vòng lặp
                        // printf("Timeout reached, ending read loop.\n");
                        break;
                    }

                    if (FD_ISSET(client_fd, &fds)) {
                        bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
                        if (bytes_read <= 0) break;
                        // HttpRequest req = parseHttpRequest(buffer);
                        send(remote_fd, buffer, bytes_read, 0);
                    }

                    if (FD_ISSET(remote_fd, &fds)) {
                        bytes_read = recv(remote_fd, buffer, BUFFER_SIZE, 0);
                        if (bytes_read <= 0) break;
                        // response = parseHttpResponse(buffer);
                        send(client_fd, buffer, bytes_read, 0);
                    }
                    // conn_info.addTransaction(request, response);
                }
            } else {
                send(remote_fd, request.rawRequest.c_str(), request.rawRequest.size(), 0);
                while (true) {
                    fd_set fds;
                    FD_ZERO(&fds);
                    FD_SET(remote_fd, &fds);

                    int activity = select(remote_fd + 1, &fds, NULL, NULL, &timeout);
                    if (activity < 0) {
                        perror("Select error");
                        break;
                    } else if (activity == 0) {
                        printf("Timeout reached, ending read loop.\n");
                        break;
                    }

                    ssize_t bytes_read = recv(remote_fd, buffer, BUFFER_SIZE, 0);
                    
                    if (bytes_read <= 0) {
                        break;
                    }
                    // response = parseHttpResponse(buffer);
                    send(client_fd, buffer, bytes_read, 0); 
                    // conn_info.addTransaction(request, response);
                }
            }
            // std::lock_guard<std::mutex> lock(connections_mutex);
            // connections.push_back(conn_info);
            updateConnections(conn_info);
        } catch (const std::exception& e) {
            // Lưu thông tin kết nối
            if (remote_fd != INVALID_SOCKET) CLOSE_SOCKET(remote_fd);
            CLOSE_SOCKET(client_fd);
            std::cerr << "Error handling client: " << e.what() << std::endl;
        }

        if (remote_fd != INVALID_SOCKET) CLOSE_SOCKET(remote_fd);
        CLOSE_SOCKET(client_fd);
    }

};


// class Proxy {
// public:
//     Proxy(const std::string& host = "127.0.0.1", int port = 8080)
//         : host(host), port(port), is_running(false) {}

//     // Hàm bắt đầu proxy
//     void start() {
//         if (is_running) {
//             std::cerr << "Proxy is already running." << std::endl;
//             return;
//         }

//         // Khởi tạo socket
//         server_socket = socket(AF_INET, SOCK_STREAM, 0);
//         if (server_socket < 0) {
//             perror("Failed to create socket");
//             return;
//         }

//         sockaddr_in server_addr;
//         server_addr.sin_family = AF_INET;
//         server_addr.sin_addr.s_addr = inet_addr(host.c_str());
//         server_addr.sin_port = htons(port);

//         // Liên kết socket với địa chỉ và cổng
//         if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
//             perror("Bind failed");
//             CLOSE_SOCKET(server_socket);
//             return;
//         }

//         // Lắng nghe kết nối
//         if (listen(server_socket, 5) < 0) {
//             perror("Listen failed");
//             CLOSE_SOCKET(server_socket);
//             return;
//         }

//         is_running = true;
//         std::cout << "Proxy server started on " << host << ":" << port << std::endl;
//         accept_connections();
//     }

//     // Hàm dừng proxy
//     void stop() {
//         if (!is_running) {
//             std::cerr << "Proxy is not running." << std::endl;
//             return;
//         }

//         is_running = false;
//         CLOSE_SOCKET(server_socket);
//         std::cout << "Proxy server stopped." << std::endl;
//     }

//     // Hàm để lấy danh sách kết nối hiện tại
//     std::vector<std::string> get_connections() {
//         std::lock_guard<std::mutex> lock(mtx);
//         return connections;
//     }

//     // Tùy chỉnh lại filter list
//     void set_filter_list(const std::vector<std::string>& new_filter_list) {
//         std::lock_guard<std::mutex> lock(mtx);
//         filter_list = new_filter_list;
//     }

//     // Tùy chỉnh lại cổng
//     void set_port(int new_port) {
//         port = new_port;
//         std::cout << "Port updated to " << port << "." << std::endl;
//         if (is_running) {
//             stop();
//             start();
//         }
//     }

//     // Hàm xử lý các kết nối đến
//     void accept_connections() {
//         sockaddr_in client_addr;
//         socklen_t client_len = sizeof(client_addr);
//         int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

//         if (client_socket < 0) {
//             perror("Accept failed");
//         }

//         std::string client_ip = inet_ntoa(client_addr.sin_addr);
//         {
//             std::lock_guard<std::mutex> lock(mtx);
//             connections.push_back(client_ip);
//         }

//         std::cout << "New connection from " << client_ip << std::endl;
//         std::thread(&Proxy::handle_client, this, client_socket, client_ip).detach();
//     }

// private:
//     std::string host;
//     int port;
//     bool is_running;
//     int server_socket;
//     std::vector<std::string> connections;
//     std::vector<std::string> filter_list;
//     std::mutex mtx;


//     // Hàm xử lý kết nối client
//     void handle_client(int client_socket, const std::string& client_ip) {
//         char buffer[1024] = {0};

//         // Đọc dữ liệu từ client
//         int read_size = read(client_socket, buffer, sizeof(buffer));
//         if (read_size > 0) {
//             std::cout << "Received data from " << client_ip << ": " << buffer << std::endl;
//             // Gửi lại phản hồi cho client
//             std::string response = "HTTP/1.1 200 OK\n\nHello from Proxy Server";
//             send(client_socket, response.c_str(), response.length(), 0);
//         }

//         CLOSE_SOCKET(client_socket);
//         {
//             std::lock_guard<std::mutex> lock(mtx);
//             auto it = std::find(connections.begin(), connections.end(), client_ip);
//             if (it != connections.end()) {
//                 connections.erase(it);
//             }
//         }
//         std::cout << "Connection from " << client_ip << " closed." << std::endl;
//     }
// };