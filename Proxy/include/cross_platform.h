#ifndef CROSS_PLATFORM_H
#define CROSS_PLATFORM_H

#include "common_lib.h"

#if defined(_WIN32) || defined(_WIN64)
    #define IS_WINDOWS 1
#else
    #define IS_WINDOWS 0
#endif

#if IS_WINDOWS
    // Include Windows-specific headers
    #include "netinc.h"
    #include <ws2tcpip.h>

    // #pragma comment(lib, "ws2_32.lib")
    
    // Type and macro definitions for Windows
    typedef SOCKET socket_t;
    #define SOCKET_ERROR_CODE WSAGetLastError()
    #define CLOSE_SOCKET closesocket
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
    // Include Linux-specific headers
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <errno.h>
    
    // Type and macro definitions for Linux
    typedef int socket_t;
    #define SOCKET_ERROR_CODE errno
    #define CLOSE_SOCKET close
    #define INIT_SOCKET() (void)0
    #define CLEANUP_SOCKET() (void)0
    #define SETSOCKOPT(s, level, optname, optval, optlen) \
        setsockopt(s, level, optname, optval, optlen)
    #define INVALID_SOCKET ((socket_t)-1)
    
#endif

// Inline function for error handling
static inline void print_socket_error(const char *msg) {
#if IS_WINDOWS
    fprintf(stderr, "%s: %d\n", msg, SOCKET_ERROR_CODE);
#else
    perror(msg);
#endif
}

// Optional: define macros for common socket functions
#define SOCKET_SEND send
#define SOCKET_RECV recv
#define SOCKET_BIND bind
#define SOCKET_LISTEN listen
#define SOCKET_ACCEPT accept

#endif // CROSS_PLATFORM_H
