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
#   ifndef WINSOCK2_IMPORTED
#       define WINSOCK2_IMPORTED
#       include <winsock2.h>
#   endif
#       include <windows.h>
#   endif
#endif

#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
// MSVC does not know about glext, mingw does so we just use this ifdef
#   ifndef _MSC_VER
#       include <GL/glext.h>
#   endif
#   include <GL/gl.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl3ext.h>
#   include <GLES3/gl3.h>
#endif

#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/input.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/text.hpp"
#include "eng3d/ui/progress_bar.hpp"
#include "eng3d/audio.hpp"
#include "eng3d/locale.hpp"
#include "eng3d/event.hpp"
#include "eng3d/path.hpp"
#include "eng3d/serializer.hpp"
#include "eng3d/material.hpp"
#include "eng3d/model.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/log.hpp"
#include "eng3d/camera.hpp"
#include "eng3d/interface.hpp"

#include "client/game_state.hpp"
#include "good.hpp"
#include "io_impl.hpp"
#include "product.hpp"
#include "world.hpp"
#include "client/client_network.hpp"
#include "client/interface/decision.hpp"
#include "client/interface/lobby.hpp"
#include "client/interface/top_window.hpp"
#include "client/interface/treaty.hpp"
#include "client/interface/army.hpp"
#include "client/interface/building.hpp"
#include "client/interface/minimap.hpp"
#include "client/interface/map_debug.hpp"
#include "client/map.hpp"
#include "client/map_render.hpp"
#include "server/server_network.hpp"
#include "server/export_util.hpp"
#include "client/interface/main_menu.hpp"

void GameState::play_nation() {
    current_mode = MapMode::NORMAL;

    const auto& capital = this->world->provinces[this->curr_nation->capital_id];
    map->camera->set_pos(capital.box_area.right, capital.box_area.bottom);
    map->map_render->request_update_visibility();
    map->map_render->update(*this);

    // Make topwindow
    top_win = static_cast<UI::Widget*>(new Interface::TopWindow(*this));
    minimap = static_cast<UI::Widget*>(new Interface::Minimap(*this, -400, -200, UI::Origin::LOWER_RIGHT_SCREEN));
    Eng3D::Log::debug("game", "Selecting nation " + this->curr_nation->ref_name);
    this->client->send(Action::SelectNation::form_packet(*this->curr_nation));

    // Set AI to all off
    this->curr_nation->ai_controlled = false;
    this->curr_nation->ai_do_cmd_troops = false;
    this->curr_nation->ai_controlled = false;
    this->curr_nation->ai_controlled = false;
    this->curr_nation->ai_controlled = false;
    this->curr_nation->ai_controlled = false;
    this->curr_nation->ai_controlled = false;
    this->curr_nation->ai_controlled = false;
    this->client->send(Action::AiControl::form_packet(*this->curr_nation));
}

std::shared_ptr<Eng3D::Texture> GameState::get_nation_flag(const Nation& nation) {
    std::string path = "gfx/flags/" + nation.ref_name + "_" + (nation.ideology == nullptr ? "none" : nation.ideology->ref_name.get_string()) + ".png";
    return this->tex_man.load(this->package_man.get_unique(path));
}

void handle_event(GameState& gs) {
    gs.do_event();
    const std::scoped_lock lock(gs.ui_ctx.prompt_queue_mutex);
    for(const auto& prompt : gs.ui_ctx.prompt_queue) {
        auto* win = new UI::Window(0, 0, 512, 512);
        win->origin = UI::Origin::CENTER_SCREEN;
        win->text(prompt.first);
        win->is_scroll = true;
        win->set_close_btn_function([win](UI::Widget&) {
            win->kill();
        });
        auto* txt = new UI::Text(0, 0, win->width, win->height, win);
        txt->text(prompt.second);
        txt->is_scroll = true;
        win->height = txt->y + txt->height;
    }
    gs.ui_ctx.prompt_queue.clear();
    gs.ui_ctx.clear_dead();
}

void GameState::send_command(Archive& archive) {
    std::scoped_lock lock(client->pending_packets_mutex);
    Eng3D::Networking::Packet packet(this->client->get_fd(), archive.get_buffer(), archive.size());
    client->pending_packets.push_back(packet);
}

void handle_popups(std::vector<Treaty::Id>& displayed_treaties, GameState& gs) {
    // Put popups
    // Event + Decision popups
    for(auto& msg : gs.curr_nation->inbox)
        // Check that the event is not already displayed to the user
        new Interface::DecisionWindow(gs, msg);
    gs.curr_nation->inbox.clear(); // Clear the inbox

    for(auto& treaty : gs.world->treaties) {
        // Check that the treaty is not already displayed
        auto iter = std::find_if(displayed_treaties.begin(), displayed_treaties.end(), [&treaty](const auto& e) { return e == treaty.get_id(); });
        if(iter != displayed_treaties.end()) continue;
        // Do not mess with treaties we don't partake in, hehe
        if(!treaty.does_participate(*gs.curr_nation)) continue;
        // Must participate in treaty
        new Interface::TreatyChooseWindow(gs, treaty.get_id());
        displayed_treaties.push_back(treaty.get_id());
    }
}

void GameState::update_on_tick() {
    world->profiler.start("UI_TICK");
    ui_ctx.do_tick();
    world->profiler.stop("UI_TICK");

    /// @todo This is inefficient and we should only update **when** needed
    if(current_mode != MapMode::NO_MAP) {
        map->update_mapmode();
        //map->create_labels();
    }
}

/// @todo Don't run this thread if not needed (i.e non-host-mode)
void GameState::world_thread() {
    this->paused = true;
    while(this->run) {
        // Gamestate thread hasn't acknowledged the updated tick just yet
        while(this->paused) {
            if(!this->run) {
                Eng3D::Log::debug("world_thread", "Unpaused game");
                break;
            }
        }
        if(!this->run) break;

        // Only run the economy simulation of host mode is ON, otherwise don't :-)
        const auto delta = std::chrono::milliseconds{ ms_delay_speed };
        if(host_mode) {
            const auto start_time = std::chrono::system_clock::now();
            Eng3D::Log::debug("world_thread", "World tick performed!");
            try {
                world->do_tick();
                update_tick = true;
            } catch(const std::exception& e) {
                std::scoped_lock lock(render_lock);
                ui_ctx.prompt("Runtime exception", e.what());
                Eng3D::Log::error("world_thread", e.what());
                paused = true;
            }

            // Wait until time delta is fullfilled and update_tick is false
            auto end_time = std::chrono::system_clock::now();
            while(end_time - start_time < delta)
                end_time = std::chrono::system_clock::now();
        }
    }
}

void GameState::music_enqueue() {
    const std::scoped_lock lock(this->audio_man.sound_lock);
    if(this->audio_man.music_queue.empty()) {
        auto entries = this->package_man.get_multiple_prefix("sfx/music/ambience");
        this->audio_man.music_fade_value = 0.f;
        // Search through all the music in 'music/ambience' and picks a random
        if(!entries.empty()) {
            const int music_index = rand() % entries.size();
            auto audio = this->audio_man.load(entries[music_index]->get_abs_path());
            this->audio_man.music_queue.push_back(audio);
            Eng3D::Log::debug("music", "Now playing music file " + entries[music_index]->get_abs_path());
        }
    }
}

void GameState::load_world_thread() {
    this->world = &World::get_instance();
    this->world->init_lua();
    this->world->load_initial();
    this->load_progress = 0.0f;
    /// @todo Events can't be properly reloaded right now :/
    this->world->events.clear();
    this->world->load_mod();
    this->load_progress = 0.1f;
    this->loaded_world = true;
}

void GameState::handle_resize() {
    if(map != nullptr) map->camera->set_screen(width, height);
}

void GameState::handle_mouse_btn(const Eng3D::Event::MouseButton& e) {
    if(e.hold) {
        if(show_ui) {
            if(ui_ctx.check_hover(input.mouse_pos)) {
                if(e.type == Eng3D::Event::MouseButton::Type::LEFT) {
                    input.mouse_pos = Eng3D::Event::get_mouse_pos();
                    ui_ctx.check_drag(input.mouse_pos);
                }
                return;
            }
        }

        if(current_mode != MapMode::NO_MAP)
            map->handle_mouse_button(e);
        
        if(e.type == Eng3D::Event::MouseButton::Type::MIDDLE)
            input.middle_mouse_down = true;
    } else {
        input.mouse_pos = Eng3D::Event::get_mouse_pos();
        if(e.type == Eng3D::Event::MouseButton::Type::LEFT || e.type == Eng3D::Event::MouseButton::Type::RIGHT) {
            if(show_ui) {
                if(ui_ctx.check_click(input.mouse_pos)) {
                    const std::scoped_lock lock(audio_man.sound_lock);
                    auto entries = package_man.get_multiple_prefix("sfx/click");
                    if(!entries.empty()) {
                        auto audio = audio_man.load(entries[rand() % entries.size()]->get_abs_path());
                        audio_man.sound_queue.push_back(audio);
                    }
                    return;
                }
            }

            if(current_mode != MapMode::NO_MAP) // Map
                map->handle_mouse_button(e);
        } else if(e.type == Eng3D::Event::MouseButton::Type::MIDDLE) {
            input.middle_mouse_down = false;
        }
    }
}

void GameState::handle_mouse_motion(const Eng3D::Event::MouseMotion& e) {
    input.mouse_pos = e.pos;
    if(show_ui)
        if(ui_ctx.check_hover(input.mouse_pos))
            return;

    if(current_mode != MapMode::NO_MAP)
        map->handle_mouse_motions(e);
}

void GameState::handle_mouse_wheel(const Eng3D::Event::MouseWheel& e) {
    input.mouse_pos = Eng3D::Event::get_mouse_pos();
    if(show_ui) {
        ui_ctx.check_hover(input.mouse_pos);
        if(ui_ctx.check_wheel(input.mouse_pos, e.wheel.y * 6)) return;
    }

    if(current_mode != MapMode::NO_MAP)
        map->camera->move(0.f, 0.f, -e.wheel.y * delta_time * 120.f);
}

void GameState::handle_key(const Eng3D::Event::Key& e) {
    if(e.hold) {
        switch(e.type) {
        case Eng3D::Event::Key::Type::F1:
            show_ui = !show_ui;
            break;
        case Eng3D::Event::Key::Type::F2:
            if(current_mode == MapMode::NORMAL) {
                if(profiler_view)
                    profiler_view->kill();
                else
                    profiler_view = new Eng3D::Interface::ProfilerView(*this, this->world->profiler);
            }
            break;
        case Eng3D::Event::Key::Type::F3:
            if(editor) break;

            if(current_mode == MapMode::NORMAL) {
                if(input.select_pos.x < world->width || input.select_pos.y < world->height) {
                    const auto tile = world->get_tile(input.select_pos.x, input.select_pos.y);
                    if(tile.province_id >= world->provinces.size()) break;
                    new Interface::BuildingBuildView(*this, input.select_pos.x, input.select_pos.y, true, world->provinces[tile.province_id]);
                }
            }
            break;
        case Eng3D::Event::Key::Type::F4:
            LuaAPI::invoke_registered_callback(world->lua, "ai_settings_window_invoke");
            break;
        case Eng3D::Event::Key::Type::F5:
            if(editor) break;
            if(current_mode == MapMode::NORMAL) {
                paused = !paused;
                if(paused)
                    ui_ctx.prompt("Control", "Unpaused");
                else
                    ui_ctx.prompt("Control", "Paused");
            }
            break;
        case Eng3D::Event::Key::Type::F6: {
            reload_shaders();
            // Shader used for drawing the models using custom model render
            map->obj_shader = std::make_unique<Eng3D::OpenGL::Program>();
            map->obj_shader->attach_shader(*builtin_shaders["vs_3d"].get());
            map->obj_shader->attach_shader(*builtin_shaders["fs_3d"].get());
            map->obj_shader->link();

            const std::scoped_lock lock(audio_man.sound_lock);
            audio_man.music_queue.clear();

            ui_ctx.prompt("Debug", "Partial reload");
        } break;
        case Eng3D::Event::Key::Type::BACKSPACE:
            ui_ctx.check_text_input(nullptr);
            break;
        case Eng3D::Event::Key::Type::UP:
            map->camera->move(0.f, -1.f, 0.f);
            break;
        case Eng3D::Event::Key::Type::DOWN:
            map->camera->move(0.f, 1.f, 0.f);
            break;
        case Eng3D::Event::Key::Type::LEFT:
            map->camera->move(-1.f, 0.f, 0.f);
            break;
        case Eng3D::Event::Key::Type::RIGHT:
            map->camera->move(1.f, 0.f, 0.f);
            break;
        default: break;
        }
    }
}

void start_client(int argc, char** argv) {
    std::vector<std::string> pkg_paths;
    for(int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if(arg == "--mod") {
            i++;
            if(i >= argc)
                CXX_THROW(std::runtime_error, "Expected an absolute path after --mod");
            arg = std::string(argv[i]);
            pkg_paths.push_back(arg);
        }
    }

    GameState gs(pkg_paths);

    // After loading everything initialize the gamestate initial properties
    // Call update_on_tick on start of the gamestate
    gs.update_tick = true;
    gs.in_game = false;
    gs.input = Input();
    gs.run = true;
    gs.loaded_world = false;
    gs.loaded_map = false;
    gs.load_progress = 0.f;
    std::thread load_world_th(&GameState::load_world_thread, &gs);
#if 0
    {
        std::unique_ptr<FILE, int (*)(FILE*)> fp(fopen(Path::get("locale/es/main.po").c_str(), "rt"), fclose);
        std::unique_ptr<char[]> tmp(new char[1000]);
        while(fgets(tmp.get(), 1000, fp.get()) != nullptr) {
            if(!strncmp(tmp.get(), "msgid", 5)) {
                std::unique_ptr<char[]> msgid(new char[100]);
                sscanf(tmp.get() + 5, " %*c%[^\"]s%*c ", msgid.get());
                fgets(tmp.get(), 1000, fp.get());
                if(!strncmp(tmp.get(), "msgstr", 6)) {
                    std::unique_ptr<char[]> msgstr(new char[100]);
                    sscanf(tmp.get() + 6, " %*c%[^\"]s%*c ", msgstr.get());
                    trans_msg[msgid.get()] = msgstr.get();
                }
            }
        }
        for(const auto& [key, value] : trans_msg)
            Eng3D::Log::debug("trans", key + "=" + value);
    }
#endif

    auto map_layer = new UI::Group(0, 0);

    auto *bg_img = new UI::Image(-(gs.width / 2.f), -(gs.height / 2.f), gs.width, gs.height);
    bg_img->origin = UI::Origin::CENTER_SCREEN;
    auto load_screen_entries = gs.package_man.get_multiple_prefix("gfx/load_screen");
    if(!load_screen_entries.empty())
        bg_img->current_texture = gs.tex_man.load(load_screen_entries[rand() % load_screen_entries.size()]->get_abs_path());

    auto* load_pbar = new UI::ProgressBar(0, -24, gs.width, 24, 0.f, 1.f);
    load_pbar->text(_("Initializing game resources"));
    load_pbar->origin = UI::Origin::LOWER_LEFT_SCREEN;
    load_pbar->text_color = Eng3D::Color(1.f, 1.f, 1.f);

    auto mod_logo_tex = gs.tex_man.load(gs.package_man.get_unique("gfx/mod_logo.png"));
    auto* mod_logo_img = new UI::Image(0, 0, mod_logo_tex->width, mod_logo_tex->height, mod_logo_tex);
    gs.do_run([&gs](){ return gs.loaded_map == false; },
        ([&gs]() {
            gs.music_enqueue();
            // Widgets here SHOULD NOT REQUEST UPON WORLD DATA
            // so no world lock is needed beforehand
            handle_event(gs);
        }), ([&gs, &map_layer, load_pbar]() {
            /// @todo first create the map and separately load all the assets
            std::scoped_lock lock(gs.render_lock);
            gs.clear();
            if(gs.loaded_world) {
                gs.map = new Map(gs, *gs.world, map_layer, gs.width, gs.height);
                gs.current_mode = MapMode::DISPLAY_ONLY;
                gs.map->set_view(MapView::SPHERE_VIEW);
                gs.map->camera->move(0.f, 50.f, 10.f);
                gs.loaded_map = true;
                gs.load_progress = 1.f;
            }

            load_pbar->set_value(gs.load_progress);
            gs.ui_ctx.render_all(gs.input.mouse_pos);
            gs.swap();
            gs.world->profiler.render_done();
        })
    );
    bg_img->kill();
    load_pbar->kill();
    mod_logo_img->kill();

    load_world_th.join();
    // LuaAPI::invoke_registered_callback(gs.world->lua, "map_dev_view_invoke");

    // Connect to server prompt
    new Interface::MainMenu(gs);
    // new Interface::MapDebugMenu(gs);
    Export::export_provinces(*gs.world);
    std::vector<Treaty::Id> displayed_treaties;
    auto current_frame_time = std::chrono::system_clock::now();
    // Start the world thread
    std::thread world_th(&GameState::world_thread, &gs);
    gs.do_run([&gs](){ return gs.run == true; },
        ([&displayed_treaties, &gs]() {
            gs.music_enqueue();
            // Locking is very expensive, so we condense everything into a big "if"
            if(gs.world->world_mutex.try_lock()) {
                // Required since events may request world data
                handle_event(gs);
                if(gs.current_mode == MapMode::NORMAL)
                    handle_popups(displayed_treaties, gs);

                if(gs.update_tick) {
                    gs.update_on_tick();
                    gs.map->map_render->update(gs);
                    gs.update_tick = false;

                    if(gs.current_mode == MapMode::NORMAL) {
                        // Production queue
                        for(unsigned int i = 0; i < gs.production_queue.size(); i++) {
                            const UnitType* unit_type = gs.production_queue[i];

                            /// @todo Make a better queue AI
                            bool is_built = false;
                            for(auto& building_type : gs.world->building_types) {
                                for(const auto province_id : gs.curr_nation->controlled_provinces) {
                                    auto& province = gs.world->provinces[province_id];
                                    auto& building = province.get_buildings()[gs.world->get_id(building_type)];
                                    // Must not be working on something else
                                    if(building.working_unit_type != nullptr) continue;
                                    is_built = true;
                                    gs.client->send(Action::BuildingStartProducingUnit::form_packet(province, building_type, *gs.curr_nation, *unit_type));
                                    break;
                                }

                                if(!is_built) break;
                            }
                            if(!is_built) break;
                            gs.production_queue.erase(gs.production_queue.begin() + i);
                            i--;
                        }
                    }
                }

                if(gs.current_mode == MapMode::DISPLAY_ONLY)
                    gs.map->camera->move(0.05f, 0.f, 0.f);
                gs.world->world_mutex.unlock();
            }
        }), ([&current_frame_time, &gs]() {
            std::scoped_lock lock(gs.render_lock);
            double prev_num = std::chrono::duration<double>(current_frame_time.time_since_epoch()).count();
            double now_num = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
            current_frame_time = std::chrono::system_clock::now();
            gs.delta_time = now_num - prev_num;

            gs.clear();
            if(gs.current_mode != MapMode::NO_MAP) {
                const std::scoped_lock lock(gs.world->world_mutex);
                gs.map->camera->update();
                gs.map->draw();
            }
            if(gs.show_ui)
                gs.ui_ctx.render_all(gs.input.mouse_pos);
            gs.swap();
            gs.world->profiler.render_done();
        })
    );
    world_th.join();
}

GameState::~GameState() {
    if(this->client != nullptr)
        delete this->client;
    if(this->server != nullptr)
        delete this->server;
    if(this->map != nullptr)
        delete this->map;
    this->run = false;
}
