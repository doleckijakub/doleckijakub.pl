#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <fstream>

const int port_number = 8080;
const int buffer_size = 4096;

void handle_client(int client_socket) {
    char buffer[buffer_size];
    memset(buffer, 0, buffer_size);
    read(client_socket, buffer, buffer_size - 1);

    std::string request(buffer);
    std::string response;

    if (request.find("GET /style.css") != std::string::npos) {
        std::ifstream css_file("static/style.css");
        std::string css_content((std::istreambuf_iterator<char>(css_file)),
                                 std::istreambuf_iterator<char>());

        response = "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n" + css_content;
    } else {
        response =
            "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
            "<html><head><link rel=\"stylesheet\" href=\"/style.css\"></head>"
            "<body><h1>Welcome to doleckijakub.pl</h1></body></html>";
    }

    write(client_socket, response.c_str(), response.length());
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_number);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 10);

    std::cout << "Server listening on port " << port_number << std::endl;

    while (true) {
        client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_socket >= 0) {
            handle_client(client_socket);
        }
    }

    return 0;
}

