#include "client/game_state.hpp"

#include <GL/glew.h>

#include <cstdint>
#include <cstdio>
#include <string>
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

#include "company.hpp"
#include "good.hpp"
#include "io_impl.hpp"
#include "path.hpp"
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
#include "client/map.hpp"
#include "unified_render/material.hpp"
#include "unified_render/model.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/assimp_model.hpp"
#include "client/ui.hpp"

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
    g_client->send(Action::SelectNation::form_packet(curr_nation));
    print_info("Selected nation [%s]", curr_nation->ref_name.c_str());
}

const UnifiedRender::Texture& GameState::get_nation_flag(Nation& nation) {
    Nation::Id id = world->get_id(&nation);
    return *map->nation_flags[id];
}

void handle_event(Input& input, GameState& gs, std::atomic<bool>& run) {
    std::pair<int, int>& mouse_pos = input.mouse_pos;
    std::pair<float, float>& select_pos = input.select_pos;

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
                gs.sound_queue.push_back(new UnifiedRender::Sound(Path::get("sfx/click.wav")));
                gs.map->handle_click(gs, event);
            }

            if(click_on_ui) {
                std::scoped_lock lock(gs.sound_lock);
                gs.sound_queue.push_back(new UnifiedRender::Sound(Path::get("sfx/click.wav")));
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
            case SDLK_p:
                if(gs.current_mode == MapMode::NORMAL) {
                    //gs.products_view_world->show();
                }
                break;
            case SDLK_a:
                if(gs.current_mode == MapMode::NORMAL) {
                    new Interface::ArmyView(gs);
                }
                break;
            case SDLK_SPACE:
                if(gs.current_mode == MapMode::NORMAL) {
                    gs.paused = !gs.paused;

                    if(gs.paused) {
                        ui_ctx->prompt("Control", "Unpaused");
                    } else {
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
            run = false;
            gs.paused = false;
            break;
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                std::pair<float, float> old_size = std::make_pair(width, height);
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
		
		std::scoped_lock lock(gs.world->world_mutex);

        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(map->camera->get_projection()));
        glViewport(0, 0, width, height);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(map->camera->get_view()));

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
    if(gs.current_mode != MapMode::NO_MAP) {
        gs.map->update_provinces(&gs.world->provinces);
    }
}

void handle_popups(std::vector<Event*>& displayed_events, std::vector<Treaty*>& displayed_treaties, GameState& gs) {
    // Put popups
    // Event + Descision popups
    for(auto& msg : gs.curr_nation->inbox) {
        // Check that the event is not already displayed to the user
        auto iter = std::find_if(displayed_events.begin(), displayed_events.end(),
            [&msg](const auto& e) { return e->ref_name == msg->ref_name; });
        if(iter != displayed_events.end()) continue;

        new Interface::DescisionWindow(gs, *msg);
        displayed_events.push_back(msg);
    }

    for(auto& treaty : gs.world->treaties) {
        // Check that the treaty is not already displayed
        auto iter = std::find_if(displayed_treaties.begin(), displayed_treaties.end(),
            [&treaty](const auto& e) { return e == treaty; });
        if(iter != displayed_treaties.end()) continue;

        // Must participate in treaty

        new Interface::TreatyChooseWindow(gs, treaty);
        displayed_treaties.push_back(treaty);
    }
}

void GameState::update_on_tick(void) {
    //render_lock.lock();
    ui_ctx->do_tick();
    //render_lock.lock();
}

void GameState::world_thread(void) {
    while(run) {
        while(paused) {};
        world->do_tick();
        update_tick = true;

        std::this_thread::sleep_for(std::chrono::milliseconds(ms_delay_speed));
    }
}

void main_loop(GameState& gs, Client* client, SDL_Window* window) {
    gs.current_mode = MapMode::COUNTRY_SELECT;
    gs.input = Input();

    glClearColor(0.3f, 0.3f, 0.3f, 0.5f);
    gs.select_nation = new Interface::LobbySelectView(gs);
    new MapDevView(gs.map);

    std::vector<Event*> displayed_events;
    std::vector<Treaty*> displayed_treaties;

    // Call update_on_tick on start of the gamestate
    gs.update_tick = true;
	
    gs.run = true;
	gs.paused = true;

    // Start the world thread
    std::thread world_th(&GameState::world_thread, &gs);
    while(gs.run) {
        std::scoped_lock lock(gs.render_lock);

        handle_event(gs.input, gs, gs.run);
        if(gs.current_mode == MapMode::NORMAL) {
            handle_popups(displayed_events, displayed_treaties, gs);
        }

        if(gs.update_tick) {
            gs.update_on_tick();
            gs.update_tick = false;
        }

        render(gs, gs.input, window);

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
                    print_info("BUILDING THING!?");
                    break;
                }
                if(!is_built) continue;

                gs.production_queue.erase(gs.production_queue.begin() + i);
                i--;
            }
        }

        if(gs.music_queue.empty()) {
            std::scoped_lock lock(gs.sound_lock);
            gs.music_fade_value = 100.f;
            gs.music_queue.push_back(new UnifiedRender::Sound(Path::get("music/war.wav")));
        }
    }
}

#include "client/interface/main_menu.hpp"
#include "unified_render/sound.hpp"

static void mixaudio(void* userdata, uint8_t* stream, int len) {
    GameState& gs = *((GameState*)userdata);
    std::memset(stream, 0, len);

    if(gs.sound_lock.try_lock()) {
        for(unsigned int i = 0; i < gs.sound_queue.size(); ) {
            int size = gs.sound_queue.size();
            UnifiedRender::Sound* sound = gs.sound_queue[i];
            int amount = sound->len - sound->pos;
            if(amount > len) amount = len;
            if(amount <= 0) {
                delete sound;
                gs.sound_queue.erase(gs.sound_queue.begin() + i);
                continue;
            }

            SDL_MixAudio(stream, &sound->data[sound->pos], amount, SDL_MIX_MAXVOLUME);
            sound->pos += amount;
            i++;
        }

        for(unsigned int i = 0; i < gs.music_queue.size(); ) {
            UnifiedRender::Sound* music = gs.music_queue[i];
            int amount = music->len - music->pos;
            if(amount > len) amount = len;
            if(amount <= 0) {
                delete music;
                gs.music_queue.erase(gs.music_queue.begin() + i);
                continue;
            }

            SDL_MixAudio(stream, &music->data[music->pos], amount, SDL_MIX_MAXVOLUME / gs.music_fade_value);
            music->pos += amount;
            i++;
        }
        gs.sound_lock.unlock();
    }

    if(gs.music_fade_value > 1.f) gs.music_fade_value -= 1.f;
}

void main_menu_loop(GameState& gs, SDL_Window* window) {
    gs.in_game = false;

    // Connect to server prompt
    UI::Image* mm_bg = new UI::Image(0, 0, gs.width, gs.height, &g_texture_manager->load_texture(Path::get("ui/globe.png")));
    mm_bg->is_fullscreen = true;

    Interface::MainMenu* main_menu = new Interface::MainMenu(gs);
	
	UI::Image* logo = new UI::Image(0, 0, 256, 256, &g_texture_manager->load_texture(Path::get("ui/title_alt.png")));
	logo->origin = CENTER_SCREEN;
	logo->above_of(*main_menu);
	logo->left_side_of(*main_menu);
	logo->x -= (main_menu->width / 2.f) + (logo->width / 2.f);

    gs.input = Input();
	
	std::atomic<bool> run = true;
    while(run) {
        handle_event(gs.input, gs, run);
        render(gs, gs.input, window);

        if(gs.in_game) {
            run = false;
            mm_bg->kill();
            main_menu->kill();
			logo->kill();

            std::scoped_lock lock(gs.sound_lock);
            for(auto& music : gs.music_queue) { delete music; }
            gs.music_queue.clear();
            break;
        }

        if(gs.music_queue.empty()) {
            std::scoped_lock lock(gs.sound_lock);
            gs.music_fade_value = 100.f;
            gs.music_queue.push_back(new UnifiedRender::Sound(Path::get("music/title.wav")));
        }
    }
}

extern UnifiedRender::TextureManager* g_texture_manager;
extern UnifiedRender::MaterialManager* g_material_manager;
extern UnifiedRender::ModelManager* g_model_manager;

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
void start_client(int argc, char** argv) {
    // globals
    GameState gs{};

    g_texture_manager = new UnifiedRender::TextureManager();
    g_material_manager = new UnifiedRender::MaterialManager();
    g_model_manager = new UnifiedRender::ModelManager();
    g_sound_manager = new UnifiedRender::SoundManager();
    gs.ui_ctx = new UI::Context();
	
    gs.ui_ctx->resize(gs.width, gs.height);

    gs.music_fade_value = 1.f;

    gs.loaded_world = false;
    gs.loaded_map = false;
	
	// Initialize sound subsystem (at 11,050 hz)
    SDL_AudioSpec fmt;
    fmt.freq = 11050;
    fmt.format = AUDIO_S16;
    fmt.channels = 1;
    fmt.samples = 512;
    fmt.callback = mixaudio;
    fmt.userdata = &gs;
    if(SDL_OpenAudio(&fmt, NULL) < 0)
        throw std::runtime_error("Unable to open audio: " + std::string(SDL_GetError()));
    SDL_PauseAudio(0);
    
    gs.world = new World();
    gs.world->load_mod();
    gs.map = new Map(*gs.world, gs.width, gs.height);

    main_menu_loop(gs, gs.window);
    main_loop(gs, gs.client, gs.window);
    return;
}

GameState::~GameState() {
    // delete world;
    // delete curr_nation;
    // delete map;
    // delete ui_ctx;
};
