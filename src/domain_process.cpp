#include "../include/domain_process.h"
 

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

    // Kiểm tra IP
    return ips.count(entry) > 0;
}


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

void isBlocked(const std::string& entry, const FilterList& filterList) {
    if (filterList.isBlocked(entry)) {
        std::cout << "Message bị chặn: " << entry << std::endl;
    } else {
        std::cout << "Message hợp lệ: " << entry << std::endl;
    }
}

FilterList initFilterList(const char* domainFile, const char* ipFile) {
    FilterList filterList;

    if (loadListFromFile(domainFile, filterList.domains)) {
        std::cout << "Danh sách domain đã được load thành công.\n";
    } else {
        std::cout << "Cannot open domain file.\n";
    }

    if (loadListFromFile(ipFile, filterList.ips)) {
        std::cout << "Danh sách IP đã được load thành công.\n";
    } else {
        std::cout << "Cannot open IP file.\n";
    }
    return filterList;
}
