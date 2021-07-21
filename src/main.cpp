#include <string>
#include "world.hpp"
#include "lua_api.hpp"

void client_main(void);

#ifdef windows
const char* gettext(const char* str) {
    return str;
}
#endif

#include <chrono>
#include <thread>

#include <atomic>
std::atomic<bool> run;

#ifdef unix
#	include <libintl.h>
#	include <locale.h>
#endif
#include "path.hpp"
#include "network.hpp"

#include <iostream>
#include <fstream>

#include "io_impl.hpp"

#include "actions.hpp"
std::mutex world_lock;
std::string server_addr;

#include <iostream>
int main(int argc, char ** argv) {
#ifdef unix
    setlocale(LC_ALL, "");
    bindtextdomain("main", Path::get("locale").c_str());
    textdomain("main");
#endif
    
#ifndef UNIT_TEST
    // Run as a server for servicing multiple clients
    if(argc > 1 && !strcmp(argv[1], "server")) {
        World* world = new World(false);
        Server* server = new Server(1836);
        
        run = true;
        while(run) {
            std::unique_lock<std::mutex> lock(world_lock);
            world->do_tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        delete world;
        delete server;
    }
    // Run as a client, receiving and sending packages to/from a server
    else {
        if(argc > 1) {
            server_addr = argv[1];
        } else {
            server_addr = "127.0.0.1";
            print_info("No IP specified, assuming default %s", server_addr.c_str());
        }
        print_info("Connecting to server with IP %s", server_addr.c_str());
        print_info("Initializing empty world");
        World* world = new World(true);
        print_info("Creating client");
        Client* client = new Client(server_addr, 1836);
        print_info("Waiting for snapshot");
        client->wait_for_snapshot();
        print_info("Running main client");
        client_main();
        
        delete client;
        delete world;
    }
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
