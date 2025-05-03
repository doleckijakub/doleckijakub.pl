#pragma once

#include <string>
#include <unordered_map>

enum class HttpMethod {
    GET,
    POST,
    UNKNOWN
};

struct Request {
    HttpMethod method;
    std::string path;
    std::string user_agent;
};

HttpMethod parse_method(const std::string& method_str);
Request parse_request(const std::string& raw);

