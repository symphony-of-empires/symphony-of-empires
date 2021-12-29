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

SocketStream::SocketStream(void) {

}

SocketStream::SocketStream(int _fd)
    : fd(_fd)
{

}

SocketStream::~SocketStream(void) {

}

void SocketStream::send(const void* data, size_t size) {
    const char* c_data = (const char*)data;
    for(size_t i = 0; i < size; ) {
        int r = ::send(fd, &c_data[i], std::min<size_t>(1024, size - i), 0);
        if(r < 0)
            throw SocketException("Can't send data of packet");
        i += (size_t)r;
    }
}

void SocketStream::recv(void* data, size_t size) {
    char* c_data = (char*)data;
    for(size_t i = 0; i < size; ) {
        int r = ::recv(fd, &c_data[i], std::min<size_t>(1024, size - i), MSG_WAITALL);
        if(r < 0)
            throw SocketException("Can't receive data of packet");
        i += (size_t)r;
    }
}
