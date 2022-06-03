#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>

#include <netinet/in.h>

#define PORT 80
#define HOSTS_INTERFACE_ATTR "10.0.0.1"
#define MESSAGE_LENGTH 2

sockaddr_in make_sockaddr(const std::string& addr, uint16_t port);

void handle_sys_error(const std::string& error_msg);

#endif // UTILITY_HPP