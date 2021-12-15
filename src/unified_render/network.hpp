#pragma once

#include <exception>
#ifdef unix
#	define _XOPEN_SOURCE_EXTENDED 1
#	include <sys/socket.h>
#	include <netinet/in.h>
#	ifndef INVALID_SOCKET
#		define INVALID_SOCKET -1
#	endif
#elif defined windows
#	ifndef _WINDOWS_
#		define WIN32_LEAN_AND_MEAN 1
#       ifndef NOMINMAX
#		    define NOMINMAX 1
#       endif
#		include <windows.h>
#		undef WIN32_LEAN_AND_MEAN
#	endif
/* Allow us to use deprecated functions like inet_addr */
#	define _WINSOCK_DEPRECATED_NO_WARNINGS
#	include <winsock2.h>
#	include <ws2def.h>
#	include <ws2tcpip.h>
#endif
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>

#include <zlib.h>

static inline size_t get_compressed_len(size_t len) {
    return (len + 6 + (((len + 16383) / 16384) * 5));
}

static inline  size_t compress(const void* src, size_t src_len, void* dest, size_t dest_len) {
    z_stream info = {};
    info.total_in = info.avail_in = src_len;
    info.total_out = info.avail_out = dest_len;
    info.next_in = (Bytef*)src;
    info.next_out = (Bytef*)dest;

    int r;
    r = deflateInit(&info, Z_DEFAULT_COMPRESSION);
    if(r == Z_OK) {
        r = deflate(&info, Z_FINISH);
        if(r == Z_STREAM_END) {
            return info.total_out;
        }
    }
    deflateEnd(&info);
    return info.total_out;
}

static inline size_t decompress(const void* src, size_t src_len, void* dest, size_t dest_len) {
    z_stream info = {};
    info.total_in = info.avail_in = src_len;
    info.total_out = info.avail_out = dest_len;
    info.next_in = (Bytef*)src;
    info.next_out = (Bytef*)dest;

    int r;
    r = inflateInit(&info);
    if(r == Z_OK) {
        r = inflate(&info, Z_FINISH);
        if(r == Z_STREAM_END) {
            return info.total_out;
        }
    }
    inflateEnd(&info);
    return info.total_out;
}

class SocketException: public std::exception {
    std::string buffer;
public:
    SocketException(const std::string& msg) {
        buffer = msg;
    }
    virtual const char* what(void) const noexcept {
        return buffer.c_str();
    }
};

class ServerException: public std::exception {
    std::string buffer;
public:
    ServerException(const std::string& msg) {
        buffer = msg;
    }
    virtual const char* what(void) const noexcept {
        return buffer.c_str();
    }
};

class ClientException: public std::exception {
    std::string buffer;
public:
    ClientException(const std::string& msg) {
        buffer = msg;
    }
    virtual const char* what(void) const noexcept {
        return buffer.c_str();
    }
};

class SocketStream {
    bool is_server_stream = false;
public:
    int fd;

    SocketStream() {};
    SocketStream(int _fd): fd(_fd) {};

    void send(const void* data, size_t size);
    void recv(void* data, size_t size);
};

enum class PacketCode {
    OK,
    PACKET_ERROR,
};

class Packet {
    size_t n_data = 0;
    PacketCode code = PacketCode::OK;
public:
    std::vector<uint8_t> buffer;
    SocketStream stream;

    Packet() {};
    Packet(int _fd) { stream = SocketStream(_fd); };
    ~Packet() {};

    inline void* data(void) {
        return (void*)&buffer[0];
    }

    inline void data(void* buf, size_t size) {
        n_data = size;
        buffer.resize(n_data);
        std::memcpy(&buffer[0], buf, size);
    }

    inline size_t size(void) {
        return n_data;
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

        uint8_t* new_buf = new uint8_t[size];
        size_t new_size = compress(buf, size, new_buf, size);
        stream.send(&new_buf[0], new_size);
        //stream.send(&buffer[0], n_data);
        delete[] new_buf;

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
            size_t new_size = get_compressed_len(n_data);
            char* new_buf = new char[new_size];
            size_t decomp_size = decompress(buf, n_data, new_buf, new_size);
            std::memcpy(buf, &new_buf[0], decomp_size);
            delete[] new_buf;
            //std::memcpy(buf, &buffer[0], n_data);
        }

        uint16_t eof_marker;
        stream.recv(&eof_marker, sizeof(eof_marker));
        if(ntohs(eof_marker) != 0xE0F)
            throw SocketException("Packet with invalid EOF");
    }

    inline void recv(void) {
        this->recv<void>();
    }

    inline bool is_ok() {
        return (code == PacketCode::OK);
    }
};