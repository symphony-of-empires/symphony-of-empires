#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <mutex>

#ifdef unix
#	define _XOPEN_SOURCE_EXTENDED 1
#	include <netdb.h>
#	include <arpa/inet.h>
#endif
#include <sys/types.h>

// Visual Studio does not know about UNISTD.H, Mingw does through
#ifndef _MSC_VER
#	include <unistd.h>
#endif
#ifdef windows
#   include <winsock2.h>
#endif

#include "unified_render/network.hpp"
#include "unified_render/print.hpp"

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
        if(r < 0)
            throw UnifiedRender::Networking::SocketException("Can't send data of packet");
        i += (size_t)r;
    }
}

void UnifiedRender::Networking::SocketStream::recv(void* data, size_t size) {
    char* c_data = (char*)data;
    for(size_t i = 0; i < size; ) {
        int r = ::recv(fd, &c_data[i], std::min<size_t>(1024, size - i), MSG_WAITALL);
        if(r < 0)
            throw UnifiedRender::Networking::SocketException("Can't receive data of packet");
        i += (size_t)r;
    }
}

UnifiedRender::Networking::ServerClient::ServerClient(void) {

}

UnifiedRender::Networking::ServerClient::~ServerClient(void) {
    thread.join();
}

UnifiedRender::Networking::Server::Server(const unsigned port, const unsigned max_conn)
    : n_clients(max_conn)
{
    run = true;
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
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);

    // We need to ignore pipe signals since any client disconnecting **will** kill the server
    signal(SIGPIPE, SIG_IGN);
#endif
    print_info("Server created sucessfully and listening to %u; now invite people!", port);
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
                std::scoped_lock lock(clients[i].pending_packets_mutex);
                clients[i].pending_packets.push_back(packet);
            }

            // Disconnect the client when more than 200 MB is used
            // we can't save your packets buddy - other clients need their stuff too!
            size_t total_size = 0;
            for(const auto& packet_q : clients[i].pending_packets) {
                total_size += packet_q.buffer.size();
            }
            if(total_size >= 200 * 1000000) {
                clients[i].is_connected = false;
                print_error("Client %zu has exceeded max quota! - It has used %zu bytes!", i, total_size);
            }
        }
    }
}
