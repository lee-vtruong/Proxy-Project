#ifndef UTILS_H
#define UTILS_H

#include "common_lib.h"

void read_domains(const char* filename, std::vector<std::string>& domain_list);
bool is_blocked_url(std::string url, std::vector<std::string> BLOCKED_DOMAIN);

#endif
