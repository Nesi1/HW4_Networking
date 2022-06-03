#ifndef SOCKET_WRAPPER_HPP  
#define SOCKET_WRAPPER_HPP

#include <string>

class SocketWrapper
{

public:

    SocketWrapper();
    SocketWrapper(const SocketWrapper& other);
    ~SocketWrapper();

    SocketWrapper& operator=(const SocketWrapper& other);

    void Bind(const std::string& iface_addr, uint16_t port);
    void Listen(int backlog);
    void Connect(const std::string& addr, uint16_t port);
    SocketWrapper Accept();
    std::string Recv(ssize_t len, int flags);
    void Send(const std::string& msg, int flags);

private:

    int m_sock_fd;
    int* m_ref_count;

    SocketWrapper(int sock_fd);

};

#endif // SOCKET_WRAPPER_HPP
