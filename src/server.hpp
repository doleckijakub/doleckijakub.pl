#pragma once

#include "request.hpp"
#include <functional>
#include <map>
#include <string>

using HandlerFunction = std::function<std::string(const Request&)>;

class Server {
public:
    explicit Server(int port);

    void dispatch(HttpMethod method, const std::string& path, HandlerFunction handler);
    void start();

private:
    int port;
    std::map<std::pair<HttpMethod, std::string>, HandlerFunction> routes;

    std::string handle(const Request& req) const;
};

