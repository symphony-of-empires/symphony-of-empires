#include "client/game_state.hpp"

#include <GL/glew.h>

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
#ifdef _MSC_VER
// Required before GL/gl.h
#   ifndef _WINDOWS_
#   define WIN32_LEAN_AND_MEAN 1
#       include <windows.h>
#       undef WIN32_LEAN_AND_MEAN
#   endif
#endif

// MSVC does not know about glext, mingw does so we just use this ifdef
#ifndef _MSC_VER
#   include <GL/glext.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

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
#include "client/map.hpp"
#include "unified_render/material.hpp"
#include "unified_render/model.hpp"
#include "unified_render/texture.hpp"
#include "client/ui.hpp"
#include "server/server_network.hpp"

void GameState::play_nation() {
    current_mode = MapMode::NORMAL;

    const Province* capital = curr_nation->capital;
    if(capital != nullptr) {
        map->camera->position.x = capital->max_x;
        map->camera->position.y = capital->max_y;
        map->camera->position.z = -100.f;
    }

    // Make topwindow
    top_win = new Interface::TopWindow(*this);
    minimap = new Interface::Minimap(*this, 0, -256, UI::Origin::LOWER_LEFT_SCREEN);
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
    while(SDL_PollEvent(&event)) {
        bool click_on_ui = false;
        switch(event.type) {
        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
            ui_ctx->check_drag(mouse_pos.first, mouse_pos.second);
            if(event.button.button == SDL_BUTTON_MIDDLE) {
                input.middle_mouse_down = true;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
            if(event.button.button == SDL_BUTTON_MIDDLE) {
                input.middle_mouse_down = false;
                break;
            }

            click_on_ui = ui_ctx->check_click(mouse_pos.first, mouse_pos.second);
            if(!click_on_ui && gs.current_mode != MapMode::NO_MAP) {
                gs.sound_queue.push_back(new UnifiedRender::Sound(Path::get("sfx/click.ogg")));
                gs.map->handle_click(gs, event);
            }

            if(click_on_ui) {
                std::scoped_lock lock(gs.sound_lock);
                gs.sound_queue.push_back(new UnifiedRender::Sound(Path::get("sfx/click.ogg")));
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
            case SDLK_t:
                if(gs.current_mode == MapMode::NORMAL) {
                    new Interface::TreatyDraftView(gs);
                }
                break;
            case SDLK_SPACE:
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
                if(gs.current_mode == MapMode::NORMAL) {
                    if(input.select_pos.first < gs.world->width || input.select_pos.second < gs.world->height) {
                        const Tile& tile = gs.world->get_tile(input.select_pos.first, input.select_pos.second);
                        if(tile.province_id >= gs.world->provinces.size()) break;
                        new Interface::BuildingBuildView(gs, input.select_pos.first, input.select_pos.second, true, gs.world->provinces[tile.province_id]->owner, gs.world->provinces[tile.province_id]);
                    }
                }
                break;
            case SDLK_BACKSPACE:
                ui_ctx->check_text_input(nullptr);
                break;
            }
            break;
        case SDL_QUIT:
            gs.run = false;
            gs.paused = false;
            break;
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_GetWindowSize(SDL_GetWindowFromID(event.window.windowID), &width, &height);

                ui_ctx->resize(width, height);
                gs.width = width;
                gs.height = height;
            }
            break;
        default:
            break;
        }

        if(gs.current_mode != MapMode::NO_MAP && !click_on_ui) {
            gs.map->update(event, input);
        }
    }
    ui_ctx->clear_dead();
}

void GameState::send_command(Archive& archive) {
    std::scoped_lock lock(client->pending_packets_mutex);

    Packet packet = Packet(g_client->get_fd());
    packet.data(archive.get_buffer(), archive.size());
    client->pending_packets.push_back(packet);
}

void render(GameState& gs, Input& input, SDL_Window* window) {
    int& width = gs.width;
    int& height = gs.height;

    std::pair<float, float>& select_pos = input.select_pos;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    if(gs.current_mode != MapMode::NO_MAP) {
        Map* map = gs.map;

        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(map->camera->get_projection()));
        glViewport(0, 0, width, height);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(map->camera->get_view()));

        std::scoped_lock lock(gs.world->world_mutex);
        map->draw(gs, width, height);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBegin(GL_QUADS);
        glColor4f(1.f, 1.f, 1.f, 0.8f);
        glVertex2f(select_pos.first, select_pos.second);
        glVertex2f(select_pos.first + 1.f, select_pos.second);
        glVertex2f(select_pos.first + 1.f, select_pos.second + 1.f);
        glVertex2f(select_pos.first, select_pos.second + 1.f);
        glEnd();

        glPopMatrix();

        map->camera->update();
    }

    gs.ui_ctx->render_all();
    glLoadIdentity();
    glRasterPos2f(-3.0f, -2.0f);
    SDL_GL_SwapWindow(window);
}

void handle_popups(std::vector<Event*>& displayed_events, std::vector<Treaty*>& displayed_treaties, GameState& gs) {
    // Put popups
    // Event + Descision popups
    for(auto& msg : gs.curr_nation->inbox) {
        // Check that the event is not already displayed to the user
        auto iter = std::find_if(displayed_events.begin(), displayed_events.end(), [&msg](const auto& e) { return e->ref_name == msg->ref_name; });
        if(iter != displayed_events.end()) continue;

        new Interface::DescisionWindow(gs, *msg);
        displayed_events.push_back(msg);
    }

    for(auto& treaty : gs.world->treaties) {
        // Check that the treaty is not already displayed
        auto iter = std::find_if(displayed_treaties.begin(), displayed_treaties.end(), [&treaty](const auto& e) { return e == treaty; });
        if(iter != displayed_treaties.end()) continue;

        // Do not mess with treaties we don't partake in, hehe
        if(!treaty->does_participate(gs.curr_nation)) continue;

        // Must participate in treaty
        new Interface::TreatyChooseWindow(gs, treaty);
        displayed_treaties.push_back(treaty);
    }
}

void GameState::update_on_tick(void) {
    //render_lock.lock();
    ui_ctx->do_tick();
    if(current_mode != MapMode::NO_MAP) {
        map->update_mapmode();
    }
    //render_lock.lock();
}

void GameState::world_thread(void) {
    while(run) {
        while(paused) {
            if(!run) return;
        }
        world->do_tick();
        update_tick = true;

        std::this_thread::sleep_for(std::chrono::milliseconds(ms_delay_speed));
    }
}

#include "client/interface/main_menu.hpp"
#include "unified_render/sound.hpp"
#include <filesystem>

void main_loop(GameState& gs) {
    gs.in_game = false;

    // Connect to server prompt
    /*auto* mm_bg = new UI::Image(0, 0, gs.width, gs.height, &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/globe.png")));
    mm_bg->is_fullscreen = true;
    Interface::MainMenu* main_menu = new Interface::MainMenu(gs);
    auto* logo = new UI::Image(0, 0, 256, 256, &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/title_alt.png")));
    logo->above_of(*main_menu);
    logo->left_side_of(*main_menu);*/

    std::vector<Event*> displayed_events;
    std::vector<Treaty*> displayed_treaties;

    // Call update_on_tick on start of the gamestate
    gs.update_tick = true;
    gs.run = true;
    gs.paused = true;

    gs.current_mode = MapMode::COUNTRY_SELECT;
    gs.select_nation = new Interface::LobbySelectView(gs);
    new MapDevView(gs.map);

    gs.input = Input();

    gs.host_mode = true;
    gs.server = new Server(gs, 1836);
    gs.client = new Client(gs, "127.0.0.1", 1836);
    gs.client->username = "TUTORIAL_PLAYER";
    gs.in_game = true;

    // Start the world thread
    std::thread world_th(&GameState::world_thread, &gs);
    glClearColor(0.3f, 0.3f, 0.3f, 0.5f);
    while(gs.run) {
        std::scoped_lock lock(gs.render_lock);
        handle_event(gs.input, gs);

        if(gs.current_mode == MapMode::NORMAL) {
            std::scoped_lock lock(gs.world->world_mutex);
            handle_popups(displayed_events, displayed_treaties, gs);
        }

        if(gs.update_tick) {
            gs.update_on_tick();
            gs.update_tick = false;
        }

        render(gs, gs.input, gs.window);

        if(gs.current_mode == MapMode::NORMAL) {
            std::scoped_lock lock(gs.world->world_mutex);
            // Production queue
            if(!gs.production_queue.empty()) {
                for(unsigned int i = 0; i < gs.production_queue.size(); i++) {
                    UnitType* unit = gs.production_queue[i];

                    // TODO: Make a better queue AI
                    bool is_built = false;
                    for(const auto& building : gs.world->buildings) {
                        // Must be our building
                        if(building->get_owner() != gs.curr_nation) continue;

                        // Must not be working on something else
                        if(building->working_unit_type != nullptr) continue;

                        is_built = true;

                        g_client->send(Action::BuildingStartProducingUnit::form_packet(building, unit));
                        break;
                    }
                    if(!is_built) continue;

                    gs.production_queue.erase(gs.production_queue.begin() + i);
                    i--;
                }
            }
        }

        if(gs.music_queue.empty()) {
            // Search through all the music in 'music/ambience' and picks a random
            auto entries = Path::get_all_recursive("music/ambience");
            if(entries.size() != 0) {
                int music_index = std::rand() % entries.size();
                std::scoped_lock lock(gs.sound_lock);
                gs.music_fade_value = 100.f;
                gs.music_queue.push_back(new UnifiedRender::Sound(entries[music_index]));
            }
        }
    }
    world_th.join();
}

void start_client(int argc, char** argv) {
    // globals
    GameState gs{};
    
    gs.ui_ctx = new UI::Context();

    gs.ui_ctx->resize(gs.width, gs.height);

    gs.loaded_world = false;
    gs.loaded_map = false;

    gs.world = new World();
    gs.world->load_initial();
    gs.world->load_mod();
    gs.map = new Map(*gs.world, gs.width, gs.height);

    /*gs.tutorial.fire_at_start = 0;
    if(!gs.tutorial.fire_at_start) {
        main_menu_loop(gs, gs.window);
    } else {
        gs.host_mode = true;
        gs.server = new Server(gs, 1836);
        gs.client = new Client(gs, "127.0.0.1", 1836);
        gs.client->username = "TUTORIAL_PLAYER";
        gs.in_game = true;
    }*/
    main_loop(gs);
    return;
}

GameState::~GameState() {
    // delete world;
    // delete curr_nation;
    // delete map;
    // delete ui_ctx;
};
