#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <cstring>

inline const char *now_str() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    static char buffer[32];
    std::strncpy(buffer, std::ctime(&now), 30);
    buffer[std::strlen(buffer) - 1] = '\0';
    return buffer;
}

inline void log_info(const std::string& message) {
    std::cout << "[INFO] " << now_str() << " --- " << message << std::endl;
}

inline void log_error(const std::string& message) {
    std::cerr << "[ERROR] " << now_str() << " --- " << message << std::endl;
}

