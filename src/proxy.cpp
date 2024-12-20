#include "../include/proxy.h"

#define ANSI_RED         "\033[31m"
#define ANSI_RESET       "\033[0m"
#define ANSI_GREEN       "\033[32m"
#define ANSI_YELLOW      "\033[33m"
#define ANSI_CONCEALED   "\033[8m"

Proxy::Proxy(int port) : port(port), server_fd(-1), running(false), file_descriptors(), connections(),
                         BLACK_LIST(initFilterList("asset/blocked_domains.txt", "asset/blocked_ips.txt")) {}

void Proxy::setupServerSocket() {
    sockaddr_in server_addr;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        print_socket_error(ANSI_RED "ERROR(Proxy::setupServerSocket):" ANSI_RESET " Socket creation failed");
        exit(EXIT_FAILURE);
    }
    file_descriptors.push_back(server_fd);

    int opt = 1;
    SETSOCKOPT(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port); 

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        print_socket_error(ANSI_RED "ERROR(Proxy::setupServerSocket):" ANSI_RESET " Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CONNECTIONS) < 0) {
        print_socket_error(ANSI_RED "ERROR(Proxy::setupServerSocket):" ANSI_RESET " Listen failed");
        exit(EXIT_FAILURE);
    }
}

int Proxy::start() {
    running = true;

    INIT_SOCKET();
    setupServerSocket();

    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cerr << ANSI_RED << "[ " << std::ctime(&now) << " ] " << ANSI_RESET << "Proxy server started on port " << port << "\n";
   
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    std::thread(&Proxy::acceptConnections, this).detach();
    return server_fd;    
}

void Proxy::stop() {
    running = false;
    if (server_fd != INVALID_SOCKET) {
        CLOSE_SOCKET(server_fd);
    }

    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cerr << ANSI_RED << "[ " << std::ctime(&now) << " ] " << ANSI_RESET << "Proxy server stopped." << "\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

bool Proxy::setPort(int port) {
    this->port = port;
    if (!running) return false;
    stop();
    start();
    return true;
}

int Proxy::getPort() const {
    return port;
}

void Proxy::acceptConnections() {
    while (running) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        socket_t client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);

        if (client_fd == INVALID_SOCKET) {
            if (!running) break;
            print_socket_error(ANSI_RED "ERROR (Proxy::acceptConnections):" ANSI_RESET " Accept failed");
            continue;
        }
        file_descriptors.push_back(client_fd);
        
        std::thread(&Proxy::handleClient, this, client_fd, client_addr).detach();
    }
}

void Proxy::updateConnections(ConnectionInfo conn_info) {
    std::lock_guard<std::mutex> lock(connections_mutex);
    connections.insert(connections.begin(), conn_info);
    if (connections.size() > 100) {
        connections.pop_back();
    }

    log_connection_to_file(conn_info, TextFormat("log/%s-log.txt", std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())).c_str()));
}


void Proxy::handleClient(socket_t client_fd, sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    socket_t remote_fd = -1;
    ConnectionInfo conn_info;
    
    inet_ntop(AF_INET, &(client_addr.sin_addr), conn_info.client.ip, INET_ADDRSTRLEN);
    conn_info.client.port = ntohs(client_addr.sin_port);

    conn_info.time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    try {
        ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_read <= 0) {
            throw std::runtime_error("Failed to receive data from client");
        }

        buffer[bytes_read] = '\0';
        std::string rawRequest(buffer);

        HttpRequest request = parseHttpRequest(rawRequest);
        conn_info.parseServerPort(request);
        HttpResponse response;
        
        sockaddr_in remote_addr;
        std::string host = request.getHeader("Host");

        remote_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (remote_fd == INVALID_SOCKET) {
            print_socket_error(ANSI_RED "ERROR (Proxy::handleClient):" ANSI_RESET " Socket (remote) creation failed");
            throw std::runtime_error("Failed to connect server remote");
        }
        file_descriptors.push_back(remote_fd);

        memset(&remote_addr, 0, sizeof(remote_addr));
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = htons(conn_info.server.port);
        struct hostent* remote_host = gethostbyname(host.c_str());

        if (!remote_host) {
            print_socket_error(ANSI_RED "ERROR (Proxy::handleClient):" ANSI_RESET " Failed to resolve remote domain");
            CLOSE_SOCKET(remote_fd); 
            throw std::runtime_error("Failed to connect server remote");
        }
        memcpy(&remote_addr.sin_addr.s_addr, remote_host->h_addr, remote_host->h_length);
        
        if (unsigned(remote_host->h_length) > sizeof(remote_addr.sin_addr)) {
            print_socket_error(ANSI_RED "ERROR (Proxy::handleClient):" ANSI_RESET " Remote host length is too long");
            CLOSE_SOCKET(remote_fd);
            throw std::runtime_error("Failed to connect server remote");
        }

        if (connect(remote_fd, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) < 0) {
            print_socket_error(ANSI_RED "ERROR (Proxy::handleClient):" ANSI_RESET " Connect to remote server failed");
            CLOSE_SOCKET(remote_fd);
            throw std::runtime_error("Failed to connect server remote");
        }

        if (remote_fd == INVALID_SOCKET) {
            throw std::runtime_error("Failed to connect to remote server");
        }

        inet_ntop(AF_INET, &(remote_addr.sin_addr), conn_info.server.ip, INET_ADDRSTRLEN);
        conn_info.server.port = ntohs(remote_addr.sin_port);

        std::cout << ANSI_GREEN << "[ " << std::ctime(&conn_info.time) << " ] " << ANSI_RESET << "Client " << conn_info.client.ip << ":" << conn_info.client.port << " connected to " << conn_info.server.ip << ":" << conn_info.server.port << "\n";

        if (!isValidHttpMethod(request.method)) {
            std::cout << ANSI_YELLOW << "WARNING: " << ANSI_RESET << "Method is not valid!\n";

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
            std::cout << ANSI_YELLOW << "WARNING: " << ANSI_RESET << "HTTP version is not supported!\n";

            const char *connect_response =  "HTTP/1.1 505 HTTP Version Not Supported\r\n"
                                            "Content-Type: text/plain\r\n"
                                            "Content-Length: 0\r\n\r\n";

            send(client_fd, connect_response, strlen(connect_response), 0);

            response = parseHttpResponse(connect_response);
            conn_info.addTransaction(request, response);
            updateConnections(conn_info);

            throw std::runtime_error("Version HTTP is not supported!\n");
        }

        if (BLACK_LIST.isBlocked(host) || BLACK_LIST.isBlocked(conn_info.server.ip)) {
            std::cout << ANSI_YELLOW << "WARNING: " << ANSI_RESET << "This domain/ip is blocked!\n";

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

            throw std::runtime_error("This domain/ip is blocked!\n");
        }

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        std::string res;

        if (request.method == "CONNECT") {
            std::cout << ANSI_CONCEALED << "Connect successful!\n" << ANSI_RESET;

            char connect_response[] = "HTTP/1.1 200 Connection Established\r\n\r\n";
            
            send(client_fd, connect_response, strlen(connect_response), 0);
            response = parseHttpResponse(connect_response);
            conn_info.addTransaction(request, response);

            bool countClient = 0, countRemote = 0;
            while (running) {
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(client_fd, &fds);
                FD_SET(remote_fd, &fds);
                int max_fd = std::max(client_fd, remote_fd);

                int activity = select(max_fd + 1, &fds, NULL, NULL, &timeout);
                if (activity < 0) {
                    print_socket_error(ANSI_RED "ERROR (Proxy::handleClient):"  ANSI_RESET" Select Error.");
                    break;
                } else if (activity == 0) { 
                    std::cout << ANSI_YELLOW << "WARNING: " << ANSI_RESET << "Timeout reached, ending read loop.\n";
                    break;
                }

                if (FD_ISSET(client_fd, &fds)) {
                    bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
                    if (bytes_read <= 0) break;
                    send(remote_fd, buffer, bytes_read, 0);

                    HttpRequest req = parseHttpRequest(buffer);
                    countClient = true;
                }

                if (FD_ISSET(remote_fd, &fds)) {
                    bytes_read = recv(remote_fd, buffer, BUFFER_SIZE, 0);
                    if (bytes_read <= 0) break;
                    send(client_fd, buffer, bytes_read, 0);
                    
                    response = parseHttpResponse(buffer);
                    countRemote = true;
                }
                if (countClient && countRemote && !conn_info.transactions[0].request.isEncrypted) {
                    conn_info.addTransaction(request, response);
                    countClient = false;
                    countRemote = false;
                }
            }
        } else {
            send(remote_fd, request.rawRequest.c_str(), request.rawRequest.size(), 0);
            while (true) {
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(remote_fd, &fds);

                int activity = select(remote_fd + 1, &fds, NULL, NULL, &timeout);
                if (activity < 0) {
                    print_socket_error(ANSI_RED "ERROR (Proxy::handleClient):" ANSI_RESET " Select Error.");
                    break;
                } else if (activity == 0) {
                    std::cout << ANSI_YELLOW << "WARNING: " << ANSI_RESET << "Timeout reached, ending read loop.\n";
                    break;
                }

                ssize_t bytes_read = recv(remote_fd, buffer, BUFFER_SIZE, 0);
                if (bytes_read <= 0) break;
                send(client_fd, buffer, bytes_read, 0); 

                res += buffer;
            }
            res += "\n";
            response = parseHttpResponse(res);
            conn_info.addTransaction(request, response);
        }
        updateConnections(conn_info);
    } catch (const std::exception& e) {
        if (remote_fd != INVALID_SOCKET) CLOSE_SOCKET(remote_fd);
        CLOSE_SOCKET(client_fd);
        std::cerr << ANSI_RED << "[ " << std::ctime(&conn_info.time) << " ] " << "- Program throw exception: " << e.what() << "\n";
    }

    if (remote_fd != INVALID_SOCKET) CLOSE_SOCKET(remote_fd);
    CLOSE_SOCKET(client_fd);
}