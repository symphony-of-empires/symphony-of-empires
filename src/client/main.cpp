#include <string>
#include "world.hpp"

void client_main(int argc, char** argv);

#ifdef windows
const char* gettext(const char* str) {
    return str;
}
#endif

#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <atomic>
std::atomic<bool> run;

#ifdef unix
#	include <libintl.h>
#	include <locale.h>
#endif
#include "path.hpp"
#include "network.hpp"
#include "io_impl.hpp"
#include "actions.hpp"
std::mutex world_lock;

std::string async_get_input(void) {
    std::cout << "server> ";

    std::string cmd;
    std::cin >> cmd;
    return cmd;
}

#include <iostream>
#include <future>
int main(int argc, char** argv) {
#ifdef unix
    setlocale(LC_ALL, "");
    bindtextdomain("main", Path::get("locale").c_str());
    textdomain("main");
#endif
    
#ifndef UNIT_TEST
    // Path::add_path("test");
    Path::add_path("base_game");
    
    client_main(argc, argv);
#endif
    exit(EXIT_SUCCESS);
    return 0;
}

#ifdef windows
#include <windows.h>
#include <cstring>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgument, int iShow) {
    char* argv[1];
    argv[0] = new char[2];
    strcpy((char*)argv[0], "/");
    
    main(1, argv);
    
    free(argv[0]);
}
#endif