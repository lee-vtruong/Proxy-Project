#include "../include/domain_process.h"

void read_domains(const char* filename, std::vector<std::string>& domain_list) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Cannot load BlackList: " << filename << std::endl;
        return;
    }

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v")); 
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        if (line.empty()) {
            continue;
        }
        domain_list.push_back(line);
    }

    file.close();
}

bool is_blocked_url(std::string url, std::vector<std::string> BLOCKED_DOMAIN) {
    for (auto domain : BLOCKED_DOMAIN) {
        if (url.find(domain) != std::string::npos) return true;
    }
    return false;
}
