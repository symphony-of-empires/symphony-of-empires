// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
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
//      network.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <exception>
#include <vector>
#include <string>
#include <cstring>
#include <atomic>
#include <thread>
#include <mutex>
#include <deque>
#include <stdexcept>
#include <functional>
#include <tbb/concurrent_queue.h>

#ifdef _WIN32
// Allow us to use deprecated functions like inet_addr
#   define _WINSOCK_DEPRECATED_NO_WARNINGS
// MingW heavily dislikes ws2def.h and causes spurious errors
//#   ifndef __MINGW32__
//#       include <ws2def.h>
//#   endif
#   ifndef WINSOCK2_IMPORTED
#       define WINSOCK2_IMPORTED
#       include <winsock2.h>
#   endif
#elif defined __unix__
#    define _XOPEN_SOURCE_EXTENDED 1
#    include <netdb.h>
#    include <arpa/inet.h>
#endif

// Visual Studio does not know about UNISTD.H, Mingw does through
#ifndef _MSC_VER
#    include <unistd.h>
#endif

#include "eng3d/utils.hpp"

namespace Eng3D::Deser {
    struct Archive;
}

namespace Eng3D::Networking {
    class SocketException : public std::exception {
        std::string buffer;
    public:
        SocketException(const std::string_view msg) {
            buffer = msg;
        }
        virtual const char* what() const noexcept {
            return buffer.data();
        }
    };

    class SocketStream {
        bool is_server_stream = false;
    public:
        SocketStream() = default;
        SocketStream(int _fd) : fd(_fd) {}
        ~SocketStream() = default;
        bool send(const void* data, size_t size, std::function<bool()> pred);
        bool recv(void* data, size_t size, std::function<bool()> pred = 0);
        void set_timeout(int seconds);
        bool has_pending();
        void set_blocking(bool value);

        int fd;
    };

    enum class PacketCode {
        OK,
        PACKET_ERROR,
    };

    class Packet {
        size_t n_data = 0;
        PacketCode code = PacketCode::OK;
    public:
        Packet() = default;
        Packet(int _fd) {
            stream = Eng3D::Networking::SocketStream(_fd);
        }
        Packet(int _fd, const void* buf, size_t size) {
            stream = Eng3D::Networking::SocketStream(_fd);
            this->data(buf, size);
        }
        ~Packet() = default;

        void* data() noexcept {
            return static_cast<void*>(&buffer[0]);
        }

        template<typename T>
        void data(const T* buf = nullptr, size_t size = sizeof(T)) noexcept {
            n_data = size;
            buffer.resize(n_data);
            if(buf != nullptr)
                std::memcpy(&buffer[0], buf, size);
        }

        size_t size() const noexcept {
            return n_data;
        }

        bool is_ok() const noexcept {
            return code == PacketCode::OK;
        }

        bool send();
        bool recv();

        std::vector<uint8_t> buffer;
        SocketStream stream;
        std::function<bool()> pred;
    };

    class ServerClient {
        int conn_fd = 0;
    public:
        ServerClient() = default;
        ~ServerClient();

        int try_connect(int fd);
        void flush_packets();
        bool has_pending();

        std::atomic<bool> is_connected;
        tbb::concurrent_bounded_queue<Eng3D::Networking::Packet> packets;
        std::string username;
        std::unique_ptr<std::thread> thread;
    };

    class Server {
    protected:
        struct sockaddr_in addr;
        int fd;
        std::atomic<bool> run;
    public:
        class Exception : public std::exception {
            std::string msg;
        public:
            Exception(const std::string_view _msg)
                : msg{ _msg }
            {

            }
            virtual const char* what() const noexcept {
                return msg.c_str();
            }
        };

        Server(unsigned port, unsigned max_conn);
        virtual ~Server();
        void broadcast(const Eng3D::Networking::Packet& packet);
        void do_netloop(std::function<void(int i)> on_wake_thread, int id);

        virtual void on_connect(int conn_fd, int id) = 0;
        virtual void on_disconnect() = 0;
        virtual void handler(const Packet& packet, Eng3D::Deser::Archive& ar, int id) = 0;

        ServerClient* clients;
        std::size_t n_clients;
        std::size_t player_count = 0;
    };

    class Client {
    protected:
        struct sockaddr_in addr;
        int fd;
    public:
        class Exception : public std::exception {
            std::string msg;
        public:
            Exception(const std::string_view _msg)
                : msg{ _msg }
            {

            }
            virtual const char* what() const noexcept {
                return msg.c_str();
            }
        };

        Client(std::string host, const unsigned port);
        ~Client();
        void do_netloop(std::function<bool()> cond, std::function<void(const Packet& packet, Eng3D::Deser::Archive& ar)> handler);

        inline void send(const Eng3D::Networking::Packet& packet) {
            packets.push(packet);
        }

        inline int get_fd() const {
            return fd;
        }

        tbb::concurrent_bounded_queue<Eng3D::Networking::Packet> packets;
        std::string username;
    };
}
