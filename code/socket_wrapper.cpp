#include "socket_wrapper.hpp"

#include "utility.hpp"

#include <iostream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

SocketWrapper::SocketWrapper() {
    m_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock_fd == -1) {
        handle_sys_error("socket");
    }
}

SocketWrapper::~SocketWrapper() {
    close(m_sock_fd);
}

void SocketWrapper::Bind(const string& ipv4_iface_addr, uint16_t port) {
    sockaddr_in my_addr = make_sockaddr(ipv4_iface_addr, port);
    if (bind(m_sock_fd, (const sockaddr *) &my_addr, sizeof(my_addr)) == -1) {
        handle_sys_error("bind");
    }
}

void SocketWrapper::Listen(int backlog) {
    if (listen(m_sock_fd, backlog) == -1) {
        handle_sys_error("listen");
    }
}

void SocketWrapper::Connect(const string& ipv4_addr, uint16_t port) {
    sockaddr_in conn_addr = make_sockaddr(ipv4_addr, port);
    if (connect(m_sock_fd, (const sockaddr *) &conn_addr, sizeof(conn_addr)) == -1) {
        handle_sys_error("connect");
    }
}