#define NBNET_IMPL

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif

#include "../include/netinc.h"
#include "../include/common_lib.h"

std::string readFile(const char* filename) {
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Không thể mở tệp: " << filename << std::endl;
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string fileContent = buffer.str();

    return fileContent; 
}