#ifndef CROSS_PLATFORM_PROXY_H
#define CROSS_PLATFORM_PROXY_H

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h> // cho ipv6
    #define CLOSE_SOCKET(s) closesocket(s)
    #define SOCKET_ERROR_VALUE SOCKET_ERROR
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h> 
    #define CLOSE_SOCKET(s) close(s)
    #define SOCKET_ERROR_VALUE -1
#endif

int socket_initialize();
void socket_cleanup();

#if defined(_WIN32) || defined(_WIN64)
int socket_initialize() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData); 
}

void socket_cleanup() {
    WSACleanup(); 
}
#else
int socket_initialize() {
    return 0;
}

void socket_cleanup() {
    return;
}
#endif

#endif