#include "client/interface/lobby.hpp"
#include "client/game_state.hpp"
#include "client/ui.hpp"
#include "world.hpp"
#include "client/map.hpp"
#include "client/camera.hpp"

using namespace Interface;

// Start screen
LobbySelectView::LobbySelectView(GameState& _gs) : gs{_gs}, curr_selected_nation{0} {
    select_country_lab = new UI::Label((gs.width / 2) - (320 / 2), 8, "Select a country");

    curr_country_btn = new UI::Button((gs.width / 2) - (320 / 2), gs.height - 48, 320, 24);
    curr_country_btn->text("!");
    curr_country_btn->user_data = this;
    curr_country_btn->on_click = ([](UI::Widget& w, void* data) {
        LobbySelectView* o = (LobbySelectView*)data;
        if (o->gs.curr_nation != nullptr) {
            // Didn't seem to be able to delete them in a callback so this will do
            o->next_country_btn->kill();
            o->next_country_btn = nullptr;
            o->prev_country_btn->kill();
            o->prev_country_btn = nullptr;
            o->select_country_lab->kill();
            o->select_country_lab = nullptr;
            o->curr_country_btn->kill();
            o->curr_country_btn = nullptr;
            o->gs.play_nation();
        }
    });

    next_country_btn = new UI::Button(0, gs.height - 48, 128, 24);
    next_country_btn->text("Next");
    next_country_btn->right_side_of(*curr_country_btn);
    next_country_btn->user_data = this;
    next_country_btn->on_click = ([](UI::Widget& w, void* data) {
        LobbySelectView* o = (LobbySelectView*)data;
        o->change_nation(o->curr_selected_nation + 1);
    });

    prev_country_btn = new UI::Button(0, gs.height - 48, 128, 24);
    prev_country_btn->text("Prev");
    prev_country_btn->left_side_of(*curr_country_btn);
    prev_country_btn->user_data = this;
    prev_country_btn->on_click = ([](UI::Widget& w, void* data) {
        LobbySelectView* o = (LobbySelectView*)data;
        o->change_nation(o->curr_selected_nation - 1);
    });
}

// Change nation in start screen
void LobbySelectView::change_nation(size_t id) {
    size_t old_id = curr_selected_nation;
    if (!g_world->nations.size()) {
        UI::Window* win = new UI::Window(gs.input.mouse_pos.first, gs.input.mouse_pos.second, 512, 32);
        win->text("No nations exist");
        return;
    }

    if (id >= g_world->nations.size()) {
        id = 0;
    }

    gs.curr_nation = gs.world->nations[id];
    if (id > old_id) {
        while (gs.curr_nation->exists() == false) {
            id++;
            if (id >= gs.world->nations.size()) {
                id = 0;
            }
            gs.curr_nation = gs.world->nations[id];
        }
    } else {
        while (gs.curr_nation->exists() == false) {
            id--;
            if (id >= gs.world->nations.size()) {
                id = gs.world->nations.size() - 1;
            }
            gs.curr_nation = gs.world->nations[id];
        }
    }

    curr_selected_nation = id;
    curr_country_btn->text(gs.curr_nation->name.c_str());
    const Province* capital = gs.curr_nation->capital;
    if (capital != nullptr) {
        gs.map->camera->position.x = capital->max_x;
        gs.map->camera->position.y = capital->max_y;
    }
}
