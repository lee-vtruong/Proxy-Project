#include "../include/cross_platform.h"
#include "../include/common_lib.h"
#include "../include/domain_process.h"
#include "../include/http_parser.h"

// Kích thước bộ đệm
#define BUFFER_SIZE 65536
// Cổng mặc định
#define LISTEN_PORT 8080
// Số kết nối tối đa
#define MAX_CONNECTIONS 100

static socket_t server_fd_global = INVALID_SOCKET;

// Danh sách các kết nối
std::vector<ConnectionInfo> connections;
// Mutex để bảo vệ danh sách kết nối
std::mutex connections_mutex;

// Quản lý tín hiệu
bool running = true;

void signalHandler(int signal) {
    std::cout << "\nReceived signal: " << signal << ", shutting down..." << std::endl;
    running = false;
    if (server_fd_global != INVALID_SOCKET ) {
        CLOSE_SOCKET(server_fd_global);
    }
}

// Lớp Proxy
class Proxy {
public:
    Proxy(int port) : port(port), server_fd(-1) {}

    void start() {
        INIT_SOCKET();
        black_list = initFilterList("asset/blocked_domains.txt", "asset/blocked_ip.txt");
        setupServerSocket();
        acceptConnections();
        CLEANUP_SOCKET();
    }

private:
    int port;
    socket_t server_fd;
    sockaddr_in server_addr;
    FilterList black_list;
    //BLOCKING DOMAIN

    void setupServerSocket() {
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }

        int opt = 1;
        SETSOCKOPT(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        server_fd_global = server_fd;

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            print_socket_error("Bind failed");
            CLOSE_SOCKET(server_fd);
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, MAX_CONNECTIONS) < 0) {
            print_socket_error("Listen failed");
            CLOSE_SOCKET(server_fd);
            exit(EXIT_FAILURE);
        }

        std::cout << "Proxy server started on port " << port << std::endl;
    }

    void acceptConnections() {
        while (running) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            socket_t client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);

            if (client_fd < 0) {
                if (!running) break;
                print_socket_error("Accept failed");
                continue; 
            }

            std::thread(&Proxy::handleClient, this, client_fd, client_addr).detach();
        }

        CLOSE_SOCKET(server_fd);
    }


    void handleClient(socket_t client_fd, sockaddr_in client_addr) {
        char buffer[BUFFER_SIZE];
        socket_t remote_fd = -1;
        ConnectionInfo conn_info;
        
        // inet_ntop(AF_INET, &(client_addr.sin_addr), conn_info.client.ip, INET_ADDRSTRLEN);
        conn_info.client.port = ntohs(client_addr.sin_port);
        std::cout << "Client IP: " << conn_info.client.ip << ", Port: " << conn_info.client.port << std::endl;

        try {
            ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if (bytes_read <= 0) {
                throw std::runtime_error("Failed to receive data from client");
            }

            // buffer[bytes_read] = '\0';
            std::string rawRequest(buffer);

            HttpRequest request = parseHttpRequest(rawRequest);
            std::cout << "\033[31mREQUEST:\033[0m " << request.rawRequest << "\n"; 
            HttpResponse response;
            
            if (!isValidHttpMethod(request.method)) {
                fprintf(stderr, "Method is not valid!\n");
                const char* connect_response =  "HTTP/1.1 400 Bad Request\r\n"
                                                "Content-Type: text/plain\r\n"
                                                "Content-Length: 0\r\n\r\n";
                send(client_fd, connect_response, strlen(connect_response), 0);
                throw std::runtime_error("Method is not valid!\n");
            }

            if (!isValidHttpVersion(request.httpVersion)) {
                fprintf(stderr, "Version HTTP is not supported!\n");
                const char *connect_response =  "HTTP/1.1 505 HTTP Version Not Supported\r\n"
                                                "Content-Type: text/plain\r\n"
                                                "Content-Length: 0\r\n\r\n";
                send(client_fd, connect_response, strlen(connect_response), 0);
                throw std::runtime_error("Version HTTP is not supported!\n");
            }

            std::string host = request.getHeader("Host");
            if (black_list.isBlocked(host)) {
                const char *http_404_message =
                        "HTTP/1.1 404 Not Found\r\n"
                        "Content-Type: text/html; charset=UTF-8\r\n"
                        "Content-Length: 123\r\n"
                        "Server: Apache/2.4.41 (Ubuntu)\r\n"
                        "Date: Mon, 07 Nov 2024 12:34:56 GMT\r\n"
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
                throw std::runtime_error("THIS HOST IS BLCOKED!\n");
            }
            remote_fd = connectRemote(request, host);
            if (remote_fd == -1) {
                throw std::runtime_error("Failed to connect to remote server");
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
                        HttpRequest req = parseHttpRequest(buffer);
                        send(remote_fd, buffer, bytes_read, 0);
                    }

                    if (FD_ISSET(remote_fd, &fds)) {
                        bytes_read = recv(remote_fd, buffer, BUFFER_SIZE, 0);
                        if (bytes_read <= 0) break;
                        response = parseHttpResponse(buffer);
                        send(client_fd, buffer, bytes_read, 0);
                    }
                    conn_info.addTransaction(request, response);
                }
            } else {
                send(remote_fd, request.rawRequest.c_str(), request.rawRequest.size(), 0);
                while (running) {
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
                    response = parseHttpResponse(buffer);
                    send(client_fd, buffer, bytes_read, 0); 
                    conn_info.addTransaction(request, response);
                }
            }

            // Lưu thông tin kết nối
            std::lock_guard<std::mutex> lock(connections_mutex);
            connections.push_back(conn_info);
        } catch (const std::exception& e) {
            CLOSE_SOCKET(client_fd);
            std::cerr << "Error handling client: " << e.what() << std::endl;
        }

        if (remote_fd != -1) CLOSE_SOCKET(remote_fd);
        CLOSE_SOCKET(client_fd);
    }

    socket_t connectRemote(const HttpRequest& request, const std::string host) {
        socket_t remote_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (remote_socket == -1) {
            print_socket_error("Remote socket creation failed");
            return -1;
        }

        sockaddr_in remote_addr;
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = htons(request.isEncrypted ? 443 : 80);
        struct hostent* remote_host = gethostbyname(host.c_str());

        if (remote_host == nullptr) {
            print_socket_error("Failed to resolve host address");
            CLOSE_SOCKET(remote_socket);
            return -1;
        }
        memcpy(&remote_addr.sin_addr, remote_host->h_addr, remote_host->h_length);
        
        if (unsigned(remote_host->h_length) > sizeof(remote_addr.sin_addr)) {
            fprintf(stderr, "Invalid host address length\n");
            CLOSE_SOCKET(remote_socket);
            return -1;
        }

        if (connect(remote_socket, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) < 0) {
            print_socket_error("Connect to remote server failed");
            CLOSE_SOCKET(remote_socket);
            return -1;
        }
        // send(remote_socket, request.rawRequest.c_str(), request.rawRequest.size(), 0);

        return remote_socket;
    }
};

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        Proxy proxy(LISTEN_PORT);
        proxy.start();
    } catch (const std::exception& e) {
        std::cerr << "Proxy server error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}