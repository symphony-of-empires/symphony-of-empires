#include "select_nation.hpp"

#include "../ui.hpp"

// Start screen
SelectNation::SelectNation(GameState& _gs) : gs{_gs}, curr_selected_nation{0} {
    select_country_btn = new UI::Button((gs.width / 2) - (320 / 2), 8, 320, 38);
    select_country_btn->text("Select a country");

    curr_country_btn = new UI::Button((gs.width / 2) - (320 / 2), gs.height - 128, 320, 38);
    curr_country_btn->text("!");

    next_country_btn = new UI::Button(0, gs.height - 128, 128, 38);
    next_country_btn->text("Next");
    next_country_btn->right_side_of(*curr_country_btn);
    next_country_btn->user_data = this;
    next_country_btn->on_click = (UI::Callback)next_nation_cb;

    prev_country_btn = new UI::Button(0, gs.height - 128, 128, 38);
    prev_country_btn->text("Prev");
    prev_country_btn->left_side_of(*curr_country_btn);
    next_country_btn->user_data = this;
    next_country_btn->on_click = (UI::Callback)prev_nation_cb;

    play_btn = new UI::Button((gs.width / 2) - (320 / 2), gs.height - 38, 320, 38);
    play_btn->text("Play");
    play_btn->user_data = this;
    play_btn->on_click = (UI::Callback)play_nation_cb;
}

void SelectNation::next_nation_cb(UI::Widget& w, SelectNation* data) {
    data->change_nation(data->curr_selected_nation + 1);
}

void SelectNation::prev_nation_cb(UI::Widget& w, SelectNation* data) {
    data->change_nation(data->curr_selected_nation - 1);
}

void SelectNation::play_nation_cb(UI::Widget& w, SelectNation* data) {
    if (data->gs.curr_nation != nullptr) {
        // delete data->next_country_btn;
        // delete data->prev_country_btn;
        // delete data->play_btn;
        // delete data->select_country_btn;
        // delete data->curr_country_btn;
        data->next_country_btn->kill();
        data->next_country_btn = nullptr;
        data->prev_country_btn->kill();
        data->prev_country_btn = nullptr;
        data->play_btn->kill();
        data->play_btn = nullptr;
        data->select_country_btn->kill();
        data->select_country_btn = nullptr;
        data->curr_country_btn->kill();
        data->curr_country_btn = nullptr;
        data->gs.play_nation();
    }
}

// Change nation in start screen
void SelectNation::change_nation(size_t id) {
    size_t old_id = curr_selected_nation;
    if (!g_world->nations.size()) {
        UI::Window* win = new UI::Window(gs.input.mouse_pos.first, gs.input.mouse_pos.second, 512, 32);
        win->text("No selectable countries!!");
        return;
    }

    curr_selected_nation = id;
    if (curr_selected_nation >= g_world->nations.size()) {
        curr_selected_nation = 0;
    }

    gs.curr_nation = gs.world->nations[curr_selected_nation];
    if (gs.curr_nation->exists() == false) {
        if (curr_selected_nation > old_id) {
            while (gs.curr_nation->exists() == false) {
                curr_selected_nation++;
                if (curr_selected_nation >= gs.world->nations.size()) {
                    curr_selected_nation = 0;
                }
                gs.curr_nation = gs.world->nations[curr_selected_nation];
            }
        } else {
            while (gs.curr_nation->exists() == false) {
                curr_selected_nation--;
                if (curr_selected_nation >= gs.world->nations.size()) {
                    curr_selected_nation = gs.world->nations.size() - 1;
                }
                gs.curr_nation = gs.world->nations[curr_selected_nation];
            }
        }
    }
    curr_country_btn->text(gs.curr_nation->name.c_str());

    const Province* capital = gs.curr_nation->capital;
    if (capital != nullptr) {
        gs.cam.position.x = capital->max_x;
        gs.cam.position.y = capital->max_y;
    }
}
