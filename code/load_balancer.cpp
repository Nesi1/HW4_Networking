#include "load_balancer.hpp"

#include <string>

void load_balancer::listen_clients() {
    while (true) {
        int temp_fd = listener_socket.accept();
        std::string request_msg = listener_socket.recv(/*TODO*/);
        int dest_server = get_dest(Request(request_msg));
        servers[dest_server].requests_queue.enqueue(request_msg, temp_fd);
    }
}