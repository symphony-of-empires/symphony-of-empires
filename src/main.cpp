#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <atomic>
#include <cstdlib>

#include <libintl.h>
#include <locale.h>
#include "path.hpp"
#include "network.hpp"

#include "io_impl.hpp"

extern TextureManager * g_texture_manager;
std::mutex world_lock;
std::atomic<bool> run;
std::atomic<bool> paused;
std::atomic<bool> do_start;

int main(int argc, char ** argv) {
	setlocale(LC_ALL, "");
	bindtextdomain("main", Path::get("locale").c_str());
	textdomain("main");

	if(!strcmp("server", argv[1])) {
		Server * server = new Server(1825, 24);
		World * world = new World(false);

		print_info("Size: %zu x %zu", g_world->width, g_world->height);
		print_info("Seal level: %lu", g_world->sea_level);
		print_info("Time: %lu", g_world->time);
		print_info("Cultures: %zu", g_world->cultures.size());
		print_info("Nations: %zu", g_world->nations.size());
		print_info("PopTypes: %zu", g_world->pop_types.size());
		print_info("Provinces: %zu", g_world->provinces.size());
		print_info("IndustryTypes: %zu", g_world->industry_types.size());
		print_info("Goods: %zu", g_world->goods.size());
		print_info("Orders: %zu, Delivers: %zu", g_world->orders.size(), g_world->delivers.size());

		// TODO: For debugging only, in release should be set via cmd
		do_start = true;
		run = true;
		paused = false;

		while(!do_start);
		while(run) {
			//std::unique_lock<std::mutex> lock(world_lock);

			world_lock.lock();
			world->do_tick();
			world_lock.unlock();

			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
			while(paused);
		}
	} else {
		Client * client = new Client("127.0.0.1", 1825);
		World * world = new World(true);

		Packet * packet = new Packet(client->get_fd());

		uint16_t version;
		packet->recv(&version);

		print_info("Server's version: %u\n", version);
		print_info("Our version: %u\n", 0x0001);

		packet->recv<void>();

		Archive ar = Archive();
		ar.buffer.resize(packet->size());
		memcpy(&ar.buffer[0], packet->data(), packet->size());
		deserialize(ar, *g_world);

		print_info("Size: %zu x %zu", g_world->width, g_world->height);
		print_info("Seal level: %lu", g_world->sea_level);
		print_info("Time: %lu", g_world->time);
		print_info("Cultures: %zu", g_world->cultures.size());
		print_info("Nations: %zu", g_world->nations.size());
		print_info("PopTypes: %zu", g_world->pop_types.size());
		print_info("Provinces: %zu", g_world->provinces.size());
		print_info("IndustryTypes: %zu", g_world->industry_types.size());
		print_info("Goods: %zu", g_world->goods.size());

		//g_world = (World *)packet->data();

		while(1) {

		}
		delete packet;
	}
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
