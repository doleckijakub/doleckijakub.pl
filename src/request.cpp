#include "request.hpp"
#include <sstream>

HttpMethod parse_method(const std::string& method_str) {
    if (method_str == "GET") return HttpMethod::GET;
    if (method_str == "POST") return HttpMethod::POST;
    return HttpMethod::UNKNOWN;
}

Request parse_request(const std::string& raw) {
    std::istringstream stream(raw);
    std::string line;
    Request req { HttpMethod::UNKNOWN, "/", "" };

    if (std::getline(stream, line)) {
        std::istringstream line_stream(line);
        std::string method, path;
        line_stream >> method >> path;
        req.method = parse_method(method);
        req.path = path;
    }

    while (std::getline(stream, line) && line != "\r") {
        if (line.find("User-Agent:") != std::string::npos) {
            req.user_agent = line.substr(12);
            while (!req.user_agent.empty() && (req.user_agent.front() == ' ' || req.user_agent.front() == '\t'))
                req.user_agent.erase(0, 1);
        }
    }

    return req;
}

