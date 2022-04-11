// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      server/server_network.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

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