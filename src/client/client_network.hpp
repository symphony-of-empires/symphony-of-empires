#pragma once

#include <mutex>
#include <deque>
#include <thread>
#include <atomic>

#include "unified_render/network.hpp"
#include "client/game_state.hpp"

class ClientException : public std::exception {
    std::string buffer;
public:
    ClientException(const std::string& msg) {
        buffer = msg;
    }
    virtual const char* what(void) const noexcept {
        return buffer.c_str();
    }
};

class Client : public UnifiedRender::Networking::Client {
    std::thread net_thread;
    std::atomic<bool> has_snapshot;
    GameState& gs;
public:
    Client(GameState& gs, std::string host, const unsigned port);
    ~Client();
    void net_loop(void);
    void wait_for_snapshot(void);
};

extern Client* g_client;