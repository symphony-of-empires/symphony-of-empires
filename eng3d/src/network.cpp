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
#define NETWORK_FLAG MSG_DONTWAIT
#elif defined E3D_TARGET_WINDOWS
#	define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#   ifndef WINSOCK2_IMPORTED
#       define WINSOCK2_IMPORTED
#       include <winsock2.h>
#       include <ws2tcpip.h>
#   endif
#	include <windows.h>
#define NETWORK_FLAG MSG_DONTROUTE
#endif
#include <sys/types.h>

#include "eng3d/network.hpp"
#include "eng3d/log.hpp"
#include "eng3d/serializer.hpp"
#include "eng3d/utils.hpp"

constexpr static int max_tries = 10; // 10 * 100ms = 10 seconds
constexpr static int tries_ms = 100;

//
// Socket stream
//
void Eng3D::Networking::SocketStream::send(const void* data, size_t size, std::function<bool()> pred) {
    const auto* c_data = reinterpret_cast<const char*>(data);
    auto tries = max_tries;
    for(size_t i = 0; i < size; ) {
        if(pred && !pred()) // If (any) predicate fails then return immediately
            return;
        int r = ::send(fd, &c_data[i], glm::min<std::size_t>(1024, size - i), NETWORK_FLAG);
        if(r <= 0) {
            if(!tries)
                CXX_THROW(Eng3D::Networking::SocketException, "Packet send interrupted");
            tries--;
            std::this_thread::sleep_for(std::chrono::milliseconds(tries_ms));
            continue;
        }
        i += static_cast<std::size_t>(r);
        tries = max_tries;
    }
}

void Eng3D::Networking::SocketStream::recv(void* data, size_t size, std::function<bool()> pred) {
    auto* c_data = reinterpret_cast<char*>(data);
    std::memset(c_data, 0, size);
    auto tries = max_tries;
    for(size_t i = 0; i < size; ) {
        if(pred && !pred()) // If (any) predicate fails then return immediately
            return;
        int r = ::recv(fd, &c_data[i], glm::min<std::size_t>(1024, size - i), NETWORK_FLAG);
        if(r <= 0) {
            if(!tries)
                CXX_THROW(Eng3D::Networking::SocketException, "Packet receive interrupted");
            tries--;
            std::this_thread::sleep_for(std::chrono::milliseconds(tries_ms));
            continue;
        }
        i += static_cast<std::size_t>(r);
        tries = max_tries;
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
        Eng3D::Log::debug("socket_stream", translate("Can't set socket as non_blocking"));
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
// Packet
//
void Eng3D::Networking::Packet::send() {
    const uint16_t net_code = htons(static_cast<uint16_t>(code));
    stream.send(&net_code, sizeof(net_code), pred);
    const uint16_t net_size = htons(n_data);
    stream.send(&net_size, sizeof(net_size), pred);
    
    stream.send(buffer.data(), n_data, pred);

    const uint16_t eof_marker = htons(0xE0F);
    stream.send(&eof_marker, sizeof(eof_marker), pred);
}

void Eng3D::Networking::Packet::recv() {
    uint16_t net_code;
    stream.recv(&net_code, sizeof(net_code), pred);
    net_code  = ntohs(net_code);
    code = static_cast<PacketCode>(net_code);
    uint16_t net_size;
    stream.recv(&net_size, sizeof(net_size), pred);
    n_data = (size_t)ntohs(net_size);
    
    buffer.resize(n_data);
    stream.recv(buffer.data(), buffer.size(), pred);

    uint16_t eof_marker;
    stream.recv(&eof_marker, sizeof(eof_marker), pred);
    if(ntohs(eof_marker) != 0xE0F)
        CXX_THROW(Eng3D::Networking::SocketException, translate_format("Packet with invalid end marker %x, size %zu", (unsigned int)eof_marker, net_size));
}

//
// Server client
//
Eng3D::Networking::ServerClient::~ServerClient() {
    if(this->thread && this->thread->joinable())
        this->thread->join();
}

int Eng3D::Networking::ServerClient::try_connect(int fd) try {
    sockaddr_in client;
    socklen_t len = sizeof(client);
    conn_fd = accept(fd, reinterpret_cast<sockaddr*>(&client), &len);
    if(conn_fd == INVALID_SOCKET)
        CXX_THROW(Eng3D::Networking::SocketException, translate("Cannot accept client connection"));
    
    // At this point the client's connection was accepted - so we only have to check
    // Then we check if the server is running and we throw accordingly
    is_connected = true;
    Eng3D::Log::debug("server", translate("New client connection established"));
    return conn_fd;
} catch(Eng3D::Networking::SocketException& e) {
    return 0;
}

/// @brief TODO: flush packets
void Eng3D::Networking::ServerClient::flush_packets() {

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
        CXX_THROW(Eng3D::Networking::SocketException, translate("Cannot create server socket"));
#ifdef E3D_TARGET_UNIX
    int enable = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
#endif
    if(bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
        CXX_THROW(Eng3D::Networking::SocketException, translate("Cannot bind server"));
    if(listen(fd, max_conn) != 0)
        CXX_THROW(Eng3D::Networking::SocketException, translate("Cannot listen in specified number of concurrent connections"));
#ifdef E3D_TARGET_UNIX
    // Allow non-blocking operations on this socket (we don't want to block on multi-listener servers)
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
    // We need to ignore pipe signals since any client disconnecting **will** kill the server
    signal(SIGPIPE, SIG_IGN);
#endif
    this->run = true;
    Eng3D::Log::debug("server", Eng3D::translate_format("Server listening on IP port *::%u", port));
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
        if(this->clients[i].thread && this->clients[i].thread->joinable())
            this->clients[i].thread->join();
    delete[] this->clients;
}

/// @brief This will broadcast the given packet to all clients currently on the server
void Eng3D::Networking::Server::broadcast(const Eng3D::Networking::Packet& packet) {
    for(size_t i = 0; i < n_clients; i++)
        if(clients[i].is_connected == true)
            clients[i].packets.push(packet);
}

void Eng3D::Networking::Server::do_netloop(std::function<bool()> cond, std::function<void(int)> on_connect, std::function<void(void)> on_disconnect, std::function<void(const Packet& packet, Eng3D::Deser::Archive& ar)> handler, std::function<void(int i)> on_wake_thread, int id) {
    auto& cl = clients[id];
    int conn_fd = 0;
    try {
        cl.is_connected = false;
        while(!cl.is_connected) {
            conn_fd = cl.try_connect(fd);
            // Perform a 5 second delay between connection tries
            const auto delta = std::chrono::seconds{ 5 };
            const auto start_time = std::chrono::system_clock::now();
            std::this_thread::sleep_until(start_time + delta);
            if(!this->run) CXX_THROW(Eng3D::Networking::Server::Exception, "Server closed");
        }

        Eng3D::Networking::Packet packet(conn_fd);
        packet.pred = cond;

        player_count++;
        // Wake up another thread
        for(size_t i = 0; i < n_clients; i++) {
            if(clients[i].thread == nullptr) {
                on_wake_thread(i);
                break;
            }
        }
        on_connect(conn_fd); // Read the data from client
        Eng3D::Deser::Archive ar{};
        while(cond() && cl.is_connected == true) {
            cl.flush_packets();
            if(cl.has_pending()) { // Check if we need to read packets
                packet.recv();
                if(!cond()) break;
                ar.set_buffer(packet.data(), packet.size());
                ar.rewind();
                Eng3D::Log::debug("server", translate_format("Receiving %zuB from #%i", packet.size(), id));
                handler(packet, ar);
            }
            ar.buffer.clear();
            ar.rewind();

            // After reading everything we will send our queue appropriately to the client
            Eng3D::Networking::Packet tosend_packet;
            while(cl.packets.try_pop(tosend_packet)) {
                tosend_packet.stream = Eng3D::Networking::SocketStream(conn_fd);
                Eng3D::Log::debug("server", translate_format("Sending package of %zuB", tosend_packet.size()));
                tosend_packet.send();
            }
            cl.packets.clear();
        }
    } catch(Eng3D::Networking::Server::Exception& e) {
        Eng3D::Log::error("server", std::string() + "Eng3D::Networking::Server::Exception: " + e.what());
    } catch(Eng3D::Networking::SocketException& e) {
        Eng3D::Log::error("server", std::string() + "Eng3D::Networking::SocketException: " + e.what());
    } catch(Eng3D::Deser::Exception& e) {
        Eng3D::Log::error("server", std::string() + "Eng3D::Deser::Exception: " + e.what());
    }

    player_count--;

    // Unlock mutexes so we don't end up with weird situations... like deadlocks
    cl.is_connected = false;
    cl.packets.clear();

    on_disconnect();
#ifdef E3D_TARGET_WINDOWS
    Eng3D::Log::error("server", "WSA Code: " + std::to_string(WSAGetLastError()));
    WSACleanup();
#endif
    Eng3D::Log::debug("server", "Client disconnected");
#ifdef E3D_TARGET_WINDOWS
    shutdown(conn_fd, SD_BOTH);
#elif defined E3D_TARGET_UNIX && !defined E3D_TARGET_SWITCH
    // Switch doesn't support shutting down sockets
    shutdown(conn_fd, SHUT_RDWR);
#endif
}

//
// Client
//
Eng3D::Networking::Client::Client(std::string host, const unsigned port) {
    // Initialize WSA
#ifdef E3D_TARGET_WINDOWS
    WSADATA data;
    if(WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        Eng3D::Log::error("network", Eng3D::translate_format("WSA code %s", WSAGetLastError()));
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
        Eng3D::Log::error("network", Eng3D::translate_format("WSA code %s", WSAGetLastError()));
        WSACleanup();
#endif
        CXX_THROW(Eng3D::Networking::SocketException, "Can't create client socket");
    }

    if(connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
#ifdef E3D_TARGET_UNIX
        close(fd);
#elif defined E3D_TARGET_WINDOWS
        Eng3D::Log::error("network", Eng3D::translate_format("WSA code %s", WSAGetLastError()));
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

void Eng3D::Networking::Client::do_netloop(std::function<bool()> cond, std::function<void(const Packet& packet, Eng3D::Deser::Archive& ar)> handler) try {
    Eng3D::Networking::SocketStream stream(fd);
    Eng3D::Networking::Packet packet(fd);
    packet.pred = cond;
    while(cond()) {
        // Conditional of above statements
        // When we are on host_mode we discard all potential packets sent by the server
        // (because our data is already synchronized since WE ARE the server)
        if(stream.has_pending()) {
            Eng3D::Deser::Archive ar{};
            // Obtain the action from the server
            while(1) {
                try {
                    packet.recv();
                    if(packet.size() <= 1) continue;
                    break;
                } catch(Eng3D::Networking::SocketException& e) {
                    // Pass
                }
                if(!cond()) CXX_THROW(Eng3D::Networking::Client::Exception, "Server closed");
            }
            ar.set_buffer(packet.data(), packet.size());
            ar.rewind();
            Eng3D::Log::debug("client", translate_format("Receiving package of %zuB", packet.size()));
            handler(packet, ar);
        }

        // Client will also flush it's queue to the server
        Eng3D::Networking::Packet tosend_packet;
        while(packets.try_pop(tosend_packet)) {
            tosend_packet.stream = Eng3D::Networking::SocketStream(fd);
            tosend_packet.pred = cond;
            Eng3D::Log::debug("client", translate_format("Sending package of %zuB", tosend_packet.size()));
            tosend_packet.send();
        }
    }
} catch(Eng3D::Networking::Client::Exception& e) {
    Eng3D::Log::error("client", translate_format("Exception: %s", e.what()));
}
