#pragma once

#include <queue>
#include <mutex>
#include <vector>
#include <atomic>

#include "unified_render/serializer.hpp"
#include "unified_render/sound.hpp"
#include "unified_render/state.hpp"
#include "tutorial.hpp"

enum class MapMode {
    COUNTRY_SELECT,
    NORMAL,
    NO_MAP,
};

class Unit;
class Building;

class Input {
public:
    std::pair<float, float> select_pos;
    bool middle_mouse_down = false;
    std::pair<float, float> last_camera_drag_pos;
    std::pair<float, float> last_camera_mouse_pos;
    std::pair<int, int> mouse_pos;

    std::vector<Unit*> selected_units;

    bool is_drag;
    std::pair<int, int> drag_coord;
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
class GameState : public UnifiedRender::State {
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
    Nation* curr_nation;
    Map* map = nullptr;
    Input input;
    MapMode current_mode = MapMode::NO_MAP;

    UI::Context* ui_ctx;

    Interface::LobbySelectView* select_nation;
    Interface::TopWindow* top_win;
    Interface::Minimap* minimap;

    std::vector<const UnifiedRender::Texture*> nation_flags;
    const UnifiedRender::Texture& get_nation_flag(Nation& nation);

    // Used for synchronization between the networking client and the rendering thread
    std::mutex render_lock;

    // Queue of production for unit_types
    std::deque<UnitType*> production_queue;

    TutorialState tutorial;

    UI::Widget* right_side_panel = nullptr;

    bool in_game;
};

// Run world tick and pending commands
void main_loop(GameState&, Client*, SDL_Window*);
void start_client(int argc, char** argv);
