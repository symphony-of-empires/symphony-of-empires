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
//      client/client_network.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <mutex>
#include <deque>
#include <thread>
#include <atomic>

#include "eng3d/network.hpp"
#include "client/game_state.hpp"

class ClientException : public std::exception {
    std::string buffer;
public:
    ClientException(const std::string_view msg) {
        buffer = msg;
    }
    virtual const char* what() const noexcept {
        return buffer.data();
    }
};

class GameState;
class Client : public Eng3D::Networking::Client {
    std::thread net_thread;
    std::atomic<bool> has_snapshot;
    std::atomic<bool> run;
    GameState& gs;
public:
    Client(GameState& gs, std::string host, const unsigned port);
    ~Client();
    void netloop();
    void wait_for_snapshot();
};

extern Client* g_client;