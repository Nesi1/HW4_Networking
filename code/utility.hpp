#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>

#include <netinet/in.h>

sockaddr_in make_sockaddr(const std::string& addr, uint16_t port);

void handle_sys_error(const std::string& error_msg);

#endif // UTILITY_HPP
