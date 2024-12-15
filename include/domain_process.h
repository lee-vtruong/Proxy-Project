#ifndef UTILS_H
#define UTILS_H

#include "common_lib.h"

struct FilterList {
    std::unordered_set<std::string> domains; 
    std::unordered_set<std::string> ips;     

    void addDomain(const std::string& domain);

    void addIP(const std::string& ip);

    bool isBlocked(const std::string& entry) const;
};

bool loadListFromFile(const char*  filePath, std::unordered_set<std::string>& list);
void isBlocked(const std::string& entry, const FilterList& filterList);
FilterList initFilterList(const char* domainFile, const char* ipFile);
    
#endif
