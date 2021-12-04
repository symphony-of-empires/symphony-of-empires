#include <string>
#include "../world.hpp"
#include "lua_api.hpp"
#include "swap.hpp"

void client_main(int argc, char** argv);

#if defined windows
const char* gettext(const char* str) {
    return str;
}
#endif

#include <chrono>
#include <thread>

#include <atomic>
std::atomic<bool> run;

#if defined unix
#	include <libintl.h>
#	include <locale.h>
#endif
#include "../path.hpp"
#include "server_network.hpp"

#include <iostream>
#include <fstream>

#include "../io_impl.hpp"

#include "../action.hpp"
std::mutex world_lock;

std::string async_get_input(void) {
    std::cout << "server> ";

    std::string cmd;
    std::cin >> cmd;
    return cmd;
}

#include <iostream>
#include <future>

class ServerCommand {
public:
    std::function<void(World&, Server&, std::string)> invoke_cb;
    std::vector<std::string> aliases;
    std::string desc;
};

ServerCommand cmds[] = {
    { ([](World& world, Server& server, std::string cmdline) {
        std::cout << "Hello! :D" << std::endl;
    }), { "hello", "world", "test" }, "Prints hello world" },
    { ([](World& world, Server& server, std::string cmdline) {
        print_enable_debug();
    }), { "debug", "enabledebug" }, "Enable debugging features" },
    { ([](World& world, Server& server, std::string cmdline) {
        print_disable_debug();
    }), { "nodebug", "disabledebug" }, "Disable debugging features" },
    { ([](World& world, Server& server, std::string cmdline) {
        for(uint i = 0; i < server.n_clients; i++) {
            ServerClient& cl = server.clients[i];
            if(cl.is_connected == true) {
                std::cout << i << " " << cl.username << std::endl;
            }
        }
    }), { "list", "lsc" }, "List connected clients and usernames" },
    { ([](World& world, Server& server, std::string cmdline) {
        for(auto& province: world.provinces) {
            for(auto& pop: province->pops) {
                pop.budget = (pop.budget + 1.f) * 1000.f;
            }
        }
    }), { "money" }, "Gives everyone money" },
    { ([](World& world, Server& server, std::string cmdline) {
        for(auto& province: world.provinces) {
            for(auto& pop: province->pops) {
                pop.budget = 0.f;
            }
        }
    }), { "fuckmoney" }, "Remove all the money" },
    { ([](World& world, Server& server, std::string cmdline) {
        for(auto& province: world.provinces) {
            for(auto& pop: province->pops) {
                pop.militancy = 50.f;
            }
        }
    }), { "cantcope" }, "Increase militancy all over the world" },
    { ([](World& world, Server& server, std::string cmdline) {
        for(auto& province: world.provinces) {
            for(auto& pop: province->pops) {
                pop.militancy = -50.f;
            }
        }
    }), { "cope" }, "Make everyone more peaceful and less militant" },
    { ([](World& world, Server& server, std::string cmdline) {
        for(auto& product : world.products) {
            if(!product->good->is_edible) continue;
            product->price = 0.1f;
            product->price_vel = -50.f;
            
            for(auto& province : world.provinces) {
                province->stockpile[world.get_id(product)] += province->total_pops() * 500.f;
            }
        }
    }), { "maxfood" }, "Make all edible products have a price of 0.1 and increase their supply worldwide to satisfy x500 times the pops" },
    { ([](World& world, Server& server, std::string cmdline) {
        for(auto& province: world.provinces) {
            for(auto& pop: province->pops) {
                pop.life_needs_met = -8.f;
            }
        }
    }), { "socialism" }, "Starve everyone (WILL CAUSE MASSIVE IMMIGRATIONS)" },
    { ([](World& world, Server& server, std::string cmdline) {
        for(auto& province: world.provinces) {
            for(auto& pop: province->pops) {
                pop.life_needs_met = -100.f;
            }
        }
    }), { "communism" }, "Starve everyone harder (WILL CAUSE EVEN WORSE IMMIGRATIONS)" },
    { ([](World& world, Server& server, std::string cmdline) {
        if(!world.products.empty()) {
            // Highest priced product
            Product* hp_product = world.products[0];
            for(const auto& product : world.products) {
                if(product->price > hp_product->price) {
                    hp_product = product;
                }
            }

            std::cout << "Highest priced product" << std::endl;
            std::cout << "Company: " << hp_product->owner->name << std::endl;
            std::cout << "Province: " << hp_product->origin->name << std::endl;
            std::cout << "Type: " << hp_product->good->name << std::endl;
            std::cout << "Owner: " << hp_product->building->owner->name << std::endl;
        } else {
            std::cout << "No products!" << std::endl;
        }

    }), { "market_stats" }, "Get interesting stuff about the market" },
    { ([](World& world, Server& server, std::string cmdline) {
        uint i = 0;
        std::cout << "{" << std::endl;
        std::cout << "  \"provinces\": {" << std::endl;
        for(const auto& province: world.provinces) {
            std::cout << "    \"" << (size_t)province->color << "\":\"" << province->name << "\"";
            i++;
            if(i < world.provinces.size()) std::cout << ",";
            std::cout << std::endl;
        }
        std::cout << "  }," << std::endl;

        i = 0;
        std::cout << "  \"provinces_reverse\": {" << std::endl;
        for(const auto& province: world.provinces) {
            std::cout << "    \"" << province->name << "\":" << (size_t)province->color << "";
            i++;
            if(i < world.provinces.size()) std::cout << ",";
            std::cout << std::endl;
        }
        std::cout << "  }" << std::endl;
        std::cout << "}" << std::endl;
    }), { "cdcs" }, "Generate JSON CDCS dump" },
    { ([](World& world, Server& server, std::string cmdline) {
        for(const auto& province : world.provinces) {
            std::cout << "(" << province->name << ", " << province->name << ", " << (size_t)((province->total_pops() + 1) / 1000.f) << ")" << std::endl;
        }
    }), { "cdcs_legacy" }, "Generate legacy CDCS dump" },
    { ([](World& world, Server& server, std::string cmdline) {
        size_t count = 0;
        for(const auto& province : world.provinces) {
            count += province->total_pops();
        }
        std::cout << "World population: " << count << std::endl;
    }), { "popcount" }, "Display current world population" },
    { ([](World& world, Server& server, std::string cmdline) {
        for(const auto& province : world.provinces) {
            uint32_t color = bswap32(province->color) >> 8;
            uint8_t r, g, b;
            r = (color >> 16) & 0xff;
            g = (color >> 8) & 0xff;
            b = (color >> 0) & 0xff;

            std::cout << "    "
                << "{ "
                << "ref_name = \"" << province->ref_name << "\", "
                << "name = _(\"" << province->name << "\"), "
                << "color = rgb("
                << std::to_string(r) << ", " << std::to_string(g) << ", " << std::to_string(b) << ")"
                << " },"
                << std::endl;
        }
    }), { "lua_list" }, "Generate lua list" },
    { ([](World& world, Server& server, std::string cmdline) {
        std::cout << "digraph {" << std::endl;
        for(const auto& unit_type: world.unit_types) {
            std::cout << "  " << unit_type->ref_name << " [ label = \""
                << unit_type->name
                << "\", color = \"yellow\""
                << "];"
                << std::endl;
            

            for(const auto& good: unit_type->req_goods) {
                std::cout << "  " << good.first->ref_name << " -> " << unit_type->ref_name << ";" << std::endl;
            }
        }

        for(const auto& good: world.goods) {
            std::cout << "  " << good->ref_name << " [ label = \""
                << good->name
                << "\", color = \"blue\""
                << "];"
                << std::endl;
        }

        for(const auto& industry_type: world.building_types) {
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
    }), { "supply_chain_graph" }, "Generate a graphviz of the supply chain" },
};

#define _XOPEN_SOURCE 700
#include <cstdio>
#include <dirent.h>
#include <sys/types.h>
int main(int argc, char** argv) {
#if defined unix
    setlocale(LC_ALL, "");
    bindtextdomain("main", Path::get("locale").c_str());
    textdomain("main");
#endif

#if defined unix
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
#elif defined windows
    //Path::add_path("01_tutorial");
    Path::add_path("ie_client");
    Path::add_path("ie_core");
    Path::add_path("ie_map");
#endif

    std::srand(std::time(nullptr));

#if !defined UNIT_TEST
    try {
        // Run as a server for servicing multiple clients
        World* world = new World();
        world->load_mod();
        Server* server = new Server(1836);

        std::cout << "Type start to start the simulation or help to view all the commands" << std::endl;

        std::future<std::string> future = std::async(std::launch::async, async_get_input);
        run = true;

        bool paused = true;

        // Time between each tick
        int delta_ms = 2;
        while(run) {
            auto start_time = std::chrono::system_clock::now();
            if(!paused) {
                std::lock_guard lock(world_lock);
                world->do_tick();
            }

            if(future.wait_for(std::chrono::milliseconds(10)) == std::future_status::ready) {
                std::string r = future.get();

                for(const auto& cmd : cmds) {
                    for(const auto& alias: cmd.aliases) {
                        if(r == alias) {
                            cmd.invoke_cb(*world, *server, r);
                        }
                    }
                }

                if(r == "help" || r == "info") {
                    std::cout << "Builtin commands:" << std::endl;
                    std::cout << "start: Start the simulation" << std::endl;
                    std::cout << "stop: Pause the simulation" << std::endl;
                    std::cout << "resetp: Reset all POPs" << std::endl;
                    std::cout << "reload: Reload all lua files and assets" << std::endl;
                    std::cout << "quit: Quit the server" << std::endl;
                    
                    std::cout << "Dynamic commands:" << std::endl;
                    for(const auto& cmd : cmds) {
                        std::cout << "* ";
                        for(const auto& alias : cmd.aliases) {
                            std::cout << alias << " ";
                        }
                        std::cout << "- " << cmd.desc << std::endl;
                    }
                }
                else if(r == "start" || r == "unpause" || r == "ipl 1b9") {
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
                } else if(r == "exit" || r == "quit") {
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
            
            std::this_thread::sleep_until(start_time + std::chrono::milliseconds(delta_ms));
        }

        print_info(gettext("Destroying world"));
        delete world;
        print_info(gettext("Destroying server"));
        delete server;
    } catch(const LuaAPI::Exception& e) {
        luaL_traceback(g_world->lua, g_world->lua, e.what(), 0);
        print_error(lua_tostring(g_world->lua, -1));
        exit(EXIT_FAILURE);
    } catch(const std::exception& e) {
        print_error(e.what());
        exit(EXIT_FAILURE);
    }
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
    return 0;
}
#endif
