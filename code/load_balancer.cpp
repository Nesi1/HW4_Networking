#include "load_balancer.hpp"

#include "utility.hpp"

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
        SocketWrapper temp_sock = m_listener_socket.Accept();
        string request_msg = temp_sock.Recv(MESSAGE_LENGTH);
        m_calc_queue.push(request_msg, temp_sock);
    }
}

void LoadBalancer::calc_dests() {
    while (true) {
        ServerQueue::QueueItem query = m_calc_queue.pop();
        int dest_server = get_dest(Request(query.msg));
        m_servers[dest_server].requests_queue.push(query.msg, query.response_sock);
    }
}