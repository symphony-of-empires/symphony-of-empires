#pragma once

#include <queue>
#include <mutex>
#include <vector>
#include <atomic>

#include "serializer.hpp"
#include "render/sound.hpp"

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

    Unit* selected_unit = nullptr;
    Building* selected_building = nullptr;
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
}
namespace UI {
    class Context;
}
namespace UnifiedRender {
    class Texture;
}

// The all encompassing client state
// This is the state we could pass down to all the ui widgets
class GameState {
public:
    GameState() {};
    ~GameState();

    void play_nation();
    void send_command(Archive& archive);
    void update_on_tick(void);
    void world_thread(void);

    Client* client = nullptr;
    Server* server = nullptr;

    std::atomic<bool> update_tick;
    std::atomic<bool> run;
    std::atomic<bool> paused;
    bool host_mode;

    // The ui will mostly need to read the world state
    World* world = nullptr;
    // Nation::Id curr_nation
    Nation* curr_nation;
    Map* map;
    Input input;
    int width, height;
    MapMode current_mode = MapMode::NO_MAP;

    UI::Context* ui_ctx;

    Interface::LobbySelectView* select_nation;
    Interface::TopWindow* top_win;

    std::vector<const UnifiedRender::Texture*> nation_flags;
    const UnifiedRender::Texture& get_nation_flag(Nation& nation);

    // Used for synchronization between the networking client and the rendering thread
    std::recursive_mutex render_lock;

    // Queue of production for unit_types
    std::deque<UnitType*> production_queue;

    // Queue of sounds/music
    std::vector<UnifiedRender::Sound*> sound_queue;
    std::vector<UnifiedRender::Sound*> music_queue;
    float music_fade_value;

    bool in_game;
};

// Run world tick and pending commands
void main_loop(GameState&, Client*, SDL_Window*);
void start_client(int argc, char** argv);
