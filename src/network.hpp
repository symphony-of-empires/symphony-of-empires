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
	uint8_t* bufdata = nullptr;
	size_t n_data = 0;
	PacketCode code = PACKET_OK;
	int fd;
public:
	Packet(int _fd, std::string buf) : fd(_fd) {
		n_data = buf.size();
		bufdata = new unsigned char[n_data];
		for(size_t i = 0; i < n_data; i++) {
			bufdata[i] = buf[i];
		}
	};

	Packet(int _fd) : fd(_fd) {};
	~Packet() {
		delete bufdata;
	};

	void* data(void) {
		return (void *)bufdata;
	}

	size_t size(void) {
		return n_data;
	}

	template<typename T>
	void send(const T* buf, size_t size = sizeof(T)) {
		n_data = size;
		bufdata = (uint8_t *)realloc(bufdata, n_data);
		memcpy(bufdata, buf, n_data);

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
			int r = write(fd, bufdata + i, std::min<size_t>(1024, n_data - i));
			if(r == -1) {
				throw std::runtime_error("Socket write error for data in packet");
			}
			i += r;
		}
		
		print_info("(host) -> Packet of size %zu, with code %zu\n", (size_t)n_data, (size_t)net_code);
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
		bufdata = (uint8_t *)realloc(bufdata, n_data);
		
		/* Reads can only be done 1024 bytes at a time */
		for(size_t i = 0; i < n_data; ) {
			int r = ::recv(fd, bufdata + i, std::min<size_t>(1024, n_data - i), MSG_WAITALL);
			if(r == -1) {
				throw std::runtime_error("Socket read error for data in packet");
			}
			i += r;
		}
		
		if(buf != nullptr)
			memcpy(buf, bufdata, n_data);
		
		print_info("(host) <- Packet of size %zu, with code %zu\n", (size_t)n_data, (size_t)net_code);
	}
	
	void recv(void) {
		this->recv<void>();
	}

	bool is_ok() {
		return (code == PACKET_OK);
	}
};

class Server {
	struct sockaddr_in addr;
	int fd;

	std::vector<std::thread> threads;
	std::atomic<bool> run;
public:
	Server(unsigned port = 1825, unsigned max_conn = 16);
	~Server();
	void client_loop(void);
	
	std::vector<Packet *> packet_queue;
};

class Client {
	struct sockaddr_in addr;
	int fd;
public:
	Client(std::string host, unsigned port);
	~Client();
	int get_fd(void) {
		return fd;
	}

	std::vector<Packet *> packet_queue;
};

extern Server* g_server;

#endif
