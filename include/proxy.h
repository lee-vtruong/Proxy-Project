#ifndef PROXY_H
#define PROXY_H

#include "domain_process.h"
#include "http_parser.h"

class Proxy {
private:
    int port;
    socket_t server_fd;
    bool running;
    std::mutex connections_mutex;

    void updateConnections(ConnectionInfo conn_info);
    void setupServerSocket();
    void handleClient(socket_t client_fd, sockaddr_in client_addr);
    void acceptConnections();

public:
    std::vector<socket_t> file_descriptors;
    std::vector<ConnectionInfo> connections;
    FilterList BLACK_LIST;
    Proxy(int port);

    void stop();
    int start();
    bool setPort(int port);
    int getPort() const;

};

#endif // PROXY_H