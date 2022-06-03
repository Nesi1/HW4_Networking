#ifndef SOCKET_WRAPPER_HPP  
#define SOCKET_WRAPPER_HPP

class SocketWrapper
{

public:

    SocketWrapper();
    SocketWrapper(const SocketWrapper& other) = delete;
    ~SocketWrapper();

private:

    int m_sock_fd;

};

#endif // SOCKET_WRAPPER_HPP