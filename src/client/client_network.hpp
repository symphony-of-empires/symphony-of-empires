#pragma once

#include <mutex>
#include <deque>
#include <thread>
#include <atomic>

#include "unified_render/network.hpp"
#include "client/game_state.hpp"

class Client {
    struct sockaddr_in addr;
#ifdef unix
    int fd;
#elif defined windows
    SOCKET fd;
#endif
    
    std::thread net_thread;
    std::atomic<bool> has_snapshot;
    GameState& gs;
public:
    std::string username;

    Client(GameState& gs, std::string host, const unsigned port);
    ~Client();
    int get_fd(void) {
        return fd;
    }
    
    void net_loop(void);
    void wait_for_snapshot(void);
	
    std::deque<Packet> packets;
	std::mutex packets_mutex;
	
	std::deque<Packet> pending_packets;
	std::mutex pending_packets_mutex;
};

extern Client* g_client;