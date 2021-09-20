#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <queue>

#include "../nation.hpp"
#include "../world.hpp"
#include "command.hpp"
#include "interface/industry_view_nation.hpp"
#include "interface/pop_view_nation.hpp"
#include "interface/products_view_world.hpp"
#include "interface/province_view.hpp"
#include "interface/select_nation.hpp"
#include "interface/top_window.hpp"
#include "interface/ui_reform.hpp"
#include "map.hpp"
#include "ui.hpp"

enum MapMode {
    MAP_MODE_COUNTRY_SELECT,
    MAP_MODE_NORMAL,
};

struct Input {
    std::pair<float, float> select_pos;
    bool middle_mouse_down = false;
    std::pair<float, float> last_camera_drag_pos;
    std::pair<int, int> mouse_pos;
    Boat* selected_boat = nullptr;
    Unit* selected_unit = nullptr;
    Building* selected_building = nullptr;
};

// The all encompassing client state
// This is the state we could pass down to all the ui widgets
class ProvinceView;
class TopWindow;
class SelectNation;
class IndustryViewNation;
class PopViewNation;
class ProductsViewWorld;
class UIReform;
class GameState {
   public:
    GameState(Camera _cam) : cam{_cam} {};
    // TODO add deconstructor
    void play_nation();

    // The ui will mostly need to read the world state
    World* world;
    // Nation::Id curr_nation
    Nation* curr_nation;
    Map* map;
    // TODO Move camera into map later since it will never be used outside of map anyway
    Camera cam;
    Input input;
    int width, height;
    MapMode current_mode = MapMode::MAP_MODE_COUNTRY_SELECT;

    UI::Context* ui_ctx;

    SelectNation* select_nation;
    TopWindow* top_win;
    ProvinceView* province_view;
    ProductsViewWorld* products_view_world;
    IndustryViewNation* industry_view_nation;
    PopViewNation* pop_view_nation;
    UIReform* ui_reform;

    std::vector<const UnifiedRender::Texture*> nation_flags;
    const UnifiedRender::Texture& get_nation_flag(Nation& nation);

    // Ui calls add_command to set world state
    // Commands like set budget and move troops
    void add_command(Command* command);

    std::queue<Command*> pending_commands;
   private:
};

// Run world tick and pending commands
void main_loop(GameState&, Client*, SDL_Window*);
void start_client(int argc, char** argv);
#endif