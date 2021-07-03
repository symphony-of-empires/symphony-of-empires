#ifndef NETWORK_H
#define NETWORK_H

#define _XOPEN_SOURCE_EXTENDED 1
#include <sys/socket.h>
#include <netinet/in.h>

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
	int fd;
	
	Packet(int _fd, std::string buf) : fd(_fd) {
		n_data = buf.size();
		bufdata.resize(n_data);
		for(size_t i = 0; i < n_data; i++) {
			bufdata[i] = buf[i];
		}
	};

	Packet(int _fd) : fd(_fd) {};

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
		if(write(fd, &net_code, sizeof(net_code)) == -1) {
			throw std::runtime_error("Socket write error for packet code");
		}
		
		const uint32_t net_size = htonl(n_data);
		if(write(fd, &net_size, sizeof(net_size)) == -1) {
			throw std::runtime_error("Socket write error for size of packet");
		}
		
		/* Socket writes can only be done 1024 bytes at a time */
		for(size_t i = 0; i < n_data; ) {
			int r = write(fd, &bufdata[i], std::min<size_t>(1024, n_data - i));
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
		if(::recv(fd, &net_code, sizeof(net_code), MSG_WAITALL) == -1) {
			throw std::runtime_error("Socket read error for packet code");
		}
		net_code  = ntohl(net_code);
		code = (PacketCode)net_code;

		uint32_t net_size;
		if(::recv(fd, &net_size, sizeof(net_size), MSG_WAITALL) == -1) {
			throw std::runtime_error("Socket read error for size of packet");
		}
		
		n_data = (size_t)ntohl(net_size);
		bufdata.resize(n_data + 1);
		
		/* Reads can only be done 1024 bytes at a time */
		for(size_t i = 0; i < n_data; ) {
			int r = ::recv(fd, &bufdata[i], std::min<size_t>(1024, n_data - i), MSG_WAITALL);
			if(r == -1) {
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
	int fd;

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
	int fd;
	
	std::thread net_thread;
	std::atomic<bool> has_snapshot;
public:
	Client(std::string host, unsigned port);
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
