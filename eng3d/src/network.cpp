// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
#include <numeric>

#include <glm/glm.hpp>
// Visual Studio does not know about UNISTD.H, Mingw does through
#ifndef _MSC_VER
#	include <unistd.h>
#endif
#ifdef E3D_TARGET_UNIX
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
#elif defined E3D_TARGET_WINDOWS
#	define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#   ifndef WINSOCK2_IMPORTED
#       define WINSOCK2_IMPORTED
#       include <winsock2.h>
#       include <ws2tcpip.h>
#   endif
#	include <windows.h>
#endif
#include <sys/types.h>

#include "eng3d/network.hpp"
#include "eng3d/log.hpp"
#include "eng3d/utils.hpp"

//
// Socket stream
//
void Eng3D::Networking::SocketStream::send(const void* data, size_t size, std::function<bool()> pred) {
    const auto* c_data = reinterpret_cast<const char*>(data);
    int times = 1000;
    for(size_t i = 0; i < size; ) {
        if(pred && !pred()) // If (any) predicate fails then return immediately
            return;
        int r = ::send(fd, &c_data[i], glm::min<std::size_t>(1024, size - i), MSG_DONTWAIT);
        if(r < 0) {
            if(!times)
                CXX_THROW(Eng3D::Networking::SocketException, "Packet send interrupted");
            times--;
            continue;
        }
        i += static_cast<std::size_t>(r);
    }
}

void Eng3D::Networking::SocketStream::recv(void* data, size_t size, std::function<bool()> pred) {
    auto* c_data = reinterpret_cast<char*>(data);
    int times = 1000;
    for(size_t i = 0; i < size; ) {
        if(pred && !pred()) // If (any) predicate fails then return immediately
            return;
        int r = ::recv(fd, &c_data[i], glm::min<std::size_t>(1024, size - i), MSG_DONTWAIT);
        if(r < 0) {
            if(!times)
                CXX_THROW(Eng3D::Networking::SocketException, "Packet receive interrupted");
            times--;
            continue;
        }
        i += static_cast<std::size_t>(r);
    }
}

void Eng3D::Networking::SocketStream::set_timeout(int seconds) {
    // See https://stackoverflow.com/questions/2876024/linux-is-there-a-read-or-recv-from-socket-with-timeout
#ifdef E3D_TARGET_UNIX
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    setsockopt(this->fd, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&tv), sizeof tv);
#elif defined E3D_TARGET_WINDOWS
    DWORD timeout = seconds * 1000;
    setsockopt(this->fd, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof timeout);
#endif
}

bool Eng3D::Networking::SocketStream::has_pending() {
    // Check if we need to read packets
#ifdef E3D_TARGET_UNIX
    struct pollfd pfd{};
    pfd.fd = this->fd;
    pfd.events = POLLIN;
    int has_pending = poll(&pfd, 1, 10);
    return (pfd.revents & POLLIN) != 0 || has_pending;
#elif defined E3D_TARGET_WINDOWS
    u_long has_pending = 0;
    int test = ioctlsocket(this->fd, FIONREAD, &has_pending);
    return has_pending;
#endif
}

void Eng3D::Networking::SocketStream::set_blocking(bool blocking) {
#ifdef E3D_TARGET_UNIX
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1) {
        Eng3D::Log::debug("socket_stream", "Can't set socket as non_blocking");
        return;
    }
    flags = blocking ? (flags & (~O_NONBLOCK)) : (flags | O_NONBLOCK);
    fcntl(fd, F_SETFL, flags);
#elif defined E3D_TARGET_WINDOWS
    u_long mode = blocking ? 0 : 1;
    ioctlsocket(fd, FIONBIO, &mode);
#endif
}

//
// Server client
//
int Eng3D::Networking::ServerClient::try_connect(int fd) try {
    sockaddr_in client;
    socklen_t len = sizeof(client);
    conn_fd = accept(fd, reinterpret_cast<sockaddr*>(&client), &len);
    if(conn_fd == INVALID_SOCKET)
        CXX_THROW(Eng3D::Networking::SocketException, "Cannot accept client connection");
    
    // At this point the client's connection was accepted - so we only have to check
    // Then we check if the server is running and we throw accordingly
    is_connected = true;
    Eng3D::Log::debug("server", "New client connection established");
    return conn_fd;
} catch(Eng3D::Networking::SocketException& e) {
    return 0;
}

/// @brief Push pending_packets to the packets queue (the packets queue is managed
/// by us and requires almost 0 locking, so the host does not stagnate when
/// trying to send packets to a certain client)
void Eng3D::Networking::ServerClient::flush_packets() {
    if(!pending_packets.empty()) {
        if(pending_packets_mutex.try_lock()) {
            const std::scoped_lock lock(packets_mutex);
            for(auto& packet : pending_packets) // Put all pending packets into the main queue
                packets.push_back(packet);
            pending_packets.clear();
            pending_packets_mutex.unlock();
        }
    }
}

bool Eng3D::Networking::ServerClient::has_pending() {
    return Eng3D::Networking::SocketStream(conn_fd).has_pending();
}

//
// Server
//
Eng3D::Networking::Server::Server(const unsigned port, const unsigned max_conn)
    : n_clients{ static_cast<std::size_t>(max_conn) }
{
#ifdef E3D_TARGET_WINDOWS
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
#endif

    addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(fd == INVALID_SOCKET)
        CXX_THROW(Eng3D::Networking::SocketException, "Cannot create server socket");
#ifdef E3D_TARGET_UNIX
    int enable = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
#endif
    if(bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
        CXX_THROW(Eng3D::Networking::SocketException, "Cannot bind server");
    if(listen(fd, max_conn) != 0)
        CXX_THROW(Eng3D::Networking::SocketException, "Cannot listen in specified number of concurrent connections");
#ifdef E3D_TARGET_UNIX
    // Allow non-blocking operations on this socket (we don't want to block on multi-listener servers)
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
    // We need to ignore pipe signals since any client disconnecting **will** kill the server
    signal(SIGPIPE, SIG_IGN);
#endif
    this->run = true;
    Eng3D::Log::debug("serber", "Server created sucessfully and listening to " + std::to_string(port) + "; now invite people!");
}

Eng3D::Networking::Server::~Server() {
    this->run = false;
#ifdef E3D_TARGET_UNIX
    close(fd);
#elif defined E3D_TARGET_WINDOWS
    closesocket(fd);
    WSACleanup();
#endif
    // Join all threads before deletion
    for(size_t i = 0; i < this->n_clients; i++)
        if(this->clients[i].thread.joinable())
            this->clients[i].thread.join();
    delete[] this->clients;
}

/// @brief This will broadcast the given packet to all clients currently on the server
void Eng3D::Networking::Server::broadcast(const Eng3D::Networking::Packet& packet) {
    for(size_t i = 0; i < n_clients; i++) {
        if(clients[i].is_connected == true) {
            // If we can "acquire" the spinlock to the main packet queue we will push
            // our packet there, otherwise we take the alternative packet queue to minimize
            // locking between server and client
            if(clients[i].packets_mutex.try_lock()) {
                clients[i].packets.push_back(packet);
                clients[i].packets_mutex.unlock();
            } else {
                const std::scoped_lock lock(clients[i].pending_packets_mutex);
                clients[i].pending_packets.push_back(packet);
            }

            // Disconnect the client when more than 200 MB is used
            // we can't save your packets buddy - other clients need their stuff too!
            const auto total_size = std::accumulate(clients[i].pending_packets.begin(), clients[i].pending_packets.end(), 0, [](const auto a, const auto& b) {
                return a + b.buffer.size();
            });

            if(total_size >= 200 * 1000) {
                clients[i].is_connected = false;
                Eng3D::Log::debug("server", "Client#" + std::to_string(i) + " has exceeded max quota! - It has used " + std::to_string(total_size) + "B!");
            }
        }
    }
}

//
// Client
//
Eng3D::Networking::Client::Client(std::string host, const unsigned port) {
    // Initialize WSA
#ifdef E3D_TARGET_WINDOWS
    WSADATA data;
    if(WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        Eng3D::Log::error("network", "WSA code " + std::to_string(WSAGetLastError()));
        CXX_THROW(Eng3D::Networking::SocketException, "Can't start WSA subsystem");
    }
#endif

    addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host.c_str());
    addr.sin_port = htons(port);

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(fd == INVALID_SOCKET) {
#ifdef E3D_TARGET_WINDOWS
        Eng3D::Log::error("network", "WSA Code " + std::to_string(WSAGetLastError()));
        WSACleanup();
#endif
        CXX_THROW(Eng3D::Networking::SocketException, "Can't create client socket");
    }

    if(connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
#ifdef E3D_TARGET_UNIX
        close(fd);
#elif defined E3D_TARGET_WINDOWS
        Eng3D::Log::error("network", "WSA Code " + std::to_string(WSAGetLastError()));
        closesocket(fd);
#endif
        CXX_THROW(Eng3D::Networking::SocketException, "Can't connect to server");
    }
}

Eng3D::Networking::Client::~Client() {
#ifdef E3D_TARGET_WINDOWS
    closesocket(fd);
    WSACleanup();
#else
    close(fd);
#endif
}

