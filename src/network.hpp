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
	unsigned char * data;
	size_t n_data;
	PacketCode code = PACKET_OK;
	int fd;
public:
	Packet(int _fd, std::string buf) : fd(_fd) {
		n_data = buf.size();
		data = new unsigned char[n_data];
		for(size_t i = 0; i < n_data; i++) {
			data[i] = buf[i];
		}
	};

	template<typename T>
	Packet(int _fd, T * buf) : n_data(sizeof(T)), fd(_fd) {
		data = new unsigned char[n_data];
		memcpy(data, buf, n_data);
	};

	template<typename T, typename S>
	Packet(int _fd, T * buf, S size) : fd(_fd), n_data(size) {
		data = new unsigned char[n_data];
		memcpy(data, buf, n_data);
	}

	Packet(int _fd) : fd(_fd) {};
	~Packet() {
		delete data;
	};

	inline void compose(void) {
		uint32_t net_size = htonl(n_data);
		uint32_t net_code = htonl(code);

		if(write(fd, &net_size, sizeof(net_size)) == -1) {
			print_error("Socket write error");
			return;
		}
		if(write(fd, data, n_data) == -1) {
			print_error("Socket write error");
			return;
		}
		if(write(fd, &net_code, sizeof(net_code)) == -1) {
			print_error("Socket write error");
			return;
		}
	}
	inline void decompose(void) {
		uint32_t net_size, net_code;

		if(read(fd, &net_size, sizeof(net_size)) == -1) {
			print_error("Socket read error");
			return;
		}
		net_size = ntohl(net_size);
		n_data = (size_t)net_size;

		data = new unsigned char[n_data];
		if(read(fd, data, net_size) == -1) {
			print_error("Socket read error");
			return;
		}
		
		if(read(fd, &net_code, sizeof(net_code)) == -1) {
			print_error("Socket read error");
			return;
		}
		net_code  = ntohl(net_code);
		code = (PacketCode)net_code;
	}
	inline bool is_ok() {
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
	void client_loop(void);
};

#endif