#include "socket_wrapper.hpp"

#include "utility.hpp"

#include <array>
#include <iostream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

SocketWrapper::SocketWrapper(): m_ref_count(new int(1)) {
    m_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock_fd == -1) {
        delete m_ref_count;
        handle_sys_error("socket");
    }
}

SocketWrapper::SocketWrapper(const SocketWrapper& other):m_sock_fd(other.m_sock_fd), m_ref_count(other.m_ref_count) {
    ++(*m_ref_count);
}

SocketWrapper::~SocketWrapper() {
    --(*m_ref_count);
    if (m_ref_count == 0) {
        delete m_ref_count;
        close(m_sock_fd);
    }
}

SocketWrapper& SocketWrapper::operator=(const SocketWrapper& other) {
    --(*m_ref_count);
    if (m_ref_count == 0) {
        delete m_ref_count;
        close(m_sock_fd);
    }
    m_sock_fd = other.m_sock_fd;
    m_ref_count = other.m_ref_count;
}

void SocketWrapper::Bind(const string& iface_addr, uint16_t port) {
    sockaddr_in my_addr = make_sockaddr(iface_addr, port);
    if (bind(m_sock_fd, (const sockaddr *) &my_addr, sizeof(my_addr)) == -1) {
        handle_sys_error("bind");
    }
}

void SocketWrapper::Listen(int backlog) {
    if (listen(m_sock_fd, backlog) == -1) {
        handle_sys_error("listen");
    }
}

void SocketWrapper::Connect(const string& addr, uint16_t port) {
    sockaddr_in conn_addr = make_sockaddr(addr, port);
    if (connect(m_sock_fd, (const sockaddr *) &conn_addr, sizeof(conn_addr)) == -1) {
        handle_sys_error("connect");
    }
}

SocketWrapper SocketWrapper::Accept() {
    int out_fd = accept(m_sock_fd, nullptr, 0);
    if (out_fd == -1) {
        handle_sys_error("accept");
    }
    return SocketWrapper(out_fd);
}

string SocketWrapper::Recv(size_t len, int flags) {
    array<char,len> buf;
    ssize_t in_len = recv(m_sock_fd, buf.data(), buf.size(), flags);
    if (in_len != len) {
        handle_sys_error("recv");
    }
    return string(buf.data(), len);
}

void SocketWrapper::Send(const string& msg, int flags) {
    ssize_t out_len = send(m_sock_fd, msg.c_str(), msg.size(), flags);
    if (out_len != msg.size()) {
        handle_sys_error("send");
    }
}

SocketWrapper::SocketWrapper(int sock_fd): m_sock_fd(sock_fd), m_ref_count(new int(1)) {}