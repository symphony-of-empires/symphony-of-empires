#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>

#include <thread>
#include <vector>
#include <atomic>

#include <cstring>
#include <unistd.h>
#include "print.hpp"
enum PacketCode {
	PACKET_OK,
	PACKET_ERROR,
};
class Packet {
	uint8_t * bufdata = nullptr;
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

	void * data(void) {
		return (void *)bufdata;
	}

	size_t size(void) {
		return n_data;
	}

	template<typename T>
	void send(const T * buf, size_t size = sizeof(T)) {
		n_data = size;
		bufdata = (uint8_t *)realloc(bufdata, n_data);
		memcpy(bufdata, buf, n_data);

		uint32_t net_code = htonl(code);
		if(write(fd, &net_code, sizeof(net_code)) == -1) {
			print_error("Socket write error for packet code");
			return;
		}

		uint32_t net_size = htonl(n_data);
		if(write(fd, &net_size, sizeof(net_size)) == -1) {
			print_error("Socket write error for size of packet");
			return;
		}

		if(write(fd, bufdata, n_data) == -1) {
			print_error("Socket write error for data in packet");
			return;
		}
	}

	template<typename T>
	void recv(T * buf = nullptr) {
		uint32_t net_code;
		if(read(fd, &net_code, sizeof(net_code)) == -1) {
			print_error("Socket read error for packet code");
			return;
		}
		net_code  = ntohl(net_code);
		code = (PacketCode)net_code;

		uint32_t net_size;
		if(read(fd, &net_size, sizeof(net_size)) == -1) {
			print_error("Socket read error for size of packet");
			return;
		}
		net_size = ntohl(net_size);
		n_data = (size_t)net_size;

		bufdata = (uint8_t *)realloc(bufdata, n_data);
		if(read(fd, bufdata, n_data) == -1) {
			print_error("Socket read error for data in packet");
			return;
		}

		if(buf != nullptr)
			memcpy(buf, bufdata, n_data);
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
	void client_loop(void);
};

#endif
