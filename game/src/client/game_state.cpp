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

#ifdef _MSC_VER
// Required before GL/gl.h
#   ifndef _WINDOWS_
#   define WIN32_LEAN_AND_MEAN 1
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

#include <cstdint>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>

#ifdef _MSC_VER
#   include <SDL.h>
#   include <SDL_events.h>
#   include <SDL_keycode.h>
#   include <SDL_mouse.h>
#   include <SDL_opengl.h>
#   include <SDL_ttf.h>
#   include <SDL_audio.h>
#else
#   include <SDL2/SDL.h>
#   include <SDL2/SDL_events.h>
#   include <SDL2/SDL_keycode.h>
#   include <SDL2/SDL_mouse.h>
#   include <SDL2/SDL_opengl.h>
#   include <SDL2/SDL_ttf.h>
#   include <SDL2/SDL_audio.h>
//#include <sys/wait.h>
#endif

#include "good.hpp"
#include "io_impl.hpp"
#include "unified_render/path.hpp"
#include "product.hpp"
#include "unified_render/serializer.hpp"
#include "world.hpp"
#include "unified_render/print.hpp"
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
#include "unified_render/material.hpp"
#include "unified_render/model.hpp"
#include "unified_render/texture.hpp"
#include "client/ui/ui.hpp"
#include "client/ui/input.hpp"
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
    Nation::Id id = world->get_id(&nation);
    return *map->nation_flags[id];
}

void handle_event(Input& input, GameState& gs) {
    std::pair<int, int>& mouse_pos = input.mouse_pos;
    UI::Context* ui_ctx = gs.ui_ctx;

    int& width = gs.width;
    int& height = gs.height;
    SDL_Event event;

    // Check window size every update
    //   - needed cause the window sometimes changes size without calling the change window size event
    SDL_GetWindowSize(gs.window, &width, &height);
    gs.ui_ctx->resize(width, height);
    gs.map->camera->set_screen(width, height);

    while(SDL_PollEvent(&event)) {
        bool click_on_ui = false;
        switch(event.type) {
        case SDL_CONTROLLERDEVICEADDED:
            break;
        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
            click_on_ui = ui_ctx->check_hover(mouse_pos.first, mouse_pos.second);
            ui_ctx->check_drag(mouse_pos.first, mouse_pos.second);
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
            ui_ctx->check_hover(mouse_pos.first, mouse_pos.second);
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
            switch(event.key.keysym.sym) {
            case SDLK_F1:
                if(gs.current_mode == MapMode::NORMAL) {
                    if(gs.profiler_view) {
                        delete gs.profiler_view;
                        gs.profiler_view = nullptr;
                    } else {
                        gs.profiler_view = new Interface::ProfilerView(gs);
                    }
                }
                break;
            case SDLK_SPACE:
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
            case SDLK_b:
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
            case SDLK_BACKSPACE:
                ui_ctx->check_text_input(nullptr);
                break;
            }
            break;
        case SDL_JOYAXISMOTION:
            ui_ctx->check_hover(gs.input.mouse_pos.first, gs.input.mouse_pos.second);
            break;
        case SDL_QUIT:
            gs.run = false;
            gs.paused = false;
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
    if(current_mode != MapMode::NO_MAP) {
        map->update_mapmode();
    }
}

void GameState::world_thread(void) {
    while(run) {
        while(paused) {
            if(!run) {
                return;
            }
        }
        world->do_tick();
        update_tick = true;

        std::this_thread::sleep_for(std::chrono::milliseconds(ms_delay_speed));
    }
}

#include "client/ui/image.hpp"
#include "client/interface/main_menu.hpp"
#include "unified_render/audio.hpp"
#include <filesystem>

void main_loop(GameState& gs) {
    gs.input = Input();
    gs.in_game = false;

    // Connect to server prompt
    gs.current_mode = MapMode::NO_MAP;
    //auto* mm_bg = new UI::Image(0, 0, gs.width, gs.height, &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/globe.png")));
    //mm_bg->is_fullscreen = true;
    /*Interface::MainMenu* main_menu =*/
    new Interface::MainMenu(gs);
    //auto* logo = new UI::Image(0, 0, 256, 256, &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/title_alt.png")));
    //logo->above_of(*main_menu);
    //logo->left_side_of(*main_menu);

    std::vector<Event*> displayed_events;
    std::vector<Treaty*> displayed_treaties;

    // Call update_on_tick on start of the gamestate
    gs.update_tick = true;
    gs.run = true;
    gs.paused = true;

    // Start the world thread
    std::thread world_th(&GameState::world_thread, &gs);
    while(gs.run) {
        handle_event(gs.input, gs);
        if(gs.current_mode == MapMode::NORMAL) {
            if(gs.world->world_mutex.try_lock()) {
                handle_popups(displayed_events, displayed_treaties, gs);
                gs.world->world_mutex.unlock();
            }
        }

        if(gs.sound_queue.empty()) {
            for(const auto& war : gs.world->wars) {
                for(const auto& battle : war->battles) {
                    std::scoped_lock lock(gs.sound_lock);
                    auto entries = Path::get_all_recursive("sfx/gunfire");
                    if(!entries.empty()) {
                        gs.sound_queue.push_back(new UnifiedRender::Audio(entries[std::rand() % entries.size()]));
                    }
                    break;
                }
                break;
            }
        }

        if(gs.update_tick) {
            gs.update_on_tick();
            gs.update_tick = false;

            if(gs.current_mode == MapMode::NORMAL) {
                if(gs.world->world_mutex.try_lock()) {
                    // Production queue
                    if(!gs.production_queue.empty()) {
                        for(unsigned int i = 0; i < gs.production_queue.size(); i++) {
                            UnitType* unit = gs.production_queue[i];

                            // TODO: Make a better queue AI
                            bool is_built = false;
                            for(const auto& building : gs.world->buildings) {
                                // Must be our building
                                if(building->get_owner() != gs.curr_nation) {
                                    continue;
                                }

                                // Must not be working on something else
                                if(building->working_unit_type != nullptr) {
                                    continue;
                                }

                                is_built = true;

                                g_client->send(Action::BuildingStartProducingUnit::form_packet(building, unit));
                                break;
                            }

                            if(!is_built) {
                                continue;
                            }

                            gs.production_queue.erase(gs.production_queue.begin() + i);
                            i--;
                        }
                    }
                    gs.world->world_mutex.unlock();
                }
            }

            if(gs.music_queue.empty()) {
                // Search through all the music in 'music/ambience' and picks a random
                auto entries = Path::get_all_recursive("music/ambience");
                if(entries.size() != 0) {
                    int music_index = std::rand() % entries.size();
                    std::scoped_lock lock(gs.sound_lock);
                    gs.music_fade_value = 100.f;
                    gs.music_queue.push_back(new UnifiedRender::Audio(entries[music_index]));
                }
            }
        }

        std::scoped_lock lock(gs.render_lock);

        if(!gs.motion_blur) {
            gs.clear();
        }

        if(gs.current_mode != MapMode::NO_MAP) {
            std::scoped_lock lock(gs.world->world_mutex);
            gs.map->draw(gs);
            gs.map->camera->update();
        }
        gs.ui_ctx->render_all();

        glUseProgram(0);
        glActiveTexture(GL_TEXTURE0);
        glViewport(0, 0, gs.width, gs.height);
        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.f, (float)gs.width, (float)gs.height, 0.f, 0.0f, 1.f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.f, 0.f, 0.f);
        // Cursor
        glPushMatrix();
        glTranslatef(gs.input.mouse_pos.first, gs.input.mouse_pos.second, 0.f);
        gs.tex_man->load(Path::get("ui/cursor.png")).bind();
        glColor3f(1.f, 1.f, 1.f);
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(0.f, 0.f);
        glTexCoord2f(1.f, 0.f);
        glVertex2f(32.f, 0.f);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(32.f, 32.f);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(32.f, 32.f);
        glTexCoord2f(0.f, 1.f);
        glVertex2f(0.f, 32.f);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(0.f, 0.f);
        glEnd();
        glPopMatrix();
        glPopMatrix();

        gs.swap();
        gs.world->profiler.render_done();
    }
    world_th.join();
}

#include "unified_render/locale.hpp"
void start_client(int, char**) {
    GameState gs{};

    if(1) {
        FILE* fp = fopen(Path::get("locale/ro/main.po").c_str(), "rt");
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
                print_info("%s=%s", key.c_str(), value.c_str());
            }
        }
        else {
            print_error("Cannot open locale file %s", Path::get("locale/ko/main.po").c_str());
        }
    }

    gs.ui_ctx = new UI::Context();

    gs.ui_ctx->resize(gs.width, gs.height);

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
