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

    static const int c_num_servers=3;

    LoadBalancer();
    
    void run();

private:

    struct Request {
        RequestType type;
        time_t time;

        explicit Request(const std::string& msg);
    };

    struct ServerDescriptor
    {
        time_t remaining_time;
        ServerType type;
        SocketWrapper socket;
        ServerQueue requests_queue;
        ServerQueue feedback_queue;

        ServerDescriptor();
    };

    std::array<ServerDescriptor, c_num_servers> m_servers;
    SocketWrapper m_listener_socket;
    ServerQueue m_calc_queue;
    time_t m_last_time;
    double m_est_arr_time;

    void init_servers_types();
    void listen_clients();
    void calc_dests();
    int get_dest(const Request& req);
    void update_servers_times();
    void apply_time(int server_index, const Request& req);
    std::array<int, c_num_servers> get_goodness_list(const Request& req) const;
    int get_goodness(int server_index, const Request& req) const;
    int get_process_cost(int server_index, const Request& req) const;
    void send_server(int server_index);
    void recv_server(int server_index);

};

#endif // LOAD_BALANCER_HPP
