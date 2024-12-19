#include "../include/domain_process.h"
 
//------------------------ FilterList ------------------------
void FilterList::addDomain(const std::string& domain) {
    domains.insert(domain);
}

void FilterList::addIP(const std::string& ip) {
    ips.insert(ip);
}

bool FilterList::isBlocked(const std::string& entry) const {
    // Kiểm tra domain
    for (const auto& domain : domains) {
        if (entry == domain || 
            (entry.size() > domain.size() && 
             entry.compare(entry.size() - domain.size(), domain.size(), domain) == 0 &&
             entry[entry.size() - domain.size() - 1] == '.')) {
            return true;
        }
    }
    for (const auto& ip : ips) {
        std::string res;
        size_t end = entry.find_last_not_of("\r\n");
        if (end != std::string::npos) {
            res = entry.substr(0, end + 1);
        } else {
            res.clear();
        }
        if (res == ip) {
            return true;
        }
    }

    return false;
}

// ------------------------ Utils ------------------------
bool loadListFromFile(const char*  filePath, std::unordered_set<std::string>& list) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Không thể mở file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        line.erase(0, line.find_first_not_of(" \n\r\t"));
        if (!line.empty()) {
            list.insert(line);
        }
    }

    file.close();
    return true;
}

FilterList initFilterList(const char* domainFile, const char* ipFile) {
    FilterList filterList;

    if (loadListFromFile(domainFile, filterList.domains)) {
        std::cerr << "Open Domains File Successfully.\n";
    } else {
        std::cerr << "Cannot open domain file.\n";
    }

    if (loadListFromFile(ipFile, filterList.ips)) {
        std::cerr << "Open IPs File Successfully.\n";
    } else {
        std::cerr << "Cannot open IP file.\n";
    }

    return filterList;
}
