#include "load_balancer.hpp"

#include <string>

using namespace std;

void LoadBalancer::listen_clients() {
    while (true) {
        SocketWrapper temp_sock = m_listener_socket.accept();
        string request_msg = temp_sock.recv(/*TODO: fill arguments*/);
        int dest_server = get_dest(Request(request_msg));
        m_servers[dest_server].requests_queue.push(request_msg, temp_sock);
    }
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