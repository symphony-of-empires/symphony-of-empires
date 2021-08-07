#ifndef NETWORK_SERVER_HPP
#define NETWORK_SERVER_HPP

#include <deque>
#include <mutex>
#include <atomic>
#include <vector>
#include <thread>
#include "../network.hpp"

class Server {
    struct sockaddr_in addr;
#ifdef unix
    int fd;
#elif defined windows
    SOCKET fd;
#endif

    std::vector<std::thread> threads;
    std::vector<std::deque<Packet>> packet_queues;
    std::atomic<bool>* is_connected;
    
    // std::vector hates mutexes because they are not copy-able
    std::mutex* packet_mutexes;
    
    std::atomic<bool> run;
public:
    Server(unsigned port = 1825, unsigned max_conn = 16);
    ~Server();
    
    void broadcast(Packet& packet);
    void net_loop(int id);
    
    int n_clients;
};
extern Server* g_server;

#endif