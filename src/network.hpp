#ifndef NETWORK_H
#define NETWORK_H

#ifdef unix
#	define _XOPEN_SOURCE_EXTENDED 1
#	include <sys/socket.h>
#	include <netinet/in.h>
#elif defined windows
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <winsock2.h>
#	include <ws2def.h>
#	include <ws2tcpip.h>
#endif

#include <thread>
#include <vector>
#include <atomic>

#include <cstring>
#include <unistd.h>
#include <stdexcept>

#include "print.hpp"
enum PacketCode {
	PACKET_OK,
	PACKET_ERROR,
};
class Packet {
	std::vector<uint8_t> bufdata;
	size_t n_data = 0;
	PacketCode code = PACKET_OK;
public:
#ifdef unix
	int fd;
#elif defined windows
	SOCKET fd;
#endif

	Packet() {};
#ifdef unix
	Packet(int _fd) : fd(_fd) {};
#elif defined windows
	Packet(SOCKET _fd) : fd(_fd) {};
#endif

	void* data(void) {
		return (void *)&bufdata[0];
	}
	
	void data(void* _bufdata, size_t _n_data) {
		n_data = _n_data;
		bufdata.resize(n_data);
		memcpy(&bufdata[0], _bufdata, n_data);
	}

	size_t size(void) {
		return n_data;
	}

	template<typename T>
	void send(const T* buf = nullptr, size_t size = sizeof(T)) {
		if(buf != nullptr) {
			n_data = size;
			bufdata.resize(n_data);
			memcpy(&bufdata[0], buf, n_data);
		}

		const uint32_t net_code = htonl(code);
		if(::send(fd, (const char*)&net_code, sizeof(net_code), 0) == -1) {
			throw std::runtime_error("Socket write error for packet code");
		}
		
		const uint32_t net_size = htonl(n_data);
		if(::send(fd, (const char*)&net_size, sizeof(net_size), 0) == -1) {
			throw std::runtime_error("Socket write error for size of packet");
		}
		
		/* Socket writes can only be done 1024 bytes at a time */
		for(size_t i = 0; i < n_data; ) {
			int r;
			r = ::send(fd, (const char*)&bufdata[i], std::min<size_t>(1024, n_data - i), 0);
			if(r == -1) {
				throw std::runtime_error("Socket write error for data in packet");
			}
			i += r;
		}
	}

	void send(void) {
		this->send<void>(nullptr, 0);
	}

	template<typename T>
	void recv(T* buf = nullptr) {
		uint32_t net_code;
		if(::recv(fd, (char*)&net_code, sizeof(net_code), MSG_WAITALL) == -1) {
#ifdef windows
			print_error("WSA Code: %u", WSAGetLastError());
#endif
			throw std::runtime_error("Socket read error for packet code");
		}
		net_code  = ntohl(net_code);
		code = (PacketCode)net_code;

		uint32_t net_size;
		if(::recv(fd, (char*)&net_size, sizeof(net_size), MSG_WAITALL) == -1) {
#ifdef windows
			print_error("WSA Code: %u", WSAGetLastError());
#endif
			throw std::runtime_error("Socket read error for size of packet");
		}
		
		n_data = (size_t)ntohl(net_size);
		bufdata.resize(n_data + 1);
		
		/* Reads can only be done 1024 bytes at a time */
		for(size_t i = 0; i < n_data; ) {
			int r;
			r = ::recv(fd, (char*)&bufdata[i], std::min<size_t>(1024, n_data - i), MSG_WAITALL);
			if(r == -1) {
#ifdef windows
				print_error("WSA Code: %u", WSAGetLastError());
#endif
				throw std::runtime_error("Socket read error for data in packet");
			}
			i += r;
		}
		
		if(buf != nullptr)
			memcpy(buf, &bufdata[0], n_data);
	}
	
	void recv(void) {
		this->recv<void>();
	}

	bool is_ok() {
		return (code == PACKET_OK);
	}
};

#include <deque>
#include <mutex>
class Server {
	struct sockaddr_in addr;
#ifdef unix
	int fd;
#elif defined windows
	SOCKET fd;
#endif

	std::vector<std::thread> threads;
	std::vector<std::deque<Packet>> packet_queues;
	// std::vector hates mutexes because they are not copy-able
	std::mutex* packet_mutexes;
	
	std::atomic<bool> run;
public:
	Server(unsigned port = 1825, unsigned max_conn = 16);
	~Server();
	
	void broadcast(Packet& packet);
	void net_loop(int id);
	
	int n_clients;
};

class Client {
	struct sockaddr_in addr;
#ifdef unix
	int fd;
#elif defined windows
	SOCKET fd;
#endif
	
	std::thread net_thread;
	std::atomic<bool> has_snapshot;
public:
	Client(std::string host, const unsigned port);
	~Client();
	int get_fd(void) {
		return fd;
	}
	
	void net_loop(void);
	void wait_for_snapshot(void);
	
	std::mutex packet_mutex;
	std::deque<Packet> packet_queue;
};

extern Server* g_server;
extern Client* g_client;

#endif
