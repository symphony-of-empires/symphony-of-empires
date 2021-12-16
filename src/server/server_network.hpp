#pragma once

#include <deque>
#include <mutex>
#include <atomic>
#include <vector>
#include <thread>
#include "unified_render/network.hpp"

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
	
	std::deque<Packet> pending_packets;
	std::mutex pending_packets_mutex;
	
    std::deque<Packet> packets;
	std::mutex packets_mutex;
	
    std::string username;
};

class GameState;
class Server {
    struct sockaddr_in addr;
#ifdef unix
    int fd;
#elif defined windows
    SOCKET fd;
#endif

    std::atomic<bool> run;
    GameState& gs;
public:
    ServerClient* clients;

    Server(GameState& gs, unsigned port = 1825, unsigned max_conn = 16);
    ~Server();

    void broadcast(const Packet& packet);
    void net_loop(int id);

    int n_clients;
    int player_count = 0;
};

extern Server* g_server;