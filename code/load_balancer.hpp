#ifndef LOAD_BALANCER_HPP  
#define LOAD_BALANCER_HPP

#include "socket_wrapper.hpp"

#include <vector>
#include <array>
#include <string>
#include <queue>

#define NUM_SERVERS 3

class load_balancer
{
public:
    load_balancer(); // TODO: this also needs to initialize the queues
    void connect_servers();
    void listen_clients();

private:

    enum class ServerType {
        VIDEO,
        MUSIC
    };

    enum class RequestType {
        PICTURE,
        VIDEO,
        MUSIC
    };

    struct Request {
        RequestType type;
        int time;

        Request(std::string msg):time(msg[1] - '0') {
            switch (msg[0]) {
            case 'P':
                type = RequestType::PICTURE;
                break;
            case 'V':
                type = RequestType::VIDEO;
                break;
            case 'M':
                type = RequestType::MUSIC;
                break;
            }
        }
    };

    struct server_descriptor
    {
        double remaining_time; //TODO: change type to time
        ServerType type;
        socket_wrapper socket;
        ServerQueue requests_queue;
    };

    std::array<server_descriptor, NUM_SERVERS> servers;
    socket_wrapper listener_socket;

    int get_dest(Request req);
};

#endif // LOAD_BALANCER_HPP