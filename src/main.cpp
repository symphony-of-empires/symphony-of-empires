#include <string>
#include "world.hpp"
#include "lua.hpp"

void rendering_main(void);

#include <chrono>
#include <thread>
#include <queue>

int greedy_bfs(World * world, int cx, int cy, int gx, int gy) {

}

int main(int argc, char ** argv) {
	World world("map_topo.png", "map_pol.png", "map_div.png", "map_infra.png");
	world.time = 695459;
	world.time -= (8600 * 76);
	world.time -= 24 * 190;

#ifndef UNIT_TEST
	printf("launching rendering thread\n");
	std::thread t1(rendering_main);

	const bool run = true;
	while(run) {
		LuaAPI::check_events(world.lua);
		world.do_tick();

		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
	t1.join();
#else
	exit(EXIT_SUCCESS);
#endif
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