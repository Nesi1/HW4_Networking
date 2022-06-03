#include "load_balancer.hpp"

#include "utility.hpp"

#include <algorithm>
#include <string>

using namespace std;

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

void LoadBalancer::listen_clients() {
    while (true) {
        SocketWrapper response_sock = m_listener_socket.Accept();
        string request_msg = response_sock.Recv(MESSAGE_LENGTH, 0);
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
    array<int, NUM_SERVERS> goodness = get_goodness(req);
    return distance(goodness.begin(), max_element(goodness.begin(), goodness.end()));
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
        string response = m_servers[server_index].socket.Recv(MESSAGE_LENGTH, 0);
        ServerQueue::QueueItem query = m_servers[server_index].feedback_queue.pop();
        query.response_sock.Send(response, 0);
    }
}