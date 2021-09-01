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
    next_country_btn->on_click = (UI::Callback)([](UI::Widget& w, void* data) {
        SelectNation* d = (SelectNation*)data;
        d->change_nation(d->curr_selected_nation + 1);
    });

    prev_country_btn = new UI::Button(0, gs.height - 128, 128, 38);
    prev_country_btn->text("Prev");
    prev_country_btn->left_side_of(*curr_country_btn);
    prev_country_btn->user_data = this;
    prev_country_btn->on_click = (UI::Callback)([](UI::Widget& w, void* data) {
        SelectNation* d = (SelectNation*)data;
        d->change_nation(d->curr_selected_nation - 1);
    });

    play_btn = new UI::Button((gs.width / 2) - (320 / 2), gs.height - 38, 320, 38);
    play_btn->text("Play");
    play_btn->user_data = this;
    play_btn->on_click = (UI::Callback)([](UI::Widget& w, void* data) {
        SelectNation* d = (SelectNation*)data;
        if (d->gs.curr_nation != nullptr) {
            // Didn't seem to be able to delete them in a callback so this will do
            d->next_country_btn->kill();
            d->next_country_btn = nullptr;
            d->prev_country_btn->kill();
            d->prev_country_btn = nullptr;
            d->play_btn->kill();
            d->play_btn = nullptr;
            d->select_country_btn->kill();
            d->select_country_btn = nullptr;
            d->curr_country_btn->kill();
            d->curr_country_btn = nullptr;
            d->gs.play_nation();
        } else {
            UI::Window* win = new UI::Window(d->gs.input.mouse_pos.first, d->gs.input.mouse_pos.second, 512, 32);
            win->text("Select a nation!!!");
            return;
        }
    });
}

// Change nation in start screen
void SelectNation::change_nation(size_t id) {
    size_t old_id = curr_selected_nation;
    if (!g_world->nations.size()) {
        UI::Window* win = new UI::Window(gs.input.mouse_pos.first, gs.input.mouse_pos.second, 512, 32);
        win->text("No selectable countries!!!");
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
        gs.cam.position.x = capital->max_x;
        gs.cam.position.y = capital->max_y;
    }
}
