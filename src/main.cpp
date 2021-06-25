#include <string>
#include "world.hpp"
#include "lua.hpp"

void rendering_main(void);

#include <chrono>
#include <thread>

#include <atomic>
std::atomic<bool> run;
std::atomic<bool> paused;
std::atomic<bool> do_start;

#include <libintl.h>
#include <locale.h>
#include "path.hpp"
#include "network.hpp"

#include <iostream>
#include <fstream>

#include "io_impl.hpp"

#include "actions.hpp"
std::mutex world_lock;
std::string server_addr;

void client_loop_thread(void) {
	Client* client = new Client(server_addr, 4206);
	Packet* packet = new Packet(client->get_fd());

	try {
		enum ActionType action;
		Archive ar = Archive();

		while(1) {
			packet->recv(&action);
			print_info("Received action %zu", (size_t)action);

			// Ping from server, we should answer with a pong!
			if(action == ACTION_PING) {
				action = ACTION_PONG;
				packet->send(&action);
			}
			// Update a province; this action is sent by the server to indicate a
			// change in the information of a province
			else if(action == ACTION_UPDATE_PROVINCE) {
				ProvinceId id;
				Province up;

				packet->recv();

				ar.set_buffer(packet->data(), packet->size());
				::deserialize(ar, &id);
				::deserialize(ar, &up);

				memcpy(g_world->provinces[id], &up, sizeof(Province));
			}
		}
	} catch(std::runtime_error& e) {
		print_error("Except: %s", e.what());
	}

	delete packet;
	delete client;
}

int main(int argc, char ** argv) {
	setlocale(LC_ALL, "");
	bindtextdomain("main", Path::get("locale").c_str());
	textdomain("main");
	
#ifndef UNIT_TEST
	// Run as a server for servicing multiple clients
	if(argc > 1 && !strcmp(argv[1], "server")) {
		World* world = new World(false);
		Server* server = new Server(4206);
		
		Archive* stream;
		
		while(!do_start);
		
		paused = false;
		while(run) {
			std::unique_lock<std::mutex> lock(world_lock);
			world->do_tick();
			while(paused);
		}
	}
	// Run as a client, receiving and sending packages to/from a server
	else {
		World* world = new World(true);

		if(argc > 1) {
			server_addr = argv[1];
		} else {
			server_addr = "127.0.0.1";
			print_info("No IP specified, assuming default %s", server_addr.c_str());
		}
		print_info("Connecting to server with IP %s", server_addr.c_str());

		std::thread t1(client_loop_thread);
		
		printf("%s\n", gettext("launching rendering thread"));
		std::thread t2(rendering_main);
		
		while(!do_start);
		
		paused = false;
		while(run) {
			std::unique_lock<std::mutex> lock(world_lock);
			world->do_tick();
			world->client_update();
			while(paused);
		}
		
		t1.join();
		t2.join();
	}
#endif
	exit(EXIT_SUCCESS);
	return 0;
}

#ifdef WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgument, int iShow) {
	char* argv[1];
	argv[0] = "/";
	main(1, argv);
}
#endif
