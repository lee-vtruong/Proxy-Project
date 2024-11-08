#include "../include/domain_process.h"
#include "../include/http_parser.h"
#include "../include/cross_platform.h"

#define BUFFER_SIZE 65536
#define PORT 8080

std::vector<std::string> BLOCKED_DOMAIN;

http_connection handle_connection(int client_fd) {
    http_connection connection;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
    char request[BUFFER_SIZE];
    memcpy(request, buffer, BUFFER_SIZE);
    if (bytes_read <= 0) {
        close(client_fd);
        return connection;
    }
    // printf("\033[31mRequest\033[0m: %s\n", buffer);

    socklen_t addr_len = sizeof(connection.client_addr);
    getpeername(client_fd, (struct sockaddr *)&connection.client_addr, &addr_len);

    connection.requests.push_back(parser_request(buffer));
    if (connection.requests.back().method == n_METHOD) {
        fprintf(stderr, "Method is not valid!\n");
        const char* connect_response =  "HTTP/1.1 400 Bad Request\r\n"
                                        "Content-Type: text/plain\r\n"
                                        "Content-Length: 0\r\n\r\n";
        send(client_fd, connect_response, strlen(connect_response), 0);
        close(client_fd);
        log_connection_information(connection, stderr);
        return connection;
    }
    if (connection.requests.back().version == n_VERSION) {
        fprintf(stderr, "Version HTTP is not supported!\n");
        const char *connect_response =  "HTTP/1.1 505 HTTP Version Not Supported\r\n"
                                        "Content-Type: text/plain\r\n"
                                        "Content-Length: 0\r\n\r\n";
        send(client_fd, connect_response, strlen(connect_response), 0);
        close(client_fd);
        log_connection_information(connection, stderr);
        return connection;
    }

    char *host_start = strstr(buffer, "Host: ");
    if (host_start == NULL) {
        close(client_fd);
        log_connection_information(connection, stderr);
        return connection;
    }
    host_start += 6;

    char *host_end = strstr(host_start, "\r\n");
    if (host_end) *host_end = '\0';

    char *port_start = strrchr(host_start, ':');
    if (port_start) *port_start = '\0';

    struct hostent *host = gethostbyname(host_start);
    if (!host) {
        fprintf(stderr, "Failed to resolve host: %s\n", host_start);
        close(client_fd);
        log_connection_information(connection, stderr);
        return connection;
    }
    if (is_blocked_url(connection.requests.front().url, BLOCKED_DOMAIN)) {
        fprintf(stderr, "THIS HOST IS BLOCKED!\n");
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
        close(client_fd);
        log_connection_information(connection, stderr);
        return connection;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Server socket creation failed");
        close(client_fd);
        log_connection_information(connection, stderr);
        return connection;
    }

    struct timeval timeout;
    timeout.tv_sec = 10;  // timeout sau 5 giây
    timeout.tv_usec = 0;

    if (connection.requests.front().method == 0) {
        int port = port_start ? atoi(port_start + 1) : 443;
        connection.server_addr.sin_family = AF_INET;
        connection.server_addr.sin_port = htons(port);
        memcpy(&connection.server_addr.sin_addr.s_addr, host->h_addr, host->h_length);
        if (connect(server_fd, (struct sockaddr *)&connection.server_addr, sizeof(connection.server_addr)) < 0) {
            perror("Connection to server failed");
            close(client_fd);
            close(server_fd);
            log_connection_information(connection, stderr);
            return connection;
        }
        char connect_response[] = "HTTP/1.1 200 Connection Established\r\n\r\n";
        connection.responses.push_back(parser_response(connect_response));
        send(client_fd, connect_response, strlen(connect_response), 0);

        while (1) {
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(client_fd, &fds);
            FD_SET(server_fd, &fds);
            int max_fd = std::max(client_fd, server_fd);

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
                connection.requests.push_back(parser_request(buffer));
                send(server_fd, buffer, bytes_read, 0);
            }

            if (FD_ISSET(server_fd, &fds)) {
                bytes_read = recv(server_fd, buffer, BUFFER_SIZE, 0);
                if (bytes_read <= 0) break;
                // connection.responses.push_back(parser_response(buffer));
                send(client_fd, buffer, bytes_read, 0);
            }
        }

        close(server_fd);
    } else {
        int port = port_start ? atoi(port_start + 1) : 80;
        connection.server_addr.sin_family = AF_INET;
        connection.server_addr.sin_port = htons(port);
        memcpy(&connection.server_addr.sin_addr.s_addr, host->h_addr, host->h_length);
        if (connect(server_fd, (struct sockaddr *)&connection.server_addr, sizeof(connection.server_addr)) < 0) {
            perror("Connection to server failed");
            close(client_fd);
            close(server_fd);
            log_connection_information(connection, stderr);
            return connection;
        }
        send(server_fd, request, BUFFER_SIZE, 0);
        while (true) {
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(server_fd, &fds);

            int activity = select(server_fd + 1, &fds, NULL, NULL, &timeout);
            if (activity < 0) {
                perror("Select error");
                break;
            } else if (activity == 0) {
                // printf("Timeout reached, ending read loop.\n");
                break;
            }

            ssize_t bytes_read = recv(server_fd, buffer, BUFFER_SIZE, 0);
            
            if (bytes_read <= 0) {
                break;
            }
            send(client_fd, buffer, bytes_read, 0); 
            connection.responses.push_back(parser_response(buffer));
        }

    }

    log_connection_information(connection, stderr);

    close(client_fd);
    return connection;
}



int main() {
    int proxy_fd;
    struct sockaddr_in proxy_addr;
    socklen_t client_len = sizeof(proxy_addr);
    if ((proxy_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    setsockopt(proxy_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_addr.s_addr = INADDR_ANY;
    proxy_addr.sin_port = htons(PORT);

    if (bind(proxy_fd, (struct sockaddr *)&proxy_addr, sizeof(proxy_addr)) < 0) {
        perror("Bind failed");
        close(proxy_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(proxy_fd, 10) < 0) {
        perror("Listen failed");
        close(proxy_fd);
        exit(EXIT_FAILURE);
    }
    printf("Proxy server is listening on port %d\n", PORT);

    read_domains("asset/blocked_domains.txt", BLOCKED_DOMAIN);
    for (auto s : BLOCKED_DOMAIN) std::cout << s << "\n";
    
    std::vector<std::future<http_connection>> futures;
    std::vector<http_connection> connections;
    while (1) {
        int client_fd = accept(proxy_fd, (struct sockaddr *)&proxy_addr, &client_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }
        try {
            futures.push_back(std::async(std::launch::async, handle_connection, client_fd));

        }  catch (const std::exception &e) {
            std::cerr << "Exception caught 1: " << e.what() << std::endl;
            continue;
        }
        try {
            for (auto it = futures.begin(); it != futures.end();) {
                if (it->wait_for(std::chrono::microseconds(0)) == std::future_status::ready) {
                    connections.push_back(it->get());
                    it = futures.erase(it);
                } else {
                    ++it;
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "Exception caught 2: " << e.what() << std::endl;
            continue;
        }
    }

    close(proxy_fd);
    return 0;
}
