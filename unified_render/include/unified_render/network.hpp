// Unified Render - General purpouse game engine
// Copyright (C) 2021, Unified Render contributors
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

#if defined windows
// Allow us to use deprecated functions like inet_addr
#   define _WINSOCK_DEPRECATED_NO_WARNINGS
// MingW heavily dislikes ws2def.h and causes spurious errors
//#   ifndef __MINGW32__
//#       include <ws2def.h>
//#   endif
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   pragma comment(lib, "Ws2_32.lib")
#endif

#ifdef unix
#    define _XOPEN_SOURCE_EXTENDED 1
#    include <netdb.h>
#    include <arpa/inet.h>
#endif
#include <sys/types.h>

// Visual Studio does not know about UNISTD.H, Mingw does through
#ifndef _MSC_VER
#    include <unistd.h>
#endif

namespace UnifiedRender::Networking {
    class SocketException : public std::exception {
        std::string buffer;
    public:
        SocketException(const std::string& msg) {
            buffer = msg;
        }
        virtual const char* what(void) const noexcept {
            return buffer.c_str();
        }
    };

    class SocketStream {
        bool is_server_stream = false;
    public:
        SocketStream(void);
        SocketStream(int _fd);
        ~SocketStream(void);
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
        Packet();
        Packet(int _fd);
        ~Packet();

        void* data(void);
        void data(void* buf, size_t size);
        size_t size(void) const;

        bool is_ok(void) const;

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

        inline void send(void) {
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
            if(ntohs(eof_marker) != 0xE0F) {
                throw UnifiedRender::Networking::SocketException("Packet with invalid EOF");
            }
        }

        void recv(void) {
            this->recv<void>(nullptr);
        }

        std::vector<uint8_t> buffer;
        SocketStream stream;
    };

    class ServerClient {
        int conn_fd = 0;
    public:
        ServerClient();
        ~ServerClient();
        int try_connect(int fd);
        void flush_packets(void);
        bool has_pending(void);

        std::thread thread;
        std::atomic<bool> is_connected;
        
        std::deque<UnifiedRender::Networking::Packet> pending_packets;
        std::mutex pending_packets_mutex;
        
        std::deque<UnifiedRender::Networking::Packet> packets;
        std::mutex packets_mutex;
        
        std::string username;
    };

    class Server {
    protected:
        struct sockaddr_in addr;
        int fd;
        std::atomic<bool> run;
    public:
        Server(unsigned port, unsigned max_conn);
        virtual ~Server(void);
        void broadcast(const UnifiedRender::Networking::Packet& packet);

        ServerClient* clients;
        int n_clients;
        int player_count = 0;
    };

    class Client {
    protected:
        struct sockaddr_in addr;
        int fd;
    public:
        Client(std::string host, const unsigned port);
        ~Client();
        int get_fd(void) const;
        void send(const UnifiedRender::Networking::Packet& packet);
        
        std::deque<UnifiedRender::Networking::Packet> packets;
        std::mutex packets_mutex;
        
        std::deque<UnifiedRender::Networking::Packet> pending_packets;
        std::mutex pending_packets_mutex;

        std::string username;
    };
};