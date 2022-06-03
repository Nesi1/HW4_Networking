#include "socket_wrapper.hpp"

#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

SocketWrapper::SocketWrapper() {
    m_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock_fd == -1) {
        syscall_error_handler("socket");
    }
}

SocketWrapper::~SocketWrapper() {
    close(m_sock_fd);
}