#include "server_queue.h"

#include <mutex>

using namespace std;

void ServerQueue::push(const string& msg, const SocketWrapper& sock) {
    lock_guard<mutex> lock(m_mutex);
    m_queue.push(QueueItem{msg, sock});
    m_cond_var.notify_one();
}

ServerQueue::QueueItem ServerQueue::pop() {
    lock_guard lock(m_mutex);
    while (m_queue.empty()) {
        m_cond_var.wait(lock);
    }
    QueueItem out = m_queue.front();
    m_queue.pop();
    return out;
}