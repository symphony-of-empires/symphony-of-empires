#pragma once

#include <deque>
#include <mutex>
#include <atomic>
#include <vector>
#include <thread>

#ifdef windows
#include <winsock2.h>
#endif

#include "unified_render/network.hpp"

class ServerException : public std::exception {
    std::string buffer;
public:
    ServerException(const std::string& msg) {
        buffer = msg;
    }
    virtual const char* what(void) const noexcept {
        return buffer.c_str();
    }
};

class GameState;
class Server : public UnifiedRender::Networking::Server {
    GameState& gs;
public:
    Server(GameState& gs, unsigned port = 1825, unsigned max_conn = 16);
    ~Server(void);
    void net_loop(int id);
};

extern Server* g_server;