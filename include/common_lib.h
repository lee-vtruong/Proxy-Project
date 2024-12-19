#ifndef COMMON_LIB_H
#define COMMON_LIB_H

#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#define BUFFER_SIZE 65536
#define LISTEN_PORT 8080
#define MAX_CONNECTIONS 100

#define blockedDomainsFile "asset/blocked_domains.txt"
#define blockedIPsFile "asset/blocked_ip.txt"

const Color MY_BACKGROUND_COLOR {166, 174, 191, 255};
const Color NORMAL_TEXT_COLOR = {73, 82, 79, 255};
const Color HOVERED_TEXT_COLOR = {255, 255, 255, 255};
const Color NORMAL_BUTTON_COLOR = {255, 116, 139, 255};
const Color PRIMARY_BUTTON_COLOR = {62, 88, 121, 255};
const Color PRIMARY_HOVERED_BUTTON_COLOR = {92, 118, 151, 255};
const Color SECONDARY_HOVERED_BUTTON_COLOR = {225, 86, 109, 255};
const Color FILLED_COLOR = {255, 248, 222, 255};
const Color EMPTY_COLOR = FILLED_COLOR;
const Color TITLE_COLOR = {188, 212, 177, 255};
const Color DATA_COLOR = {100, 100, 100, 255};
const Color CONTENT_BOX_COLOR = {238, 211, 177, 240};
const Color PRESS_COLOR = {200, 0, 0, 255};


std::string readFile(const char* filename);

#endif