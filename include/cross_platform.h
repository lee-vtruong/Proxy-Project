#ifndef CROSS_PLATFORM_H
#define CROSS_PLATFORM_H

#include "common_lib.h"

#if defined(_WIN32) || defined(_WIN64)
    #define IS_WINDOWS 1
#else
    #define IS_WINDOWS 0
#endif

#if IS_WINDOWS
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <net/ethernet.h>
    #include <netinet/in.h>
    #include <netinet/ip.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <errno.h>
#endif

#if IS_WINDOWS
    typedef SOCKET socket_t;
    #define SOCKET_ERROR_CODE WSAGetLastError()
    #define CLOSE_SOCKET closesocket
    #define INIT_SOCKET()                    
        do {                                
            WSADATA wsaData;                
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                fprintf(stderr, "WSAStartup failed.\n");
                exit(EXIT_FAILURE);         
            }                               
        } while (0)
    #define CLEANUP_SOCKET() WSACleanup()
#else
    typedef int socket_t;
    #define SOCKET_ERROR_CODE errno
    #define CLOSE_SOCKET close
    #define INIT_SOCKET() (void)0
    #define CLEANUP_SOCKET() (void)0
#endif

static inline void print_socket_error(const char *msg) {
#if IS_WINDOWS
    fprintf(stderr, "%s: %d\n", msg, SOCKET_ERROR_CODE);
#else
    perror(msg);
#endif
}

#endif