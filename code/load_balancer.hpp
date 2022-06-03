#ifndef LOAD_BALANCER_HPP  
#define LOAD_BALANCER_HPP

#include "socket_wrapper.hpp"
#include "server_queue.hpp"

#include <array>
#include <queue>
#include <string>
#include <vector>

#define NUM_SERVERS 3

class LoadBalancer
{

public:

    LoadBalancer(); // TODO: this also needs to initialize the queues

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

        Request(std::string msg);
    };

    struct ServerDescriptor
    {
        double remaining_time; //TODO: change type to time
        ServerType type;
        SocketWrapper socket;
        ServerQueue requests_queue;
    };

    std::array<ServerDescriptor, NUM_SERVERS> m_servers;
    SocketWrapper m_listener_socket;
    ServerQueue m_calc_queue;

    void connect_servers();
    void listen_clients();
    void calc_dests();
    int get_dest(Request req);
    
};

#endif // LOAD_BALANCER_HPP