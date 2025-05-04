#pragma once

#include <string>
#include <unordered_map>

enum class HttpMethod {
    GET,
    POST,
    UNKNOWN
};

inline std::string to_string(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET:     return "GET";
        case HttpMethod::POST:    return "POST";
        default:                  return "UNKNOWN";
    }
}


struct Request {
    HttpMethod method;
    std::string path;
    std::string user_agent;
};

HttpMethod parse_method(const std::string& method_str);
Request parse_request(const std::string& raw);

