#include "server.hpp"
#include "request.hpp"

#include <utility>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <cinttypes>
#include <thread>
#include <chrono>

int main() {
    Server server(8080);

    server.dispatch(HttpMethod::GET, "/", [](const Request& req) {
        std::ifstream file("static/index.html");
        if (!file.is_open()) {
            return std::string("HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found");
        }
        std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + contents;
    });

    server.dispatch(HttpMethod::GET, "/style.css", [](const Request& req) {
        std::ifstream file("static/style.css");
        if (!file.is_open()) {
            return std::string("HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found");
        }
        std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n" + contents;
    });

    server.dispatch(HttpMethod::GET, "/index.js", [](const Request& req) {
        std::ifstream file("static/index.js");
        if (!file.is_open()) {
            return std::string("HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found");
        }
        std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return "HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\n\r\n" + contents;
    });

    server.dispatch(HttpMethod::GET, "/api/cpu_usage", [](const Request& req) {
        auto read_usage = []() -> std::pair<uint64_t, uint64_t> {
            std::ifstream file("/proc/stat");
        
            std::string line;
            std::getline(file, line);
            std::istringstream iss(line);
            std::string cpu;

            uint64_t user, nice, system, idle, iowait, irq, softirq;
            iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;

            uint64_t used = user + nice + system;
            uint64_t total = user + nice + system + idle + iowait + irq + softirq;

            return { used, total };
        };

        auto t0 = read_usage();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto t1 = read_usage();

        uint64_t used_delta = t1.first - t0.first;
        uint64_t total_delta = t1.second - t0.second;

        uint64_t usage = total_delta == 0 ? 0 : 100 * used_delta / total_delta;

        return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n" + std::to_string(usage);
    });

    server.dispatch(HttpMethod::GET, "/api/mem_total_kb", [](const Request& req) {
        std::ifstream file("/proc/meminfo");

        std::string line;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key;
            uint64_t value;
            std::string unit;
            iss >> key >> value >> unit;

            if (key == "MemTotal:") {
                return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n" + std::to_string(value);
            }
        }

        return std::string("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nSomething went wrong :(");
    });

    server.dispatch(HttpMethod::GET, "/api/mem_usage_kb", [](const Request& req) {
        std::ifstream file("/proc/meminfo");

        std::string line;

        uint64_t total_kb = 0;
        uint64_t available_kb = 0;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key;
            uint64_t value;
            std::string unit;
            iss >> key >> value >> unit;

            if (key == "MemTotal:") total_kb = value;
            if (key == "MemAvailable:") available_kb = value;

            if (total_kb && available_kb) break;
        }

        return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n" + std::to_string(total_kb - available_kb);
    });

    server.start();
    return 0;
}

