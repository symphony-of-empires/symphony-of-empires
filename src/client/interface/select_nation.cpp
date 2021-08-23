#include "select_nation.hpp"

#include "../ui.hpp"

// Start screen
SelectNation::SelectNation(GameState& _gs) : gs{_gs} {
    UI::Button* select_country_btn = new UI::Button((gs.width / 2) - (320 / 2), 8, 320, 38);
    select_country_btn->text("Select a country");

    curr_country_btn = new UI::Button((gs.width / 2) - (320 / 2), gs.height - 128, 320, 38);
    curr_country_btn->text("!");
    UI::Button* next_country_btn = new UI::Button(0, gs.height - 128, 128, 38);
    next_country_btn->text("Next");
    next_country_btn->right_side_of(*curr_country_btn);
    next_country_btn->user_data = this;
    next_country_btn->on_click = (UI::Callback)next_nation_cb;
    UI::Button* prev_country_btn = new UI::Button(0, gs.height - 128, 128, 38);
    prev_country_btn->text("Prev");
    prev_country_btn->left_side_of(*curr_country_btn);
    next_country_btn->user_data = this;
    next_country_btn->on_click = (UI::Callback)prev_nation_cb;

    UI::Button* play_btn = new UI::Button((gs.width / 2) - (320 / 2), gs.height - 38, 320, 38);
    play_btn->text("Play");
    // TODO Start game action
    // play_btn->on_click = &play_nation;
}

void SelectNation::next_nation_cb(UI::Widget& w, SelectNation* data) {
    data->change_nation(data->curr_selected_nation + 1);
}

void SelectNation::prev_nation_cb(UI::Widget& w, SelectNation* data) {
    data->change_nation(data->curr_selected_nation - 1);
}

// Change nation in start screen
void SelectNation::change_nation(size_t id) {
    size_t old_id = curr_selected_nation;
    if (!g_world->nations.size()) {
        UI::Window* win = new UI::Window(gs.input.mouse_pos.first, gs.input.mouse_pos.second, 512, 32);
        win->text("No selectable countries!!");
        return;
    }

    curr_selected_nation= id;
    if (curr_selected_nation>= g_world->nations.size()) {
        curr_selected_nation = 0;
    }

    Nation* curr_nation = gs.curr_nation;
    curr_nation = gs.world->nations[curr_selected_nation];
    if (curr_nation->exists() == false) {
        if (curr_selected_nation > old_id) {
            while (curr_nation->exists() == false) {
                curr_selected_nation++;
                if (curr_selected_nation >= gs.world->nations.size()) {
                    curr_selected_nation = 0;
                }
                curr_nation = gs.world->nations[curr_selected_nation];
            }
        } else {
            while (curr_nation->exists() == false) {
                curr_selected_nation--;
                if (curr_selected_nation >= gs.world->nations.size()) {
                    curr_selected_nation = gs.world->nations.size() - 1;
                }
                curr_nation = gs.world->nations[curr_selected_nation];
            }
        }
    }
    curr_country_btn->text(curr_nation->name.c_str());

    const Province* capital = curr_nation->capital;
    if (capital != nullptr) {
        gs.cam.position.x = capital->max_x;
        gs.cam.position.y = capital->max_y;
    }
}
