#include "utility.hpp"

#include <string>

using namespace std;

void syscall_error_handler(const string& sycall_name) {
    cout << "SYSCALL \'" << sycall_name << "\' FAILED, ABORTING" << endl;
    exit(1);
}