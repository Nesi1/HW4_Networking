#ifndef SERVER_QUEUE_HPP  
#define SERVER_QUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>

class ServerQueue
{

public:

    struct QueueItem {
        std::string msg;
        SocketWrapper& response_sock;
    };

    void push(const std::string& msg, const SocketWrapper& sock);
    QueueItem pop();

private:

    std::condition_variable m_cond_var;
    std::mutex m_mutex;
    std::queue<QueueItem> m_queue;

};

#endif // SERVER_QUEUE_HPP