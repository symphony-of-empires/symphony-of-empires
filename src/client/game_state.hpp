#pragma once

#include <queue>
#include <mutex>
#include <vector>
#include <atomic>

#include "nation.hpp"
#include "world.hpp"
#include "client/interface/select_nation.hpp"
#include "client/interface/map_dev_view.hpp"
#include "client/interface/top_window.hpp"
#include "serializer.hpp"
#include "client/map.hpp"
#include "client/ui.hpp"
#include "client/camera.hpp"
#include "client/orbit_camera.hpp"

enum class MapMode {
    COUNTRY_SELECT,
    NORMAL,
    NO_MAP,
};

class Input {
public:
    std::pair<float, float> select_pos;
    bool middle_mouse_down = false;
    std::pair<float, float> last_camera_drag_pos;
    std::pair<int, int> mouse_pos;

    Unit* selected_unit = nullptr;
    Building* selected_building = nullptr;
};

// The all encompassing client state
// This is the state we could pass down to all the ui widgets
class Map;
class Client;
class GameState {
public:
    GameState(OrbitCamera _cam) : cam{_cam} {};
    // TODO: add deconstructor

    void play_nation();
    void send_command(Archive& archive);
    void update_on_tick(void);

    Client* client = nullptr;

    std::atomic<bool> update_tick;

    // The ui will mostly need to read the world state
    World* world = nullptr;
    // Nation::Id curr_nation
    Nation* curr_nation;
    Map* map;
    // TODO Move camera into map later since it will never be used outside of map anyway
    OrbitCamera cam;
    Input input;
    int width, height;
    MapMode current_mode = MapMode::NO_MAP;

    UI::Context* ui_ctx;

    Interface::SelectNation* select_nation;
    Interface::TopWindow* top_win;

    std::vector<const UnifiedRender::Texture*> nation_flags;
    const UnifiedRender::Texture& get_nation_flag(Nation& nation);

    // Used for synchronization between the networking client and the rendering thread
    std::recursive_mutex render_lock;
};

// Run world tick and pending commands
void main_loop(GameState&, Client*, SDL_Window*);
void start_client(int argc, char** argv);
