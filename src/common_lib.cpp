#include "../include/common_lib.h"

const char* HTTP_METHOD[] = {"CONNECT", "GET", "PUT", "POST", "HEAD", "OPTIONS", "PATCH"};
const int n_METHOD = sizeof(HTTP_METHOD)/sizeof(HTTP_METHOD[0]);
const char* HTTP_VERSION[] = {"HTTP/1.0", "HTTP/1.1"};
const int n_VERSION = sizeof(HTTP_VERSION)/sizeof(HTTP_VERSION[0]);

int min(int a, int b) {
    if (a < b) return a;
    return b;
}