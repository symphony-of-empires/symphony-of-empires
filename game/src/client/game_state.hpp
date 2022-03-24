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
//      client/game_state.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <queue>
#include <mutex>
#include <vector>
#include <atomic>

#include "unified_render/serializer.hpp"
#include "unified_render/audio.hpp"
#include "unified_render/state.hpp"
#include "tutorial.hpp"

enum class MapMode : unsigned char {
    COUNTRY_SELECT,
    NORMAL,
    NO_MAP,
    DISPLAY_ONLY,
};

class Unit;
class Building;

class Input {
public:
    std::pair<float, float> select_pos;
    bool middle_mouse_down = false;
    std::pair<float, float> last_camera_mouse_pos;
    std::pair<int, int> mouse_pos;

    std::vector<Unit*> selected_units;

    std::pair<int, int> drag_coord;

    SDL_Joystick* joy = nullptr;
};

class Client;
class Server;
class World;
class Nation;
class Map;
class UnitType;
struct SDL_Window;
namespace Interface {
    class LobbySelectView;
    class TopWindow;
    class Minimap;
    class ProfilerView;
}
namespace UI {
    class Context;
    class Widget;
}
namespace UnifiedRender {
    class Texture;
}

// The all encompassing client state
// This is the state we could pass down to all the ui widgets
class GameState: public UnifiedRender::State {
public:
    GameState() {};
    ~GameState();

    void play_nation();
    void send_command(Archive& archive);
    void update_on_tick(void);
    void world_thread(void);

    Client* client = nullptr;
    Server* server = nullptr;

    std::atomic<bool> loaded_world;
    std::atomic<bool> loaded_map;

    std::atomic<bool> update_tick;
    std::atomic<bool> run;
    std::atomic<bool> paused;
    std::atomic<int> ms_delay_speed;
    std::atomic<bool> quit;
    bool host_mode;

    // The ui will mostly need to read the world state
    World* world = nullptr;
    // Nation::Id curr_nation
    Nation* curr_nation = nullptr;
    // Used for mapmodes
    Map* map = nullptr;
    Input input;
    MapMode current_mode = MapMode::NO_MAP;

    Interface::LobbySelectView* select_nation;
    Interface::TopWindow* top_win;
    Interface::Minimap* minimap;
    Interface::ProfilerView* profiler_view = nullptr;

    std::vector<const UnifiedRender::Texture*> nation_flags;
    const UnifiedRender::Texture& get_nation_flag(Nation& nation);

    // Used for synchronization between the networking client and the rendering thread
    std::mutex render_lock;

    // Queue of production for unit_types
    std::deque<UnitType*> production_queue;

    TutorialState tutorial;

    UI::Widget* right_side_panel = nullptr;

    bool in_game;

    // Settings
    bool motion_blur = false;

    // Editor options
    bool editor = true;
};

// Run world tick and pending commands
void main_loop(GameState&, Client*, SDL_Window*);
void start_client(int argc, char** argv);
void save(GameState& gs);
