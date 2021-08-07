#ifdef unix
#	define _XOPEN_SOURCE_EXTENDED 1
#	include <netdb.h>
#	include <arpa/inet.h>
#endif

#include <sys/types.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <mutex>
/* Visual Studio does not know about UNISTD.H, Mingw does through */
#ifndef _MSC_VER
#	include <unistd.h>
#endif

#include "network.hpp"
#include "print.hpp"

#ifdef unix
#	include <poll.h>
#elif defined windows
/* MingW does not behave well with pollfd structures, however MSVC does */
#	ifndef _MSC_VER
typedef struct pollfd {
    SOCKET fd;
    SHORT events;
    SHORT revents;
} WSAPOLLFD, *PWSAPOLLFD, FAR *LPWSAPOLLFD;
WINSOCK_API_LINKAGE int WSAAPI WSAPoll(LPWSAPOLLFD fdArray, ULONG fds, INT timeout);
#	endif
#endif

void SocketStream::write(const void* data, size_t size) {
    for(size_t i = 0; i < size; ) {
        int r = ::send(fd, (const char*)data, std::min<size_t>(1024, size - i), 0);
        if(r < 0)
            throw SocketException("Socket write error for data in packet");
        i += (size_t)r;
    }
}

void SocketStream::read(void* data, size_t size) {
    for(size_t i = 0; i < size; ) {
        int r = ::recv(fd, (char*)data, std::min<size_t>(1024, size - i), MSG_WAITALL);
        if(r < 0)
            throw SocketException("Socket read error for data in packet");
        i += (size_t)r;
    }
}
