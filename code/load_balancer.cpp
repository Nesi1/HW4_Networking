#include "load_balancer.hpp"

#include <string>

void LoadBalancer::listen_clients() {
    while (true) {
        int temp_fd = m_listener_socket.accept();
        std::string request_msg = temp_fd.recv(/*TODO*/);
        int dest_server = get_dest(Request(request_msg));
        m_servers[dest_server].requests_queue.push(request_msg, temp_fd);
    }
}

LoadBalancer::Request::Request(std::string msg):time(msg[1] - '0') {
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