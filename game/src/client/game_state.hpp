// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
#include <functional>
#include <algorithm>

#include "eng3d/serializer.hpp"
#include "eng3d/audio.hpp"
#include "eng3d/state.hpp"
#include "eng3d/event.hpp"
#include "world.hpp"
#include "world.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
#include "client/map.hpp"
#include "eventpp/callbacklist.h"

enum class MapMode : unsigned char {
    COUNTRY_SELECT,
    NORMAL,
    NO_MAP,
    DISPLAY_ONLY,
};

struct Building;
struct Language;
struct Religion;

class ClientState {
    eventpp::CallbackList<void (const std::vector<UnitId>)> on_update_units;
    std::vector<UnitId> selected_units;
public:
    inline const std::vector<UnitId> get_selected_units() const {
        return selected_units;
    }

    inline bool is_selected_unit(UnitId id) const {
        return std::count(selected_units.begin(), selected_units.end(), id);       
    }

    inline void select_unit(UnitId id) {
        auto it = std::lower_bound(selected_units.begin(), selected_units.end(), id);
        if(it == selected_units.end() || *it != id) {
            selected_units.insert(it, id);
            on_update_units(selected_units);
        }
    }

    inline void unselect_unit(UnitId id) {
        std::erase(selected_units, id);
        on_update_units(selected_units);
    }

    inline void clear_selected_units() {
        selected_units.clear();
        on_update_units(selected_units);
    }

    inline auto add_listener(std::function<void(std::vector<UnitId>)> callback) {
        return on_update_units.append(callback);
    }

    inline void remove_listener(eventpp::CallbackList<void (const std::vector<UnitId>)>::Handle handle) {
        on_update_units.remove(handle);
    }
};

class Input {
public:
    glm::vec2 select_pos;
    glm::ivec2 drag_coord;
    bool middle_mouse_down = false;


    Language* selected_language = nullptr;
    Religion* selected_religion = nullptr;
};

class Client;
class Server;
class World;
class Nation;
struct UnitType;
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
    GameState(const std::vector<std::string>& pkg_paths);
    ~GameState() = default;

    void play_nation();
    void update_on_tick();
    void world_thread();
    void music_enqueue();
    void load_world_thread();
    void handle_resize() override;
    void handle_mouse_btn(const Eng3D::Event::MouseButton&) override;
    void handle_mouse_motion(const Eng3D::Event::MouseMotion&) override;
    void handle_mouse_wheel(const Eng3D::Event::MouseWheel&) override;
    void handle_key(const Eng3D::Event::Key&) override;

    std::unique_ptr<Client> client;
    std::unique_ptr<Server> server;

    std::atomic<bool> loaded_world;
    std::atomic<bool> loaded_map;
    float load_progress = 0.f;

    std::atomic<bool> update_tick;
    std::atomic<bool> paused;
    std::atomic<int> ms_delay_speed;
    std::atomic<bool> quit;

    // The ui will mostly need to read the world state
    World* world = nullptr;
    // NationId curr_nation
    Nation* curr_nation = nullptr;
    std::unique_ptr<Map> map;
    Input input;
    ClientState client_state;
    MapMode current_mode = MapMode::NO_MAP;

    Interface::LobbySelectView* select_nation = nullptr;
    UI::Widget* top_win = nullptr;
    UI::Widget* time_win = nullptr;
    UI::Widget* minimap = nullptr;
    UI::Widget* profiler_view = nullptr;
    UI::Widget* event_tray_grp = nullptr;
    UI::Widget* unit_menu = nullptr;

    std::vector<std::shared_ptr<Eng3D::Texture>> nation_flags;
    std::shared_ptr<Eng3D::Texture> get_nation_flag(const Nation& nation);

    // Used for synchronization between the networking client and the rendering thread
    std::mutex render_lock;
    std::vector<UnitTypeId> production_queue;

    UI::Widget* right_side_panel = nullptr;
    UI::Widget* lower_left_panel = nullptr;

    std::string ip_address = "127.0.0.1";
    bool in_game = false;
    bool motion_blur = true;
    bool host_mode = false;
    bool editor = false;
    bool sea_paint = false;
    bool singleplayer = true;

    std::vector<Eng3D::StringRef> decision_autodo;
};

// Run world tick and pending commands
extern "C" void game_main(int argc, char** argv);
