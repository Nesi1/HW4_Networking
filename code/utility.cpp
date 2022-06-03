#include "utility.hpp"

#include <string>
#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

sockaddr_in make_sockaddr(const std::string& addr, uint16_t port) {
    sockaddr_in out;
    out.sin_family = AF_INET;
    out.sin_port = htons(port);
    inet_aton(addr.c_str(), &out.sin_addr);
    return out;
}

void handle_sys_error(const string& sycall_name) {
    cout << "SYSCALL \'" << sycall_name << "\' FAILED, ABORTING" << endl;
    exit(1);
}
