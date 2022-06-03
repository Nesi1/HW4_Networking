#ifndef LOAD_BALANCER_HPP  
#define LOAD_BALANCER_HPP

#include "socket_wrapper.hpp"
#include "server_queue.hpp"

#include <array>
#include <atomic>
#include <queue>
#include <string>
#include <vector>

class LoadBalancer
{

public:

    static const int c_num_servers=3;

    LoadBalancer();
    
    void run();

private:

    enum class ServerType {
        NONE,
        VIDEO,
        MUSIC
    };

    enum class RequestType {
        NONE,
        PICTURE,
        VIDEO,
        MUSIC
    };

    struct Request {
        RequestType type;
        int time;

        explicit Request(std::string msg);
    };

    struct ServerDescriptor
    {
        double remaining_time; //TODO: change type to time
        ServerType type;
        SocketWrapper socket;
        ServerQueue requests_queue;
        ServerQueue feedback_queue;

        ServerDescriptor();
    };

    std::array<ServerDescriptor, c_num_servers> m_servers;
    SocketWrapper m_listener_socket;
    ServerQueue m_calc_queue;

    void listen_clients();
    void calc_dests();
    int get_dest(Request req);
    std::array<int, c_num_servers> get_goodness(Request req);
    void send_server(int server_index);
    void recv_server(int server_index);

};

#endif // LOAD_BALANCER_HPP
