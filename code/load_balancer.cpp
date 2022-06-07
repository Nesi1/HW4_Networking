#include "load_balancer.hpp"

#include "utility.hpp"

#include <algorithm>
#include <cassert>
#include <map>
#include <string>
#include <thread>

using namespace std;

const int c_port = 80;
const int c_message_length = 2;
const int c_backlog = 64;
const int c_purity_purge = 8;

const double c_init_est_arr_time = 0.25;
const double c_arr_time_bias = 0.75;
const double c_purity_factor = 3.0;

const string c_hosts_iface_addr = "10.0.0.1";
const array<string, LoadBalancer::c_num_servers> c_server_addrs {
    "192.168.0.101",
    "192.168.0.102",
    "192.168.0.103"
};

LoadBalancer::LoadBalancer(): m_last_time(time(nullptr)), m_est_arr_time(c_init_est_arr_time) {
    m_listener_socket.Bind(c_hosts_iface_addr,c_port);
    m_listener_socket.Listen(c_backlog);
    for (int i = 0; i < c_num_servers; ++i) {
        m_servers[i].socket.Connect(c_server_addrs[i], c_port);
    }
    init_servers_types();
}

void LoadBalancer::run() {
    array<thread, 2 + c_num_servers*2> threads;
    threads[0] = thread(&LoadBalancer::listen_clients, this);
    threads[1] = thread(&LoadBalancer::calc_dests, this);
    for (int i = 0; i < c_num_servers; ++i) {
        threads[2*i+2] = thread(&LoadBalancer::send_server, this, i);
        threads[2*i+3] = thread(&LoadBalancer::recv_server, this, i);
    }
    for (thread& t:threads) {
        t.join();
    }
    assert(false);
}

LoadBalancer::Request::Request(const string& msg):time(msg[1] - '0') {
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

LoadBalancer::ServerDescriptor::ServerDescriptor():
remaining_time(0), type(ServerType::NONE)
{}

void LoadBalancer::init_servers_types() {
    m_servers[0].type = ServerType::VIDEO;
    m_servers[1].type = ServerType::VIDEO;
    m_servers[2].type = ServerType::MUSIC;
}

void LoadBalancer::listen_clients() {
    while (true) {
        SocketWrapper response_sock = m_listener_socket.Accept();
        string request_msg = response_sock.Recv(c_message_length);
        m_calc_queue.push(request_msg, response_sock);
    }
}

void LoadBalancer::calc_dests() {
    while (true) {
        ServerQueue::QueueItem query = m_calc_queue.pop();
        int dest_server = get_dest(Request(query.msg));
        m_servers[dest_server].requests_queue.push(query.msg, query.response_sock);
    }
}

int LoadBalancer::get_dest(const Request& req) {
    update_servers_times();
    array<int, c_num_servers> goodness = get_goodness_list(req);
    int server_pick = distance(goodness.begin(), max_element(goodness.begin(), goodness.end()));
    m_servers[server_pick].remaining_time += get_process_cost(server_pick, req);
    return server_pick;
}

void LoadBalancer::update_servers_times() {
    time_t current_time = time(nullptr);
    time_t delta = current_time - m_last_time;
    m_est_arr_time = c_arr_time_bias*m_est_arr_time + (1-c_arr_time_bias)*delta;
    for (int i = 0; i < c_num_servers; ++i) {
        m_servers[i].remaining_time -= delta;
        if (m_servers[i].remaining_time < 0) {
            m_servers[i].remaining_time = 0;
        }
    }
    m_last_time = current_time;
}

array<int, LoadBalancer::c_num_servers> LoadBalancer::get_goodness_list(const Request& req) const {
    array<int, c_num_servers> goodness;
    for (int i = 0; i < c_num_servers; ++i) {
        goodness[i] = get_goodness(i, req);
    }
    return goodness;
}

int LoadBalancer::get_goodness(int server_index, const Request& req) const {
    int remaining_time = m_servers[server_index].remaining_time;
    int proc_cost = get_process_cost(server_index, req);
    int purity_thres = c_purity_factor * m_est_arr_time;
    int goodness = -(remaining_time + proc_cost);
    if (req.time != proc_cost && proc_cost > purity_thres) {
        goodness -= c_purity_purge;
    }
    return goodness;
}

int LoadBalancer::get_process_cost(int server_index, const Request& req) const {
    int factor = 0;
    switch (m_servers[server_index].type)
    {
    case ServerType::MUSIC:
        switch (req.type)
        {
        case RequestType::MUSIC:
            factor = 1;
            break;
        case RequestType::PICTURE:
            factor = 2;
            break;
        case RequestType::VIDEO:
            factor = 3;
            break;
        default:
            assert(false);
        }
        break;
    case ServerType::VIDEO:
        switch (req.type)
        {
        case RequestType::MUSIC:
            factor = 2;
            break;
        case RequestType::PICTURE:
            factor = 1;
            break;
        case RequestType::VIDEO:
            factor = 1;
            break;
        default:
            assert(false);
        }
        break;
    default:
        assert(false);
    }
    return factor*req.time;
}

void LoadBalancer::send_server(int server_index) {
    while (true) {
        ServerQueue::QueueItem query = m_servers[server_index].requests_queue.pop();
        m_servers[server_index].socket.Send(query.msg);
        m_servers[server_index].feedback_queue.push(query.msg, query.response_sock);
    }
}

void LoadBalancer::recv_server(int server_index) {
    while (true) {
        string response = m_servers[server_index].socket.Recv(c_message_length);
        ServerQueue::QueueItem query = m_servers[server_index].feedback_queue.pop();
        query.response_sock.Send(response);
    }
}