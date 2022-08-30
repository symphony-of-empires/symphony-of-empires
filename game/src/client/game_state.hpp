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
#include <set>
#include <atomic>
#include <algorithm>

#include "eng3d/serializer.hpp"
#include "eng3d/audio.hpp"
#include "eng3d/state.hpp"
#include "eng3d/event.hpp"
#include "unit.hpp"

enum class MapMode : unsigned char {
    COUNTRY_SELECT,
    NORMAL,
    NO_MAP,
    DISPLAY_ONLY,
};

class Building;
class Language;
class Religion;

class Input {
    std::set<Unit::Id> selected_units;
public:
    glm::vec2 select_pos;
    bool middle_mouse_down = false;
    glm::ivec2 mouse_pos;

    inline const std::set<Unit::Id> get_selected_units() const {
        return selected_units;
    }

    inline bool is_selected_unit(Unit::Id id) const {
        return std::count(selected_units.begin(), selected_units.end(), id);       
    }

    inline void select_unit(Unit::Id id) {
        selected_units.insert(id);       
    }

    inline void unselect_unit(Unit::Id id) {
        selected_units.erase(id);
    }

    inline void clear_selected_units() {
        selected_units.clear();
    }

    Language* selected_language = nullptr;
    Religion* selected_religion = nullptr;

    glm::ivec2 drag_coord;
};

class Client;
class Server;
class World;
class Nation;
class Map;
class UnitType;
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
namespace Eng3D {
    class Texture;
}

// The all encompassing client state
// This is the state we could pass down to all the ui widgets
class GameState: public Eng3D::State {
public:
    GameState(const std::vector<std::string>& pkg_paths)
        : Eng3D::State::State(pkg_paths)
    {

    }
    ~GameState();

    void play_nation();
    void send_command(Archive& archive);
    void update_on_tick();
    void world_thread();
    void music_enqueue();
    void load_world_thread();
    void handle_resize() override;
    void handle_mouse_btn(const Eng3D::Event::MouseButton&) override;
    void handle_mouse_motion(const Eng3D::Event::MouseMotion&) override;
    void handle_mouse_wheel(const Eng3D::Event::MouseWheel&) override;
    void handle_key(const Eng3D::Event::Key&) override;

    Client* client = nullptr;
    Server* server = nullptr;

    std::atomic<bool> loaded_world;
    std::atomic<bool> loaded_map;
    float load_progress = 0.f;

    std::atomic<bool> update_tick;
    std::atomic<bool> paused;
    std::atomic<int> ms_delay_speed;
    std::atomic<bool> quit;
    bool host_mode = false;

    float delta_time;

    // The ui will mostly need to read the world state
    World* world = nullptr;
    // Nation::Id curr_nation
    Nation* curr_nation = nullptr;
    // Used for mapmodes
    Map* map = nullptr;
    Input input;
    MapMode current_mode = MapMode::NO_MAP;

    Interface::LobbySelectView* select_nation = nullptr;
    UI::Widget* top_win = nullptr;
    UI::Widget* time_win = nullptr;
    UI::Widget* minimap = nullptr;
    UI::Widget* profiler_view = nullptr;

    std::vector<std::shared_ptr<Eng3D::Texture>> nation_flags;
    std::shared_ptr<Eng3D::Texture> get_nation_flag(const Nation& nation);

    // Used for synchronization between the networking client and the rendering thread
    std::mutex render_lock;

    // Queue of production for unit_types
    std::deque<const UnitType*> production_queue;

    UI::Widget* right_side_panel = nullptr;
    UI::Widget* lower_left_panel = nullptr;

    bool in_game = false;
    bool motion_blur = true; // Settings
    bool editor = true; // Editor options
    bool show_ui = true;
};

// Run world tick and pending commands
void start_client(int argc, char** argv);
