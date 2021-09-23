#ifndef NETWORK_SERVER_HPP
#define NETWORK_SERVER_HPP

#include <deque>
#include <mutex>
#include <atomic>
#include <vector>
#include <thread>
#include "../network.hpp"

#ifdef windows
#include <winsock2.h>
#endif

class ServerClient {
public:
    ServerClient() {};
    ~ServerClient(void) {
        thread.join();
    };

    std::thread thread;
    std::atomic<bool> is_connected;

    std::deque<Packet> packets;
    std::mutex packets_mutex;

    std::string username;
};

class Server {
    struct sockaddr_in addr;
#ifdef unix
    int fd;
#elif defined windows
    SOCKET fd;
#endif

    std::atomic<bool> run;
public:
    ServerClient* clients;

    Server(unsigned port = 1825, unsigned max_conn = 16);
    ~Server();
    
    void broadcast(Packet& packet);
    void net_loop(int id);
    
    int n_clients;
};
extern Server* g_server;

#endif