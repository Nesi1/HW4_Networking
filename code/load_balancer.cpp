#include "load_balancer.hpp"

#include "utility.hpp"

#include <algorithm>
#include <cassert>
#include <string>
#include <thread>

using namespace std;

const int c_port = 80;
const int c_message_length = 2;
const int c_backlog = 64;

const string c_hosts_iface_addr = "10.0.0.1";
const array<string, LoadBalancer::c_num_servers> c_server_addrs{
    "192.168.0.101",
    "192.168.0.102",
    "192.168.0.103"
};

LoadBalancer::LoadBalancer() {
    m_listener_socket.Bind(c_hosts_iface_addr,c_port);
    m_listener_socket.Listen(c_backlog);
    for (int i = 0; i < c_num_servers; ++i) {
        m_servers[i].socket.Connect(c_server_addrs[i], c_port);
    }
}

void LoadBalancer::run() {
    std::array<thread, 1 + c_num_servers*2> t;
    t[0] = thread(&LoadBalancer::calc_dests, this);
    for (int i = 0; i < c_num_servers; ++i) {
        t[2*i+1] = thread(&LoadBalancer::send_server, this, i);
        t[2*i+2] = thread(&LoadBalancer::recv_server, this, i);
    }
    listen_clients();
    assert(false);
}

LoadBalancer::Request::Request(string msg):time(msg[1] - '0') {
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

void LoadBalancer::listen_clients() {
    while (true) {
        SocketWrapper response_sock = m_listener_socket.Accept();
        string request_msg = response_sock.Recv(c_message_length, 0);
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

int LoadBalancer::get_dest(Request req) {
    array<int, c_num_servers> goodness = get_goodness(req);
    return distance(goodness.begin(), max_element(goodness.begin(), goodness.end()));
}

array<int, LoadBalancer::c_num_servers> LoadBalancer::get_goodness(Request req) {
    return array<int, c_num_servers>{1,2,3}; // TODO: implement the scheduling algorithm
}

void LoadBalancer::send_server(int server_index) {
    while (true) {
        ServerQueue::QueueItem query = m_servers[server_index].requests_queue.pop();
        m_servers[server_index].socket.Send(query.msg, 0);
        m_servers[server_index].feedback_queue.push(query.msg, query.response_sock);
    }
}

void LoadBalancer::recv_server(int server_index) {
    while (true) {
        string response = m_servers[server_index].socket.Recv(c_message_length, 0);
        ServerQueue::QueueItem query = m_servers[server_index].feedback_queue.pop();
        query.response_sock.Send(response, 0);
    }
}