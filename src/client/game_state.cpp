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
#else
#   include <SDL2/SDL.h>
#   include <SDL2/SDL_events.h>
#   include <SDL2/SDL_keycode.h>
#   include <SDL2/SDL_mouse.h>
#   include <SDL2/SDL_opengl.h>
#   include <SDL2/SDL_ttf.h>
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
#include "serializer.hpp"
#include "world.hpp"
#include "client/camera.hpp"
#include "client/orbit_camera.hpp"
#include "client/client_network.hpp"
#include "client/interface/descision.hpp"
#include "client/interface/province_view.hpp"
#include "client/interface/select_nation.hpp"
#include "client/interface/top_window.hpp"
#include "client/interface/province_view.hpp"
#include "client/interface/treaty.hpp"
#include "client/interface/map_dev_view.hpp"
#include "client/interface/army.hpp"
#include "client/interface/building.hpp"
#include "client/map.hpp"
#include "client/render/material.hpp"
#include "client/render/model.hpp"
#include "client/render/texture.hpp"
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

    g_client->packet_mutex.lock();
    Packet packet = Packet();
    Archive ar = Archive();
    ActionType action = ActionType::SELECT_NATION;
    ::serialize(ar, &action);
    ::serialize(ar, &curr_nation);
    packet.data(ar.get_buffer(), ar.size());
    g_client->packet_queue.push_back(packet);
    g_client->packet_mutex.unlock();
    print_info("Selected nation %s", curr_nation->ref_name.c_str());
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
        int click_on_ui = 0;
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
            if(click_on_ui == 0 && gs.current_mode != MapMode::NO_MAP) {
                gs.map->handle_click(gs, event);
            }
            break;
        case SDL_MOUSEMOTION:
            SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
            ui_ctx->check_hover(mouse_pos.first, mouse_pos.second);
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
                    //new TreatyWindow(gs, gs.top_win->top_win);
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
            case SDLK_b:
                if(gs.current_mode == MapMode::NORMAL) {
                    const Tile& tile = gs.world->get_tile(input.select_pos.first, input.select_pos.second);
                    new Interface::BuildingBuildView(gs, input.select_pos.first, input.select_pos.second, true, gs.world->nations[tile.owner_id], gs.world->provinces[tile.province_id]);
                }
                break;
            case SDLK_BACKSPACE:
                ui_ctx->check_text_input(nullptr);
                break;
            }
            break;
        case SDL_QUIT:
            run = false;
            break;
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                std::pair<float, float> old_size = std::make_pair(width, height);
                SDL_GetWindowSize(SDL_GetWindowFromID(event.window.windowID), &width, &height);

                // Resize/recenter UI according to screen change
                for(auto& widget : ui_ctx->widgets) {
                    widget->x *= width / old_size.first;
                    widget->y *= height / old_size.second;
                }

                if(gs.current_mode != MapMode::NO_MAP){
                    gs.map->camera->set_screen(width, height);
                }
            }
            break;
        default:
            break;
        }
        if(gs.current_mode != MapMode::NO_MAP && !click_on_ui){
            gs.map->update(event, input);
        }
    }
    ui_ctx->clear_dead();
}

void GameState::send_command(Archive& archive) {
    client->packet_mutex.lock();

    Packet packet = Packet(g_client->get_fd());
    packet.data(archive.get_buffer(), archive.size());
    client->packet_queue.push_back(packet);

    client->packet_mutex.unlock();
}

void render(GameState& gs, Input& input, SDL_Window* window) {
    int& width = gs.width;
    int& height = gs.height;

    std::pair<float, float>& select_pos = input.select_pos;
    Unit* selected_unit = input.selected_unit;
    Building* selected_building = input.selected_building;

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

        map->draw(width, height);

        gs.world->world_mutex.lock();
        if(selected_unit != nullptr) {
            glBegin(GL_LINE_STRIP);
            glColor3f(1.f, 0.f, 0.f);
            glVertex2f(selected_unit->x, selected_unit->y);
            glVertex2f(selected_unit->x + 1.f, selected_unit->y);
            glVertex2f(selected_unit->x + 1.f, selected_unit->y + 1.f);
            glVertex2f(selected_unit->x, selected_unit->y + 1.f);
            glEnd();
        }

        if(selected_building != nullptr) {
            glBegin(GL_LINE_STRIP);
            glColor3f(1.f, 0.f, 0.f);
            glVertex2f(selected_building->x, selected_building->y);
            glVertex2f(selected_building->x + 1.f, selected_building->y);
            glVertex2f(selected_building->x + 1.f, selected_building->y + 1.f);
            glVertex2f(selected_building->x, selected_building->y + 1.f);
            glEnd();
        }
        gs.world->world_mutex.unlock();

        glBindTexture(GL_TEXTURE_2D, 0);
        glBegin(GL_QUADS);
        glColor3f(1.f, 1.f, 1.f);
        glVertex2f(select_pos.first, select_pos.second);
        glVertex2f(select_pos.first + 1.f, select_pos.second);
        glVertex2f(select_pos.first + 1.f, select_pos.second + 1.f);
        glVertex2f(select_pos.first, select_pos.second + 1.f);
        glEnd();

        glPopMatrix();

        map->camera->update();
        map->update_tiles(*gs.world);
    }

    gs.ui_ctx->render_all(width, height);
    glLoadIdentity();
    glRasterPos2f(-3.0f, -2.0f);
    SDL_GL_SwapWindow(window);
}

void handle_popups(std::vector<Event*>& displayed_events, std::vector<Treaty*>& displayed_treaties, GameState& gs) {
    // Put popups
    // Event + Descision popups
    for(auto& msg : gs.curr_nation->inbox) {
        // Check that the event is not already displayed to the user
        auto iter = std::find_if(displayed_events.begin(), displayed_events.end(),
            [&msg](const auto& e) { return e->ref_name == msg->ref_name; });
        if(iter != displayed_events.end()) {
            continue;
        }

        new Interface::DescisionWindow(gs, *msg);
        displayed_events.push_back(msg);
    }

    for(auto& treaty : gs.world->treaties) {
        // Check that the treaty is not already displayed
        auto iter = std::find_if(displayed_treaties.begin(), displayed_treaties.end(),
            [&treaty](const auto& e) { return e == treaty; });
        if(iter != displayed_treaties.end()) {
            continue;
        }

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

void main_loop(GameState& gs, Client* client, SDL_Window* window) {
    gs.current_mode = MapMode::COUNTRY_SELECT;
    gs.input = Input{};

    // Query the initial nation flags
    for(const auto& nation : gs.world->nations) {
        std::string path;
        path = Path::get("ui/flags/" + nation->ref_name + "_" +
            ((nation->ideology == nullptr)
                ? "none"
                : nation->ideology->ref_name) + ".png"
        );
        gs.map->nation_flags.push_back(&g_texture_manager->load_texture(path));
    }

    for(const auto& building_type : gs.world->building_types) {
        std::string path = Path::get("3d/building_types/" + building_type->ref_name + ".obj");
        gs.map->outpost_type_icons.push_back(&g_model_manager->load_complex(path));
    }
    for(const auto& unit_type : gs.world->unit_types) {
        std::string path = Path::get("3d/unit_types/" + unit_type->ref_name + ".obj");
        gs.map->unit_type_icons.push_back(&g_model_manager->load_complex(path));
    }

    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    gs.select_nation = new Interface::SelectNation(gs);
    new MapDevView(gs.map);

    std::vector<Event*> displayed_events;
    std::vector<Treaty*> displayed_treaties;

    gs.update_tick = false;

    std::atomic<bool> run;
    run = true;
    while(run) {
        std::lock_guard lock(gs.render_lock);

        handle_event(gs.input, gs, run);
        if(gs.current_mode == MapMode::NORMAL) {
            handle_popups(displayed_events, displayed_treaties, gs);
        }

        if(gs.update_tick == true) {
            gs.update_on_tick();
            gs.update_tick = false;
        }

        render(gs, gs.input, window);

        // Production queue
        if(!gs.production_queue.empty()) {
            for(uint i = 0; i < gs.production_queue.size(); i++) {
                UnitType* unit = gs.production_queue[i];

                // TODO: Make a better queue AI
                bool is_built = false;
                for(const auto& building : gs.world->buildings) {
                    // Must be our building
                    if(building->get_owner() != gs.curr_nation) continue;

                    // Must not be working on something else
                    if(building->working_unit_type != nullptr) continue;

                    g_client->packet_mutex.lock();
                    Packet packet = Packet();
                    Archive ar = Archive();
                    ActionType action = ActionType::BUILDING_START_BUILDING_UNIT;
                    ::serialize(ar, &action);
                    ::serialize(ar, &building);
                    ::serialize(ar, &unit);
                    packet.data(ar.get_buffer(), ar.size());
                    g_client->packet_queue.push_back(packet);
                    g_client->packet_mutex.unlock();
                    is_built = true;
                }

                // If we couldn't find a suitable building we wont be able to find buildings for other
                // units either
                if(!is_built) break;

                gs.production_queue.erase(gs.production_queue.begin() + i);
                i--;
            }
        }
    }
}

#include "interface/main_menu.hpp"

void main_menu_loop(GameState& gs, SDL_Window* window) {
    std::atomic<bool> run;
    run = true;

    gs.in_game = false;

    // Connect to server prompt
    UI::Image* mm_bg = new UI::Image(0, 0, gs.width, gs.height, &g_texture_manager->load_texture(Path::get("ui/globe.png")));

    Interface::MainMenu* main_menu = new Interface::MainMenu(gs);

    gs.input = Input{};
    Input& input = gs.input;
    while(run) {
        handle_event(input, gs, run);
        render(gs, input, window);

        if(gs.in_game == true) {
            run = false;
            mm_bg->kill();
            main_menu->kill();
        }
    }
}

extern UnifiedRender::TextureManager* g_texture_manager;
extern UnifiedRender::MaterialManager* g_material_manager;
extern UnifiedRender::ModelManager* g_model_manager;

char* tmpbuf;
World::World(void) {
    g_world = this;
};
World::~World(){};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
void start_client(int argc, char** argv) {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    // globals
    SDL_Window* window;
    int width = 1280, height = 800;
    GameState gs{};

    window = SDL_CreateWindow("Symphony of Empires", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);  // Enable OpenGL VSYNC
    print_info("OpenGL Version: %s", glGetString(GL_VERSION));

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.f, 1.f);

    g_texture_manager = new UnifiedRender::TextureManager();
    g_material_manager = new UnifiedRender::MaterialManager();
    g_model_manager = new UnifiedRender::ModelManager();
    gs.ui_ctx = new UI::Context();

    tmpbuf = new char[512];
    GLenum err = glewInit();
    if(err != GLEW_OK)
        throw std::runtime_error("Failed to init GLEW");

    gs.width = width;
    gs.height = height;
    main_menu_loop(gs, window);
    main_loop(gs, gs.client, window);

    delete[] tmpbuf;

    TTF_Quit();
    SDL_Quit();
    return;
}

GameState::~GameState() {
    delete world;
    delete curr_nation;
    delete map;
    delete ui_ctx;
};
