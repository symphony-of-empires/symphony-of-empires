#include <cstdio>

#include "unified_render/log.hpp"

void UnifiedRender::Log::debug(const std::string& category, const std::string& msg) {
    FILE* fp = fopen("log.txt", "a+t");
    if(fp) {
        fprintf(fp, "<%s> %s\n", category.c_str(), msg.c_str());
        fclose(fp);
    }
}

void UnifiedRender::Log::warning(const std::string& category, const std::string& msg) {
    FILE* fp = fopen("log.txt", "a+t");
    if(fp) {
        fprintf(fp, "<%s> %s\n", category.c_str(), msg.c_str());
        fclose(fp);
    }
}

void UnifiedRender::Log::error(const std::string& category, const std::string& msg) {
    FILE* fp = fopen("log.txt", "a+t");
    if(fp) {
        fprintf(fp, "<%s> %s\n", category.c_str(), msg.c_str());
        fclose(fp);
    }
}