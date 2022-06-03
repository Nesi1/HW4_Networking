#ifndef SOCKET_WRAPPER_HPP  
#define SOCKET_WRAPPER_HPP

class SocketWrapper
{

public:

    SocketWrapper();
    SocketWrapper(const SocketWrapper& other) = delete;
    ~SocketWrapper();

    void Bind(const string& ipv4_iface_addr, uint16_t port);
    void Listen(int backlog);
    void Connect(const string& ipv4_addr, uint16_t port);
    SocketWrapper Accept();
    void Recv();
    void Send();

private:

    int m_sock_fd;

};

#endif // SOCKET_WRAPPER_HPP