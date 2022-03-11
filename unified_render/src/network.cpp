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
//      network.cpp
//
// Abstract:
//      Server and Client networking functions, along with primitive ones such
//      as recv and send. This abstracts away all the "OS-dependant" parts of
//      the networking code for the game.
// ----------------------------------------------------------------------------

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <mutex>

// Visual Studio does not know about UNISTD.H, Mingw does through
#ifndef _MSC_VER
#	include <unistd.h>
#endif
#ifdef unix
#	define _XOPEN_SOURCE_EXTENDED 1
#	include <sys/socket.h>
#	include <netinet/in.h>
#	ifndef INVALID_SOCKET
#		define INVALID_SOCKET -1
#	endif
#	include <netdb.h>
#	include <arpa/inet.h>
#	include <poll.h>
#   include <signal.h>
#   include <fcntl.h>
#elif defined windows
#	define WIN32_LEAN_AND_MEAN 1
#   ifndef NOMINMAX
#	    define NOMINMAX 1
#   endif
#	include <windows.h>
#	define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#	include <winsock2.h>
#	include <ws2tcpip.h>
#endif

#include <sys/types.h>

#include "unified_render/network.hpp"
#include "unified_render/log.hpp"

//
// Packet
//
UnifiedRender::Networking::Packet::Packet() {

}

UnifiedRender::Networking::Packet::Packet(int _fd) {
    stream = UnifiedRender::Networking::SocketStream(_fd);
}

UnifiedRender::Networking::Packet::~Packet(void) {

}

void* UnifiedRender::Networking::Packet::data(void) {
    return (void*)&buffer[0];
}

void UnifiedRender::Networking::Packet::data(void* buf, size_t size) {
    n_data = size;
    buffer.resize(n_data);
    std::memcpy(&buffer[0], buf, size);
}

size_t UnifiedRender::Networking::Packet::size(void) const {
    return n_data;
}

bool UnifiedRender::Networking::Packet::is_ok(void) const {
    return (code == PacketCode::OK);
}

//
// Socket stream
//
UnifiedRender::Networking::SocketStream::SocketStream(void) {

}

UnifiedRender::Networking::SocketStream::SocketStream(int _fd)
    : fd(_fd)
{

}

UnifiedRender::Networking::SocketStream::~SocketStream(void) {

}

void UnifiedRender::Networking::SocketStream::send(const void* data, size_t size) {
    const char* c_data = (const char*)data;
    for(size_t i = 0; i < size; ) {
        int r = ::send(fd, &c_data[i], std::min<size_t>(1024, size - i), 0);
        if(r < 0) {
            throw UnifiedRender::Networking::SocketException("Can't send data of packet");
        }
        
        i += (size_t)r;
    }
}

void UnifiedRender::Networking::SocketStream::recv(void* data, size_t size) {
    char* c_data = (char*)data;
    for(size_t i = 0; i < size; ) {
        int r = ::recv(fd, &c_data[i], std::min<size_t>(1024, size - i), MSG_WAITALL);
        if(r < 0) {
            throw UnifiedRender::Networking::SocketException("Can't receive data of packet");
        }

        i += (size_t)r;
    }
}

//
// Server client
//
UnifiedRender::Networking::ServerClient::ServerClient(void) {
    is_active = false;
}

int UnifiedRender::Networking::ServerClient::try_connect(int fd) {
    sockaddr_in client;
    socklen_t len = sizeof(client);
    try {
        conn_fd = accept(fd, (sockaddr*)&client, &len);
        if(conn_fd == INVALID_SOCKET) {
            throw UnifiedRender::Networking::SocketException("Cannot accept client connection");
        }

        // At this point the client's connection was accepted - so we only have to check
        // Then we check if the server is running and we throw accordingly
        is_connected = true;

        UnifiedRender::Log::debug("server", "New client connection established");
        return conn_fd;
    } catch(UnifiedRender::Networking::SocketException& e) {
        // Continue
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}

// Push pending_packets to the packets queue (the packets queue is managed
// by us and requires almost 0 locking, so the host does not stagnate when
// trying to send packets to a certain client)
void UnifiedRender::Networking::ServerClient::flush_packets(void) {
    if(!pending_packets.empty()) {
        if(pending_packets_mutex.try_lock()) {
            std::lock_guard<std::mutex> lock(packets_mutex);

            std::deque<UnifiedRender::Networking::Packet>::iterator packet;
            for(packet = pending_packets.begin(); packet != pending_packets.end(); packet++) {
                packets.push_back(*packet);
            }
            pending_packets.clear();
            pending_packets_mutex.unlock();
        }
    }
}

bool UnifiedRender::Networking::ServerClient::has_pending(void) {
    // Check if we need to read packets
#ifdef unix
    struct pollfd pfd = {};
    pfd.fd = conn_fd;
    pfd.events = POLLIN;
    
    int has_pending = poll(&pfd, 1, 10);
    if(pfd.revents & POLLIN || has_pending) {
        return true;
    }
#elif defined windows
    u_long has_pending = 0;
    int test = ioctlsocket(conn_fd, FIONREAD, &has_pending);
    if(has_pending) {
        return true;
    }
#endif
    return false;
}

UnifiedRender::Networking::ServerClient::~ServerClient(void) {
    thread.join();
}

//
// Server
//
UnifiedRender::Networking::Server::Server(const unsigned port, const unsigned max_conn)
    : n_clients{ static_cast<int>(max_conn) }
{
#ifdef windows
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
#endif

    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(fd == INVALID_SOCKET) {
        throw UnifiedRender::Networking::SocketException("Cannot create server socket");
    }

#ifdef unix
    int enable = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
#endif
    if(bind(fd, (sockaddr*)&addr, sizeof(addr)) != 0) {
        throw UnifiedRender::Networking::SocketException("Cannot bind server");
    }

    if(listen(fd, max_conn) != 0) {
        throw UnifiedRender::Networking::SocketException("Cannot listen in specified number of concurrent connections");
    }

#ifdef unix
    // Allow non-blocking operations on this socket (we don't want to block on multi-listener servers)
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);

    // We need to ignore pipe signals since any client disconnecting **will** kill the server
    signal(SIGPIPE, SIG_IGN);
#endif

    run = true;
    UnifiedRender::Log::debug("serber", "Server created sucessfully and listening to " + std::to_string(port) + "; now invite people!");
}

UnifiedRender::Networking::Server::~Server() {
    run = false;
#ifdef unix
    close(fd);
#elif defined windows
    closesocket(fd);
    WSACleanup();
#endif
    delete[] clients;
}

// This will broadcast the given packet to all clients currently on the server
void UnifiedRender::Networking::Server::broadcast(const UnifiedRender::Networking::Packet& packet) {
    for(size_t i = 0; i < n_clients; i++) {
        if(clients[i].is_connected == true) {
            // If we can "acquire" the spinlock to the main packet queue we will push
            // our packet there, otherwise we take the alternative packet queue to minimize
            // locking between server and client
            if(clients[i].packets_mutex.try_lock()) {
                clients[i].packets.push_back(packet);
                clients[i].packets_mutex.unlock();
            } else {
                std::lock_guard<std::mutex> lock(clients[i].pending_packets_mutex);
                clients[i].pending_packets.push_back(packet);
            }

            // Disconnect the client when more than 200 MB is used
            // we can't save your packets buddy - other clients need their stuff too!
            size_t total_size = 0;

            std::deque<UnifiedRender::Networking::Packet>::const_iterator packet_q;
            for(packet_q = clients[i].pending_packets.cbegin(); packet_q != clients[i].pending_packets.cend(); packet_q++) {
                total_size += (*packet_q).buffer.size();
            }

            if(total_size >= 200 * 1000000) {
                clients[i].is_connected = false;
                UnifiedRender::Log::debug("server", "Client#" + std::to_string(i) + " has exceeded max quota! - It has used " + std::to_string(total_size) + "B!");
            }
        }
    }
}

//
// Client
//
UnifiedRender::Networking::Client::Client(std::string host, const unsigned port) {
    // Initialize WSA
#ifdef windows
    WSADATA data;
    if(WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        print_error("WSA code: %u", WSAGetLastError());
        throw UnifiedRender::Networking::SocketException("Can't start WSA subsystem");
    }
#endif
    
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host.c_str());
    addr.sin_port = htons(port);
    
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(fd == INVALID_SOCKET) {
#ifdef windows
        print_error("WSA Code: %u", WSAGetLastError());
        WSACleanup();
#endif
        throw UnifiedRender::Networking::SocketException("Can't create client socket");
    }
    
    if(connect(fd, (sockaddr*)&addr, sizeof(addr)) != 0) {
#ifdef unix
        close(fd);
#elif defined windows
        print_error("WSA Code: %u", WSAGetLastError());
        closesocket(fd);
#endif
        throw UnifiedRender::Networking::SocketException("Can't connect to server");
    }
}

int UnifiedRender::Networking::Client::get_fd(void) const {
    return fd;
}

void UnifiedRender::Networking::Client::send(const UnifiedRender::Networking::Packet& packet) {
    if(packets_mutex.try_lock()) {
        packets.push_back(packet);
        packets_mutex.unlock();
    } else {
        std::lock_guard<std::mutex> lock(pending_packets_mutex);
        pending_packets.push_back(packet);
    }
}

UnifiedRender::Networking::Client::~Client() {
#ifdef windows
    closesocket(fd);
    WSACleanup();
#else
    close(fd);
#endif
}

