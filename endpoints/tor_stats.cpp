#include "endpoint-base.hpp"

#include ".secrets/endpoints/tor.secret"

#include "profiler.hpp"

#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>

namespace endpoint {
namespace tor_stats {

class TorApi {
    int fd;

public:

    TorApi(const char *ip, int port, const char *password) {
        PROFILE_FUNC;

        if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) throw std::runtime_error("socket() failed");
        
        struct sockaddr_in address = (sockaddr_in) {
            .sin_family = AF_INET,
            .sin_port   = htons(port)
        };

        if (inet_pton(AF_INET, ip, &address.sin_addr) < 0) throw std::runtime_error("inet_pton() failed");

        if (connect(fd, (struct sockaddr *) &address, sizeof(address)) < 0) throw std::runtime_error("connect() failed");

        send(std::string("AUTHENTICATE \"") + password + "\"");
        assert_recv("250 OK\r\n");

    }

    ~TorApi() {
        PROFILE_FUNC;
        close(fd);
    }

    void send(const std::string &data) {
        PROFILE_FUNC;
        ::send(fd, data.c_str(), data.length(), 0);
        ::send(fd, "\r\n", 2, 0);
    }

    uint8_t recv_u8() {
        PROFILE_FUNC;
        uint8_t result;
        ::recv(fd, &result, 1, 0);
        return result;
    }

    void assert_recv(const std::string &data) {
        PROFILE_FUNC;
        for (int i = 0; data[i]; i++) {
            assert(recv_u8() == data[i]);
        }
    }

    uint64_t traffic_read() {
        PROFILE_FUNC;
        send("GETINFO traffic/read");
        assert_recv("250-traffic/read=");
        
        uint8_t c;
        uint64_t result = 0;

        while ((c = recv_u8()) != '\r') {
            result *= 10;
            result += c - '0';
        }

        assert_recv("\n250 OK\r\n");

        return result;
    }

    uint64_t traffic_written() {
        PROFILE_FUNC;
        send("GETINFO traffic/written");
        assert_recv("250-traffic/written=");
        
        uint8_t c;
        uint64_t result = 0;

        while ((c = recv_u8()) != '\r') {
            result *= 10;
            result += c - '0';
        }

        assert_recv("\n250 OK\r\n");

        return result;
    }

};

endpointDispatchResult serve(http::request &req) {
    static TorApi tor_api(TOR_IP, TOR_CONTROL_PORT, TOR_PASSWORD);
    
    uint64_t traffic_read = tor_api.traffic_read();
    uint64_t traffic_written = tor_api.traffic_written();

    char response[256] = { 0 };
    snprintf(response, 256, "{\"read\":%" PRIu64 ",\"written\":%" PRIu64"}", traffic_read, traffic_written);

    http::response res = req.response();

    res.setStatus(200);
    res.setContentType(http::content_type::APPLICATION_JSON);
    res.setContentString(response);

    return { true, res.send() };
}

} // namespace tor_stats
} // namespace endpoint
