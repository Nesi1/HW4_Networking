#include "server_queue.hpp"
#include "socket_wrapper.hpp"

#include <mutex>

using namespace std;

ServerQueue::QueueItem::QueueItem(const std::string& msg, const SocketWrapper& response_sock):
msg(msg), response_sock(response_sock)
{}

void ServerQueue::push(const string& msg, const SocketWrapper& sock) {
    lock_guard<mutex> lock(m_mutex);
    m_queue.push(QueueItem(msg, sock));
    m_cond_var.notify_one();
}

ServerQueue::QueueItem ServerQueue::pop() {
    unique_lock<mutex> lock(m_mutex);
    while (m_queue.empty()) {
        m_cond_var.wait(lock);
    }
    QueueItem out = m_queue.front();
    m_queue.pop();
    return out;
}