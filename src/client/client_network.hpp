#pragma once

#include <mutex>
#include <deque>
#include <thread>
#include <atomic>
#include "../network.hpp"
#include "game_state.hpp"

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
    
    std::mutex packet_mutex;
    std::deque<Packet> packet_queue;
};

extern Client* g_client;