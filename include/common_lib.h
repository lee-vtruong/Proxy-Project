#ifndef COMMON_LIB_H
#define COMMON_LIB_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <thread>
#include <net/ethernet.h>
#include <pcap.h>
#include <vector>
#include <future>
#include <chrono>

extern const char* HTTP_METHOD[];
extern const int n_METHOD;
extern const char* HTTP_VERSION[];
extern const int n_VERSION;

int min(int a, int b);

#endif