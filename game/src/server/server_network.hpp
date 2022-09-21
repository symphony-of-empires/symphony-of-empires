// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

#include "eng3d/network.hpp"

class ServerException : public std::exception {
    std::string buffer;
public:
    ServerException(const std::string& msg) {
        buffer = msg;
    }
    virtual const char* what() const noexcept {
        return buffer.c_str();
    }
};

class GameState;
class Nation;
class Server : public Eng3D::Networking::Server {
    GameState& gs;
public:
    Server(GameState& gs, unsigned port = 1825, unsigned max_conn = 4);
    ~Server() = default;
    void net_loop(int id);
    std::vector<Nation*> clients_extra_data;
};

extern Server* g_server;
