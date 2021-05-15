#include <string>
#include "world.hpp"
#include "lua.hpp"

void rendering_main(void);

#include <chrono>
#include <thread>

#include <atomic>
std::atomic<int> run;

#include <libintl.h>
#include <locale.h>
#include "path.hpp"

int main(int argc, char ** argv) {
	setlocale(LC_ALL, "");
	bindtextdomain("main", Path::get("locale").c_str());
	textdomain("main");

	World world = World();
	world.time = 695459;
	world.time -= (8600 * 76);
	world.time -= 24 * 190;

#ifndef UNIT_TEST
	printf(gettext("launching rendering thread"));
	printf("\n");
	std::thread t1(rendering_main);
	
	run = 1;
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
