#include "server.hpp"
#include "request.hpp"
#include <fstream>

int main() {
    Server server(8080);

    server.dispatch(HttpMethod::GET, "/", [](const Request& req) {
        std::string body =
            "<html><head><link rel=\"stylesheet\" href=\"/style.css\"></head>"
            "<body><h1>Welcome to doleckijakub.pl</h1>"
            "<ul>"
            "<li><a href='https://fusion.doleckijakub.pl'>fusion.doleckijakub.pl</a></li>"
            "<li><a href='https://mirrors.doleckijakub.pl'>mirrors.doleckijakub.pl</a></li>"
            "<li><a href='https://cdn.doleckijakub.pl'>cdn.doleckijakub.pl</a></li>"
            "<li><a href='https://cppreference.doleckijakub.pl'>cppreference.doleckijakub.pl</a></li>"
            "<li><a href='https://rust-docs.doleckijakub.pl'>rust-docs.doleckijakub.pl</a></li>"
            "</ul>"
            "<h2>Projects</h2><ul>"
            "<li><a href='https://github.com/doleckijakub/tadeusz'>AI Voice Assistant - Tadeusz</a> (WIP)</li>"
            "<li><a href='https://github.com/doleckijakub/webnes'>Web-based NES Emulator</a> (WIP)</li>"
            "</ul></body></html>";

        return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + body;
    });

    server.dispatch(HttpMethod::GET, "/style.css", [](const Request& req) {
        std::ifstream css_file("static/style.css");
        if (!css_file.is_open()) {
            return std::string("HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found");
        }
        std::string css((std::istreambuf_iterator<char>(css_file)), std::istreambuf_iterator<char>());
        return "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n" + css;
    });

    server.start();
    return 0;
}

