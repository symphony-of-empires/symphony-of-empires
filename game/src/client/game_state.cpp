// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      client/game_state.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/game_state.hpp"

#include <cstdint>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <filesystem>

#ifdef _MSC_VER
// Required before GL/gl.h
#   ifndef _WINDOWS_
#   	define WIN32_LEAN_AND_MEAN 1
#       ifndef NOMINMAX
#	        define NOMINMAX 1
#       endif
#       include <windows.h>
#       undef WIN32_LEAN_AND_MEAN
#   endif
#endif

#include <GL/glew.h>
// MSVC does not know about glext, mingw does so we just use this ifdef
#ifndef _MSC_VER
#   include <GL/glext.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef _MSC_VER
#   include <SDL.h>
#   include <SDL_events.h>
#   include <SDL_mouse.h>
#   include <SDL_audio.h>
#else
#   include <SDL2/SDL.h>
#   include <SDL2/SDL_events.h>
#   include <SDL2/SDL_mouse.h>
#   include <SDL2/SDL_audio.h>
//#include <sys/wait.h>
#endif

#include "unified_render/event.hpp"
#include "unified_render/path.hpp"
#include "unified_render/serializer.hpp"
#include "unified_render/print.hpp"
#include "unified_render/material.hpp"
#include "unified_render/model.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/ui/ui.hpp"
#include "unified_render/ui/input.hpp"
#include "unified_render/log.hpp"

#include "good.hpp"
#include "io_impl.hpp"
#include "product.hpp"
#include "world.hpp"
#include "client/camera.hpp"
#include "client/orbit_camera.hpp"
#include "client/client_network.hpp"
#include "client/interface/descision.hpp"
#include "client/interface/province_view.hpp"
#include "client/interface/lobby.hpp"
#include "client/interface/top_window.hpp"
#include "client/interface/province_view.hpp"
#include "client/interface/treaty.hpp"
#include "client/interface/map_dev_view.hpp"
#include "client/interface/army.hpp"
#include "client/interface/building.hpp"
#include "client/interface/minimap.hpp"
#include "client/interface/profiler_view.hpp"
#include "client/map.hpp"
#include "server/server_network.hpp"

void GameState::play_nation() {
    current_mode = MapMode::NORMAL;

    const Province* capital = curr_nation->capital;
    if(capital != nullptr) {
        map->camera->set_pos(capital->max_x, capital->max_y);
    }

    // Make topwindow
    top_win = new Interface::TopWindow(*this);
    // new MapDevView(map);
    minimap = new Interface::Minimap(*this, -400, -200, UI::Origin::LOWER_RIGHT_SCREEN);
    g_client->send(Action::SelectNation::form_packet(curr_nation));
    print_info("Selected nation [%s]", curr_nation->ref_name.c_str());
}

const UnifiedRender::Texture& GameState::get_nation_flag(Nation& nation) {
    Nation::Id id = world->get_id(nation);
    return *map->nation_flags[id];
}

void handle_event(Input& input, GameState& gs) {
    std::pair<int, int>& mouse_pos = input.mouse_pos;
    UI::Context* ui_ctx = gs.ui_ctx;

    int& width = gs.width;
    int& height = gs.height;

    // Check window size every update
    //   - needed cause the window sometimes changes size without calling the change window size event
    SDL_GetWindowSize(gs.window, &width, &height);
    gs.ui_ctx->resize(width, height);
    gs.map->camera->set_screen(width, height);

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        bool click_on_ui = false;
        switch(event.type) {
        case SDL_CONTROLLERDEVICEADDED:
            break;
        case SDL_MOUSEBUTTONDOWN:
            click_on_ui = ui_ctx->check_hover(mouse_pos.first, mouse_pos.second);
            if(event.button.button == SDL_BUTTON_LEFT) {
                SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
                ui_ctx->check_drag(mouse_pos.first, mouse_pos.second);
            }
            if(event.button.button == SDL_BUTTON_MIDDLE) {
                input.middle_mouse_down = true;
            }
            break;
        case SDL_JOYBUTTONDOWN:
            ui_ctx->check_drag(mouse_pos.first, mouse_pos.second);
            break;
        case SDL_MOUSEBUTTONUP:
            SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
            if(event.button.button == SDL_BUTTON_MIDDLE) {
                input.middle_mouse_down = false;
                break;
            }

            click_on_ui = ui_ctx->check_click(mouse_pos.first, mouse_pos.second);
            if(!click_on_ui && gs.current_mode != MapMode::NO_MAP) {
                gs.map->handle_click(gs, event);
            }

            if(click_on_ui) {
                std::scoped_lock lock(gs.sound_lock);
                auto entries = Path::get_all_recursive("sfx/click");
                if(!entries.empty()) {
                    gs.sound_queue.push_back(new UnifiedRender::Audio(entries[std::rand() % entries.size()]));
                }
            }
            break;
        case SDL_JOYBUTTONUP:
            click_on_ui = ui_ctx->check_click(mouse_pos.first, mouse_pos.second);
            if(!click_on_ui && gs.current_mode != MapMode::NO_MAP) {
                gs.map->handle_click(gs, event);
            }

            if(click_on_ui) {
                std::scoped_lock lock(gs.sound_lock);
                auto entries = Path::get_all_recursive("sfx/click");
                if(!entries.empty()) {
                    gs.sound_queue.push_back(new UnifiedRender::Audio(entries[std::rand() % entries.size()]));
                }
            }
            break;
        case SDL_MOUSEMOTION:
            SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
            click_on_ui = ui_ctx->check_hover(mouse_pos.first, mouse_pos.second);
            break;
        case SDL_MOUSEWHEEL:
            SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
            ui_ctx->check_hover(mouse_pos.first, mouse_pos.second);
            click_on_ui = ui_ctx->check_wheel(mouse_pos.first, mouse_pos.second, event.wheel.y * 6);
            break;
        case SDL_TEXTINPUT:
            ui_ctx->check_text_input((const char*)&event.text.text);
            break;
        case SDL_KEYDOWN:
            switch(UnifiedRender::Keyboard::from_sdlk(event.key.keysym.sym)) {
            case UnifiedRender::Keyboard::Key::F1:
                if(gs.current_mode == MapMode::NORMAL) {
                    if(gs.profiler_view) {
                        delete gs.profiler_view;
                        gs.profiler_view = nullptr;
                    }
                    else {
                        gs.profiler_view = new Interface::ProfilerView(gs);
                    }
                }
                break;
            case UnifiedRender::Keyboard::Key::SPACE:
                if(gs.editor) {
                    break;
                }

                if(gs.current_mode == MapMode::NORMAL) {
                    gs.paused = !gs.paused;
                    if(gs.paused) {
                        ui_ctx->prompt("Control", "Unpaused");
                    }
                    else {
                        ui_ctx->prompt("Control", "Paused");
                    }
                }
                break;
            case UnifiedRender::Keyboard::Key::B:
                if(gs.editor) {
                    break;
                }

                if(gs.current_mode == MapMode::NORMAL) {
                    if(input.select_pos.first < gs.world->width || input.select_pos.second < gs.world->height) {
                        const Tile& tile = gs.world->get_tile(input.select_pos.first, input.select_pos.second);
                        if(tile.province_id >= gs.world->provinces.size()) {
                            break;
                        }

                        new Interface::BuildingBuildView(gs, input.select_pos.first, input.select_pos.second, true, gs.world->provinces[tile.province_id]->owner, gs.world->provinces[tile.province_id]);
                    }
                }
                break;
            case UnifiedRender::Keyboard::Key::BACKSPACE:
                ui_ctx->check_text_input(nullptr);
                break;
            default:
                break;
            }
            break;
        case SDL_JOYAXISMOTION:
            ui_ctx->check_hover(gs.input.mouse_pos.first, gs.input.mouse_pos.second);
            break;
        case SDL_QUIT:
            gs.run = false;
            break;
        default:
            break;
        }

        if(gs.current_mode != MapMode::NO_MAP && !click_on_ui) {
            gs.map->update(event, input, ui_ctx);
        }
    }
    ui_ctx->clear_dead();
}

void GameState::send_command(Archive& archive) {
    std::scoped_lock lock(client->pending_packets_mutex);

    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet(g_client->get_fd());
    packet.data(archive.get_buffer(), archive.size());
    client->pending_packets.push_back(packet);
}

#include "unified_render/byteswap.hpp"
void save(GameState& gs) {
    if(gs.editor) {
        std::filesystem::create_directory("editor");
        std::filesystem::create_directory("editor/lua");
        std::filesystem::create_directory("editor/lua/entities");
        std::filesystem::create_directory("editor/map");
        FILE* fp;
        size_t cnt;

        // Provinces
        fp = fopen("editor/lua/entities/provinces.lua", "wt");
        if(fp == nullptr) {
            gs.ui_ctx->prompt("Save", "Can't output provinces data!");
            return;
        }
        cnt = 0;
        fprintf(fp, "-- Generated by editor :)\n");
        for(const auto& province : gs.world->provinces) {
            // Remove pops with 0 size and ones that are redundant/duplicated
            for(std::vector<Pop>::iterator it = province->pops.begin(); it != province->pops.end(); ) {
                // If the POP is zero then delete it
                if(!it->size) {
                    province->pops.erase(it);
                    it = province->pops.begin();
                    continue;
                }

                // Or if it's duplicate, merge it
                auto dup_it = std::find_if(province->pops.begin(), province->pops.end(), [&gs, &it](const auto& e) {
                    return (
                        gs.world->get_id(*it->culture) == gs.world->get_id(*e.culture)
                        && gs.world->get_id(*it->religion) == gs.world->get_id(*e.religion)
                        && gs.world->get_id(*it->type) == gs.world->get_id(*e.type)
                    );
                });

                if(dup_it != province->pops.end() && dup_it != it) {
                    it->budget += dup_it->budget;
                    it->size += dup_it->size;
                    province->pops.erase(dup_it);
                    it = province->pops.begin();
                    continue;
                }
                it++;
            }

            const uint32_t color = bswap32((province->color & 0x00ffffff) << 8);
            fprintf(fp, "province = Province:new{ ref_name = \"%s\", name = _(\"%s\"), color = 0x%x, terrain = TerrainType:get(\"%s\") }\n", province->ref_name.c_str(), province->name.c_str(), (unsigned int)color, province->terrain_type->ref_name.c_str());
            fprintf(fp, "province:register()\n");
            for(const auto& building_type : gs.world->building_types) {
                Building& building = province->buildings[gs.world->get_id(*building_type)];
                if(building.level) {
                    building.level = 1;
                    fprintf(fp, "province:update_building(BuildingType:get(\"%s\"), %f)\n", building_type->ref_name.c_str(), building.level);
                }
            }

            for(const auto& pop : province->pops) {
                fprintf(fp, "province:add_pop(PopType:get(\"%s\"), Culture:get(\"%s\"), Religion:get(\"%s\"), %zu, %f)\n", pop.type->ref_name.c_str(), pop.culture->ref_name.c_str(), pop.religion->ref_name.c_str(), pop.size, pop.literacy);
            }

            for(const auto& nucleus : province->nuclei) {
                fprintf(fp, "province:add_nucleus(Nation:get(\"%s\"))\n", nucleus->ref_name.c_str());
            }

            if(province->owner != nullptr) {
                fprintf(fp, "province:give_to(Nation:get(\"%s\"))\n", province->owner->ref_name.c_str());
                if(province->owner->capital == province) {
                    fprintf(fp, "Nation:get(\"%s\"):set_capital(province)\n", province->owner->ref_name.c_str());
                }
            }
            cnt++;
        }
        if(!cnt) {
            fprintf(fp, "-- No provinces, what the fuck?\n");
        }
        fclose(fp);

        // Terrain types
        fp = fopen("editor/lua/entities/terrain_types.lua", "wt");
        if(fp == nullptr) {
            gs.ui_ctx->prompt("Save", "Can't output terrain_types data!");
            return;
        }
        cnt = 0;
        fprintf(fp, "-- Generated by editor :)\n");
        for(const auto& terrain_type : gs.world->terrain_types) {
            const uint32_t color = bswap32((terrain_type->color & 0x00ffffff) << 8);
            fprintf(fp, "TerrainType:new{ ref_name = \"%s\", name = _(\"%s\"), color = 0x%x, is_water_body = %s }:register()\n", terrain_type->ref_name.c_str(), terrain_type->name.c_str(), (unsigned int)color, terrain_type->is_water_body ? "true" : "false");
            cnt++;
        }
        if(!cnt) {
            fprintf(fp, "-- No terrain types, so it's all void now?\n");
        }
        fclose(fp);

        // Religions
        fp = fopen("editor/lua/entities/religions.lua", "wt");
        if(fp == nullptr) {
            gs.ui_ctx->prompt("Save", "Can't output religions data!");
            return;
        }
        cnt = 0;
        fprintf(fp, "-- Generated by editor :)\n");
        for(const auto& religion : gs.world->religions) {
            const uint32_t color = bswap32((religion->color & 0x00ffffff) << 8);
            fprintf(fp, "Religion:new{ ref_name = \"%s\", name = _(\"%s\"), color = 0x%x }:register()\n", religion->ref_name.c_str(), religion->name.c_str(), (unsigned int)color);
            cnt++;
        }
        if(!cnt) {
            fprintf(fp, "-- no religions! :)\n");
        }
        fclose(fp);

        // Pop types
        fp = fopen("editor/lua/entities/pop_types.lua", "wt");
        if(fp == nullptr) {
            gs.ui_ctx->prompt("Save", "Can't output pop_types data!");
            return;
        }
        cnt = 0;
        fprintf(fp, "-- Generated by editor :)\n");
        for(const auto& pop_type : gs.world->pop_types) {
            if(pop_type->group == PopGroup::BURGEOISE) {
                fprintf(fp, "PopType:new{ ref_name = \"%s\", name = _(\"%s\"), social_value = %f, is_burgeoise = true }\n", pop_type->ref_name.c_str(), pop_type->name.c_str(), pop_type->social_value);
            } else if(pop_type->group == PopGroup::FARMER) {
                fprintf(fp, "PopType:new{ ref_name = \"%s\", name = _(\"%s\"), social_value = %f, is_farmer = true }\n", pop_type->ref_name.c_str(), pop_type->name.c_str(), pop_type->social_value);
            } else if(pop_type->group == PopGroup::LABORER) {
                fprintf(fp, "PopType:new{ ref_name = \"%s\", name = _(\"%s\"), social_value = %f, is_laborer = true }\n", pop_type->ref_name.c_str(), pop_type->name.c_str(), pop_type->social_value);
            } else if(pop_type->group == PopGroup::Slave) {
                fprintf(fp, "PopType:new{ ref_name = \"%s\", name = _(\"%s\"), social_value = %f, is_slave = true }\n", pop_type->ref_name.c_str(), pop_type->name.c_str(), pop_type->social_value);
            } else {
                fprintf(fp, "PopType:new{ ref_name = \"%s\", name = _(\"%s\"), social_value = %f }\n", pop_type->ref_name.c_str(), pop_type->name.c_str(), pop_type->social_value);
            }
            cnt++;
        }
        if(!cnt) {
            fprintf(fp, "-- No pop classes? social inequalities solved!\n");
        }
        fclose(fp);

        // Unit types
        fp = fopen("editor/lua/entities/unit_types.lua", "wt");
        if(fp == nullptr) {
            gs.ui_ctx->prompt("Save", "Can't output unit_types data!");
            return;
        }
        cnt = 0;
        fprintf(fp, "-- Generated by editor :)\n");
        fprintf(fp, "local v = {}\n");
        for(const auto& unit_type : gs.world->unit_types) {
            if(unit_type->is_ground == true && unit_type->is_naval == false) {
                fprintf(fp, "v = UnitType:new{ ref_name = \"%s\", name = _(\"%s\"), defense = %f, attack = %f, health = %f, speed = %f }\n", unit_type->ref_name.c_str(), unit_type->name.c_str(), unit_type->defense, unit_type->attack, unit_type->max_health, unit_type->speed);
                fprintf(fp, "v:register()\n");
                for(const auto& good : unit_type->req_goods) {
                    fprintf(fp, "v:requires_good(Good:get(\"%s\"), %f)\n", good.first->ref_name.c_str(), good.second);
                }
                cnt++;
            }
        }
        if(!cnt) {
            fprintf(fp, "-- No unit types! no more war! :D\n");
        }
        fclose(fp);

        // Boat types
        fp = fopen("editor/lua/entities/boat_types.lua", "wt");
        if(fp == nullptr) {
            gs.ui_ctx->prompt("Save", "Can't output boat_types data!");
            return;
        }
        cnt = 0;
        fprintf(fp, "-- Generated by editor :)\n");
        fprintf(fp, "local v = {}\n");
        for(const auto& unit_type : gs.world->unit_types) {
            if(unit_type->is_ground == false && unit_type->is_naval == true) {
                fprintf(fp, "v = BoatType:new{ ref_name = \"%s\", name = _(\"%s\"), defense = %f, attack = %f, health = %f, speed = %f }\n", unit_type->ref_name.c_str(), unit_type->name.c_str(), unit_type->defense, unit_type->attack, unit_type->max_health, unit_type->speed);
                fprintf(fp, "v:register()\n");
                for(const auto& good : unit_type->req_goods) {
                    fprintf(fp, "v:requires_good(Good:get(\"%s\"), %f)\n", good.first->ref_name.c_str(), good.second);
                }
                cnt++;
            }
        }
        if(!cnt) {
            fprintf(fp, "-- No boats? probably there's no ocean either...\n");
        }
        fclose(fp);

        // Airplane types
        fp = fopen("editor/lua/entities/airplane_types.lua", "wt");
        if(fp == nullptr) {
            gs.ui_ctx->prompt("Save", "Can't output airplane_types data!");
            return;
        }
        cnt = 0;
        fprintf(fp, "-- Generated by editor :)\n");
        fprintf(fp, "local v = {}\n");
        for(const auto& unit_type : gs.world->unit_types) {
            if(unit_type->is_ground == true && unit_type->is_naval == true) {
                fprintf(fp, "v = AirplaneType:new{ ref_name = \"%s\", name = _(\"%s\"), defense = %f, attack = %f, health = %f, speed = %f }\n", unit_type->ref_name.c_str(), unit_type->name.c_str(), unit_type->defense, unit_type->attack, unit_type->max_health, unit_type->speed);
                fprintf(fp, "v:register()\n");
                for(const auto& good : unit_type->req_goods) {
                    fprintf(fp, "v:requires_good(Good:get(\"%s\"), %f)\n", good.first->ref_name.c_str(), good.second);
                }
                cnt++;
            }
        }
        if(!cnt) {
            fprintf(fp, "-- So many jokes i could put here but I would rather list the recipe for baking a cake :)\n");
#if defined windows
            fprintf(fp, "-- 1. buy cake from store\n");
            fprintf(fp, "-- 2. done\n");
#endif
        }
        fclose(fp);

        // Good types
        fp = fopen("editor/lua/entities/good_types.lua", "wt");
        if(fp == nullptr) {
            gs.ui_ctx->prompt("Save", "Can't output unit_types data!");
            return;
        }
        fprintf(fp, "-- Generated by editor :)\n");
        for(const auto& good_type : gs.world->goods) {
            fprintf(fp, "GoodType:new{ ref_name = \"%s\", name = _(\"%s\") }:register()\n", good_type->ref_name.c_str(), good_type->name.c_str());
        }
        if(!cnt) {
#if defined windows
            fprintf(fp, "-- Economy.exe has stopped working\n");
#else
            fprintf(fp, "-- The stock market has been /dev/null -ed :)\n");
#endif
        }
        fclose(fp);

        gs.ui_ctx->prompt("Save", "Editor data saved! (check editor folder)");
    } else {
        Archive ar = Archive();
        ::serialize(ar, gs.world);
        ar.to_file("default.scv");
        gs.ui_ctx->prompt("Save", "Saved sucessfully!");
    }
}

void handle_popups(std::vector<Event*>& displayed_events, std::vector<Treaty*>& displayed_treaties, GameState& gs) {
    // Put popups
    // Event + Descision popups
    for(auto& msg : gs.curr_nation->inbox) {
        // Check that the event is not already displayed to the user
        auto iter = std::find_if(displayed_events.begin(), displayed_events.end(), [&msg](const auto& e) { return e->ref_name == msg->ref_name; });
        if(iter != displayed_events.end()) {
            continue;
        }

        new Interface::DescisionWindow(gs, *msg);
        displayed_events.push_back(msg);
    }

    for(auto& treaty : gs.world->treaties) {
        // Check that the treaty is not already displayed
        auto iter = std::find_if(displayed_treaties.begin(), displayed_treaties.end(), [&treaty](const auto& e) { return e == treaty; });
        if(iter != displayed_treaties.end()) {
            continue;
        }

        // Do not mess with treaties we don't partake in, hehe
        if(!treaty->does_participate(*gs.curr_nation)) {
            continue;
        }

        // Must participate in treaty
        new Interface::TreatyChooseWindow(gs, treaty);
        displayed_treaties.push_back(treaty);
    }
}

void GameState::update_on_tick(void) {
    ui_ctx->do_tick();

    // TODO: This is inefficient and we should only update **when** needed
    if(current_mode != MapMode::NO_MAP) {
        map->update_mapmode();
        map->create_labels();
    }
}

void GameState::world_thread(void) {
    while(run) {
        // Gamestate thread hasn't acknowledged the updated tick just yet
        while(paused) {
            if(!run) {
                return;
            }
        }

        // TODO: We should only enable this on debug builds, but whatever...
        if(world->world_mutex.try_lock()) {
            try {
                world->do_tick();
                update_tick = true;
                world->world_mutex.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(ms_delay_speed));
            } catch(const std::exception& e) {
                world->world_mutex.unlock();
                std::scoped_lock lock(render_lock);
                ui_ctx->prompt("Runtime exception", e.what());
                UnifiedRender::Log::error("game", e.what());
                paused = true;
            }
        }
    }
}

#include <filesystem>

#include "unified_render/ui/image.hpp"
#include "unified_render/audio.hpp"

#include "client/interface/main_menu.hpp"

void main_loop(GameState& gs) {
    gs.input = Input();
    gs.in_game = false;

    // Connect to server prompt
    gs.current_mode = MapMode::DISPLAY_ONLY;
    gs.map->set_view(MapView::SPHERE_VIEW);

    new Interface::MainMenu(gs);

    std::vector<Event*> displayed_events;
    std::vector<Treaty*> displayed_treaties;

    // Call update_on_tick on start of the gamestate
    gs.update_tick = true;
    gs.run = true;
    gs.paused = true;

    gs.map->camera->move(0.f, 50.f, 10.f);

    // Start the world thread
    std::thread world_th(&GameState::world_thread, &gs);
    while(gs.run) {
        handle_event(gs.input, gs);
        if(gs.current_mode == MapMode::NORMAL && gs.world->world_mutex.try_lock()) {
            handle_popups(displayed_events, displayed_treaties, gs);
            gs.world->world_mutex.unlock();
        } else if(gs.current_mode == MapMode::DISPLAY_ONLY) {
            gs.map->camera->move(0.05f, 0.f, 0.f);
        }

        if(gs.sound_queue.empty()) {
            for(const auto& war : gs.world->wars) {
                for(const auto& battle : war->battles) {
                    const std::scoped_lock lock(gs.sound_lock);
                    auto entries = Path::get_all_recursive("sfx/gunfire");
                    if(!entries.empty()) {
                        gs.sound_queue.push_back(new UnifiedRender::Audio(entries[std::rand() % entries.size()]));
                    }
                    break;
                }
                break;
            }
        }

        if(gs.update_tick && gs.world->world_mutex.try_lock()) {
            gs.update_on_tick();
            gs.update_tick = false;

            if(gs.current_mode == MapMode::NORMAL) {
                // Production queue
                for(unsigned int i = 0; i < gs.production_queue.size(); i++) {
                    UnitType* unit_type = gs.production_queue[i];

                    // TODO: Make a better queue AI
                    bool is_built = false;
                    for(const auto& building_type : gs.world->building_types) {
                        for(const auto& province : gs.curr_nation->controlled_provinces) {
                            Building& building = province->get_buildings()[gs.world->get_id(*building_type)];
                            // Must not be working on something else
                            if(building.working_unit_type != nullptr) {
                                continue;
                            }

                            is_built = true;
                            g_client->send(Action::BuildingStartProducingUnit::form_packet(province, building_type, gs.curr_nation, unit_type));
                            break;
                        }

                        if(!is_built) {
                            break;
                        }
                    }
                    if(!is_built) {
                        break;
                    }

                    gs.production_queue.erase(gs.production_queue.begin() + i);
                    i--;
                }
            }
            gs.world->world_mutex.unlock();
        }

        if(gs.music_queue.empty()) {
            // Search through all the music in 'music/ambience' and picks a random
            auto entries = Path::get_all_recursive("sfx/music/ambience");
            if(!entries.empty()) {
                int music_index = std::rand() % entries.size();
                gs.music_fade_value = 100.f;
                std::scoped_lock lock(gs.sound_lock);
                gs.music_queue.push_back(new UnifiedRender::Audio(entries[music_index]));
            }
        }

        std::scoped_lock lock(gs.render_lock);
        if(!gs.motion_blur) {
            glClear(GL_COLOR_BUFFER_BIT);
        }
        gs.clear();

        if(gs.current_mode != MapMode::NO_MAP) {
            std::scoped_lock lock(gs.world->world_mutex);
            gs.map->draw(gs);
        }

        if(gs.current_mode != MapMode::NO_MAP) {
            gs.map->camera->update();
        }

        gs.ui_ctx->render_all(glm::ivec2(gs.input.mouse_pos.first, gs.input.mouse_pos.second));
        gs.swap();
        gs.world->profiler.render_done();
    }
    world_th.join();
}

#include "unified_render/locale.hpp"
void start_client(int, char**) {
    GameState gs{};

    if(1) {
        FILE* fp = fopen(Path::get("locale/ko/main.po").c_str(), "rt");
        if(fp != nullptr) {
            char* tmp = new char[1000];
            while(fgets(tmp, 1000, fp) != nullptr) {
                if(!strncmp(tmp, "msgid", 5)) {
                    char* msgid = new char[100];
                    sscanf(tmp + 5, " %*c%[^\"]s%*c ", msgid);
                    fgets(tmp, 1000, fp);
                    if(!strncmp(tmp, "msgstr", 6)) {
                        char* msgstr = new char[100];
                        sscanf(tmp + 6, " %*c%[^\"]s%*c ", msgstr);
                        trans_msg[msgid] = std::string(msgstr);
                        delete[] msgstr;
                    }
                    delete[] msgid;
                }
            }
            delete[] tmp;
            fclose(fp);

            for(const auto& [key, value] : trans_msg) {
                UnifiedRender::Log::debug("trans", key + "=" + value);
            }
        } else {
            UnifiedRender::Log::error("trans", "Cannot open locale file " + Path::get("locale/ko/main.po"));
        }
    }

    gs.loaded_world = false;
    gs.loaded_map = false;

    gs.world = new World();
    gs.world->load_initial();
    gs.world->load_mod();
    gs.map = new Map(*gs.world, gs.width, gs.height);

    main_loop(gs);
    return;
}

GameState::~GameState() {
    // delete world;
    // delete curr_nation;
    // delete map;
    // delete ui_ctx;
};
