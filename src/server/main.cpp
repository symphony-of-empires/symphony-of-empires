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

    try {
        std::cout << "Type start to start the simulation or help to view all the commands" << std::endl;

        std::future<std::string> future = std::async(std::launch::async, async_get_input);
        run = true;

        bool paused = true;
        while(run) {
            if(!paused) {
                std::unique_lock<std::mutex> lock(world_lock);
                world->do_tick();
            }

            if(future.wait_for(std::chrono::milliseconds(10)) == std::future_status::ready) {
                std::string r = future.get();

                if(r == "help" || r == "info") {
                    std::cout << "start: Start the simulation" << std::endl;
                    std::cout << "stop: Pause the simulation" << std::endl;
                    std::cout << "resetp: Reset all POPs" << std::endl;
                    std::cout << "gvsch: Generate a graphviz of the supply chain" << std::endl;
                    std::cout << "quit: Quit the server" << std::endl;
                    std::cout << "lsc: List all clients" << std::endl;
                    std::cout << "debugen: Enable debug" << std::endl;
                    std::cout << "debugdis: Disable debug" << std::endl;
                }
                else if(r == "debugen") {
                    print_enable_debug();
                }
                else if(r == "debugdis") {
                    print_disable_debug();
                }
                else if(r == "lsc") {
                    for(size_t i = 0; i < server->n_clients; i++) {
                        ServerClient& cl = server->clients[i];
                        
                        if(cl.is_connected == true) {
                            std::cout << i << " " << cl.username << std::endl;
                        }
                    }
                }
                else if(r == "start" || r == "unpause") {
                    paused = false;
                }
                else if(r == "stop" || r == "pause" || r == "halt") {
                    paused = true;
                }
                else if(r == "reload" || r == "reset" || r == "retest") {
                    paused = true;
                    delete world;
                    world = new World();
                    world->load_mod();
                }
                else if(r == "resetp") {
                    for(auto& province: world->provinces) {
                        for(auto& pop: province->pops) {
                            pop.militancy = 1.f;
                            pop.life_needs_met = 0.f;
                            pop.budget = 0.f;
                        }
                    }
                }
                // CDCS JSON format
                else if(r == "cdcs_json") {
                    size_t i;
                    std::cout << "{" << std::endl;
                    
                    i = 0;
                    std::cout << "  \"provinces\": {" << std::endl;
                    for(const auto& province: world->provinces) {
                        std::cout << "    \"" << (size_t)province->color << "\":\"" << province->name << "\"";

                        i++;
                        if(i < world->provinces.size()) {
                            std::cout << ",";
                        }
                        std::cout << std::endl;
                    }
                    std::cout << "  }," << std::endl;

                    i = 0;
                    std::cout << "  \"provinces_reverse\": {" << std::endl;
                    for(const auto& province: world->provinces) {
                        std::cout << "    \"" << province->name << "\":" << (size_t)province->color << "";

                        i++;
                        if(i < world->provinces.size()) {
                            std::cout << ",";
                        }
                        std::cout << std::endl;
                    }
                    std::cout << "  }" << std::endl;

                    std::cout << "}" << std::endl;
                }
                // Original CDCS file format
                else if(r == "cdcs_orig") {
                    for(const auto& province: world->provinces) {
                        std::cout << "(" << province->name << ", " << province->name << ", " << (size_t)((province->total_pops() + 1) / 1000.f) << ")" << std::endl;
                    }
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
                }
                // generate a graphviz detailing supply chain 
                else if(r == "exit" || r == "quit") {
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
    } catch(const std::exception& e) {
        print_error(e.what());
        throw;
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
