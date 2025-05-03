#include "server.hpp"
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

const int buffer_size = 4096;

Server::Server(int port_number) : port(port_number) {}

void Server::dispatch(HttpMethod method, const std::string& path, HandlerFunction handler) {
    routes[{method, path}] = handler;
}

std::string Server::handle(const Request& req) const {
    auto it = routes.find({req.method, req.path});
    if (it != routes.end()) {
        return it->second(req);
    }
    return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found";
}

void Server::start() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);

    std::cout << "Server listening on port " << port << std::endl;

    while (true) {
        client_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client_socket >= 0) {
            char buffer[buffer_size];
            memset(buffer, 0, buffer_size);
            read(client_socket, buffer, buffer_size - 1);
            Request req = parse_request(buffer);
            std::string response = handle(req);
            write(client_socket, response.c_str(), response.size());
            close(client_socket);
        }
    }
}

