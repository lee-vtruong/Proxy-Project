#ifndef CROSS_PLATFORM_H
#define CROSS_PLATFORM_H

#include "common_lib.h"

#if defined(_WIN32) || defined(_WIN64)
    #define IS_WINDOWS 1
#else
    #define IS_WINDOWS 0
#endif

#if IS_WINDOWS
    #include "netinc.h"
    #include <ws2tcpip.h>
    
    typedef SOCKET socket_t;
    #define SOCKET_ERROR_CODE WSAGetLastError()
    
    #define CLOSE_SOCKET(fd) do {\
        shutdown((fd), SD_BOTH); \
        closesocket(fd); \
    } while (0)

    #define INIT_SOCKET() do { \
        WSADATA wsaData; \
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { \
            fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError()); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

    #define CLEANUP_SOCKET() WSACleanup()
    #define SETSOCKOPT(s, level, optname, optval, optlen) \
        setsockopt(s, level, optname, (const char*)(optval), optlen)

#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <errno.h>
    
    typedef int socket_t;
    #define SOCKET_ERROR_CODE errno

    #define CLOSE_SOCKET(fd) do { \
        shutdown((fd), SHUT_RDWR); \
        close(fd); \
    } while (0)

    #define INIT_SOCKET() (void)0
    #define CLEANUP_SOCKET() (void)0
    #define SETSOCKOPT(s, level, optname, optval, optlen) \
        setsockopt(s, level, optname, optval, optlen)

    #define INVALID_SOCKET ((socket_t)-1)
#endif

inline void print_socket_error(const char *msg) {
#if IS_WINDOWS
    fprintf(stderr, "%s: %d\n", msg, SOCKET_ERROR_CODE);
#else
    perror(msg);
#endif
}

#endif // CROSS_PLATFORM_H
