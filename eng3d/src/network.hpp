// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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

#ifdef E3D_TARGET_WINDOWS
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
#endif

#ifdef E3D_TARGET_UNIX
#    define _XOPEN_SOURCE_EXTENDED 1
#    include <netdb.h>
#    include <arpa/inet.h>
#endif

// Visual Studio does not know about UNISTD.H, Mingw does through
#ifndef _MSC_VER
#    include <unistd.h>
#endif

#include "eng3d/utils.hpp"

namespace Eng3D::Networking {
    class SocketException : public std::exception {
        std::string buffer;
    public:
        SocketException(const std::string& msg) {
            buffer = msg;
        }
        virtual const char* what() const noexcept {
            return buffer.c_str();
        }
    };

    class SocketStream {
        bool is_server_stream = false;
    public:
        SocketStream() = default;
        SocketStream(int _fd) : fd(_fd) {};
        ~SocketStream() = default;
        void send(const void* data, size_t size);
        void recv(void* data, size_t size);
        
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
        ~Packet() = default;

        inline void* data() {
            return static_cast<void*>(&buffer[0]);
        }

        template<typename T>
        inline void data(const T* buf = nullptr, size_t size = sizeof(T)) {
            n_data = size;
            buffer.resize(n_data);
            if(buf != nullptr)
                std::memcpy(&buffer[0], buf, size);
        }

        inline size_t size() const {
            return n_data;
        }

        inline bool is_ok() const {
            return (code == PacketCode::OK);
        }

        template<typename T>
        inline void send(const T* buf = nullptr, size_t size = sizeof(T)) {
            if(buf != nullptr) {
                n_data = size;
                buffer.resize(n_data);
                std::memcpy(&buffer[0], buf, n_data);
            }

            const uint32_t net_code = htonl(static_cast<uint32_t>(code));
            stream.send(&net_code, sizeof(net_code));

            const uint32_t net_size = htonl(n_data);
            stream.send(&net_size, sizeof(net_size));

            //uint8_t* new_buf = new uint8_t[size];
            //size_t new_size = compress(buf, size, new_buf, size);
            //stream.send(&new_buf[0], new_size);
            stream.send(&buffer[0], n_data);
            //delete[] new_buf;

            const uint16_t eof_marker = htons(0xE0F);
            stream.send(&eof_marker, sizeof(eof_marker));
        }

        inline void send() {
            send<void>(nullptr, 0);
        }

        template<typename T>
        inline void recv(T* buf = nullptr) {
            uint32_t net_code;
            stream.recv(&net_code, sizeof(net_code));
            net_code  = ntohl(net_code);
            code = static_cast<PacketCode>(net_code);

            uint32_t net_size;
            stream.recv(&net_size, sizeof(net_size));
            n_data = (size_t)ntohl(net_size);
            buffer.resize(n_data + 1);

            // Reads can only be done 1024 bytes at a time
            stream.recv(&buffer[0], n_data);
            if(buf != nullptr) {
                //size_t new_size = get_compressed_len(n_data);
                //char* new_buf = new char[new_size];
                //size_t decomp_size = decompress(buf, n_data, new_buf, new_size);
                //std::memcpy(buf, &new_buf[0], decomp_size);
                //delete[] new_buf;
                std::memcpy(buf, &buffer[0], n_data);
            }

            uint16_t eof_marker;
            stream.recv(&eof_marker, sizeof(eof_marker));
            if(ntohs(eof_marker) != 0xE0F)
                CXX_THROW(Eng3D::Networking::SocketException, "Packet with invalid EOF");
        }

        inline void recv() {
            this->recv<void>(nullptr);
        }

        std::vector<uint8_t> buffer;
        SocketStream stream;
    };

    class ServerClient {
        int conn_fd = 0;
    public:
        ServerClient()
            : is_active{ false }
        {

        }

        ~ServerClient() {
            thread.join();
        }
        
        int try_connect(int fd);
        void flush_packets();
        bool has_pending();

        bool is_active; // Is our thread currently polling/serving?
        std::atomic<bool> is_connected;
        std::deque<Eng3D::Networking::Packet> pending_packets;
        std::mutex pending_packets_mutex;
        std::deque<Eng3D::Networking::Packet> packets;
        std::mutex packets_mutex;
        std::string username;
        std::thread thread;
    };

    class Server {
    protected:
        struct sockaddr_in addr;
        int fd;
        std::atomic<bool> run;
    public:
        Server(unsigned port, unsigned max_conn);
        virtual ~Server();
        void broadcast(const Eng3D::Networking::Packet& packet);

        ServerClient* clients;
        std::size_t n_clients;
        std::size_t player_count = 0;
    };

    class Client {
    protected:
        struct sockaddr_in addr;
        int fd;
    public:
        Client(std::string host, const unsigned port);
        ~Client();

        inline void send(const Eng3D::Networking::Packet& packet) {
            if(packets_mutex.try_lock()) {
                packets.push_back(packet);
                packets_mutex.unlock();
            } else {
                std::scoped_lock lock(pending_packets_mutex);
                pending_packets.push_back(packet);
            }
        }
        
        inline int get_fd() const {
            return fd;
        }
        
        std::deque<Eng3D::Networking::Packet> packets;
        std::mutex packets_mutex;
        std::deque<Eng3D::Networking::Packet> pending_packets;
        std::mutex pending_packets_mutex;
        std::string username;
    };
};
