#include <string>
#include "../world.hpp"
#include "lua_api.hpp"

void client_main(int argc, char** argv);

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
#include "../path.hpp"
#include "server_network.hpp"

#include <iostream>
#include <fstream>

#include "../io_impl.hpp"

#include "../actions.hpp"
std::mutex world_lock;

std::string async_get_input(void) {
    std::cout << "server> ";

    std::string cmd;
    std::cin >> cmd;
    return cmd;
}

#include <iostream>
#include <future>

#define _XOPEN_SOURCE 700
#include <cstdio>
#include <dirent.h>
#include <sys/types.h>
int main(int argc, char** argv) {
#ifdef unix
    setlocale(LC_ALL, "");
    bindtextdomain("main", Path::get("locale").c_str());
    textdomain("main");
#endif
    DIR *dir = opendir(Path::get_full().c_str());
    if(dir != NULL) {
        struct dirent *de;
        while(de = readdir(dir)) {
            if(de->d_name[0] == '.')
                continue;
            
            if(de->d_type == DT_DIR) {
                Path::add_path(de->d_name);
            }
        }
    }

#ifndef UNIT_TEST
    // Run as a server for servicing multiple clients
    World* world = new World();
    world->load_mod();
    Server* server = new Server(1836);

    std::future<std::string> future = std::async(std::launch::async, async_get_input);
    run = true;
    while(run) {
        std::unique_lock<std::mutex> lock(world_lock);
        world->do_tick();

        if(future.wait_for(std::chrono::milliseconds(10)) == std::future_status::ready) {
            std::string r = future.get();

            if(r == "brick") {
                std::cout << gettext("Bricking! :D") << std::endl;
                for(auto& province: world->provinces) {
                    for(auto& pop: province->pops) {
                        pop.militancy = 1.f;
                        pop.life_needs_met = 0.f;
                        pop.budget = 0.f;
                    }
                }
                break;
            }
            // generate a graphviz of the supply chain (abstract, just using industry types and goods)
            else if(r == "gvsch") {
                std::cout << "digraph {" << std::endl;
                for(const auto& unit_type: world->unit_types) {
                    std::cout << "  " << unit_type->ref_name << " [ label = \""
                        << unit_type->name
                        << "\", color = \"yellow\""
                        << "];"
                        << std::endl;
                    

                    for(const auto& good: unit_type->req_goods) {
                        std::cout << "  " << good.first->ref_name << " -> " << unit_type->ref_name << ";" << std::endl;
                    }
                }

                for(const auto& boat_type: world->boat_types) {
                    std::cout << "  " << boat_type->ref_name << " [ label = \""
                        << boat_type->name
                        << "\", color = \"yellow\""
                        << "];"
                        << std::endl;
                    
                    for(const auto& good: boat_type->req_goods) {
                        std::cout << "  " << good.first->ref_name << " -> " << boat_type->ref_name << ";" << std::endl;
                    }
                }

                for(const auto& good: world->goods) {
                    std::cout << "  " << good->ref_name << " [ label = \""
                        << good->name
                        << "\", color = \"blue\""
                        << "];"
                        << std::endl;
                }

                for(const auto& industry_type: world->building_types) {
                    if(industry_type->is_factory == false) {
                        continue;
                    }

                    std::cout << "  " << industry_type->ref_name
                        << " [ label = \""
                        << industry_type->name
                        << "\", color = \"red\""
                        << "];"
                        << std::endl;

                    for(const auto& output: industry_type->outputs) {
                        std::cout << "  " << industry_type->ref_name << " -> " << output->ref_name << ";" << std::endl;
                    }
                    for(const auto& input: industry_type->inputs) {
                        std::cout << "  " << input->ref_name << " -> " << industry_type->ref_name << ";" << std::endl;
                    }
                }
                std::cout << "}" << std::endl;
            // generate a graphviz detailing supply chain 
            } else if(r == "exit") {
                std::cout << gettext("Quitting...") << std::endl;
                run = false;
                break;
            } else {
                int ret = luaL_loadstring(world->lua, r.c_str());
                if (ret == 0) {
                    lua_pcall(world->lua, 0, 0, 0);
                } else {
                    print_error(gettext("Failed to load lua from input string"));
                }
            }
            future = std::async(std::launch::async, async_get_input);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
        
    print_info(gettext("Destroying world"));
    delete world;
    print_info(gettext("Destroying server"));
    delete server;
#else
    exit(EXIT_SUCCESS);
#endif
    return 0;
}

#ifdef windows
#include <windows.h>
#include <cstring>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgument, int iShow) {
    char* argv[1];
    argv[0] = new char[2];
    strcpy((char*)argv[0], "/");
    
    try {
        main(1, argv);
    } catch(const std::exception& e) {
        print_error("%s", e.what());
    }
    
    free(argv[0]);
}
#endif
