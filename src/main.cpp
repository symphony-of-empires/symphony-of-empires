#include <string>
#include "world.hpp"

void rendering_main(void);

#ifdef WINDOWS
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgument, int iShow) {
	char * argv[1];
	argv[0] = "/";
	main(1, argv);
}
#endif

#include <thread>
int main(int argc, char ** argv) {
	World world("map_topo.png", "map_pol.png", "map_div.png", "map_infra.png");
	world.time = 695459;
	world.time -= (8600 * 76);
	world.time -= 24 * 190;

#ifndef UNIT_TEST
	printf("launching rendering thread\n");
	std::thread t1(rendering_main);

	int run = 1;
	while(run) {
		world.do_tick();
	}
	t1.join();
#endif
	return 0;
}