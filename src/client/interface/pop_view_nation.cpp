#include "pop_view_nation.hpp"
#include "../game_state.hpp"
  
extern char* tmpbuf;
PopViewNation::PopViewNation(GameState& _gs) : gs{_gs} {
}

void PopViewNation::show() {
    if (pop_view_nation_win != nullptr) {
        return;
    }

    pop_view_nation_win = new UI::Window(gs.input.mouse_pos.first, gs.input.mouse_pos.second, 609, 476);

    sprintf(tmpbuf, "Your province's POPs (page %zu)", (size_t)page_nr);
    pop_view_nation_win->text(tmpbuf);

    UI::Button* ok_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, pop_view_nation_win);
    ok_btn->text("OK");
    ok_btn->user_data = this;
    ok_btn->on_click = [](UI::Widget& w, void* data) {
        PopViewNation* pvn = (PopViewNation*)data;
        pvn->pop_view_nation_win = nullptr;
        delete w.parent;
    };

    UI::Button* prev_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, pop_view_nation_win);
    prev_btn->text("Previous");
    prev_btn->right_side_of(*ok_btn);
    prev_btn->user_data = this;
    prev_btn->on_click = [](UI::Widget&, void* data) {
        PopViewNation* pvn = (PopViewNation*)data;
        pvn->page_nr--;
        sprintf(tmpbuf, "Your province's POPs (page %zu)", (size_t)pvn->page_nr);
        pvn->pop_view_nation_win->text(tmpbuf);
    };

    UI::Button* next_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, pop_view_nation_win);
    next_btn->text("Next");
    next_btn->right_side_of(*prev_btn);
    next_btn->user_data = this;
    next_btn->on_click = [](UI::Widget&, void* data) {
        PopViewNation* pvn = (PopViewNation*)data;
        pvn->page_nr++;
        sprintf(tmpbuf, "Your province's POPs (page %zu)", (size_t)pvn->page_nr);
        pvn->pop_view_nation_win->text(tmpbuf);
    };

    for (size_t i = 0; i < 14; i++) {
        UI::Button* elem = new UI::Button(0, 32 + (i * 24), pop_view_nation_win->width, 24, pop_view_nation_win);
    }

    pop_view_nation_win->on_update = (UI::Callback)on_update;
}

void PopViewNation::on_update(UI::Widget& w, PopViewNation* data) {
    size_t e = data->page_nr * 16, i = 3;
    size_t cnt = 0, total_pops = 0;
    for (const auto& province : data->gs.curr_nation->owned_provinces) {
        total_pops += province->pops.size();
    }

    for (const auto& province : data->gs.curr_nation->owned_provinces) {
        for (const auto& pop : province->pops) {
            if (cnt < data->page_nr * 16) {
                cnt++;
                continue;
            }

            if (e < total_pops) {
                sprintf(tmpbuf, "%s %zu %s %s %s %.2f$ %2.2f", province->name.c_str(), pop.size, g_world->cultures[pop.culture_id]->name.c_str(), g_world->religions[pop.religion_id]->name.c_str(), g_world->pop_types[pop.type_id]->name.c_str(), pop.budget, pop.life_needs_met);
                w.children[i]->text(tmpbuf);
            } else {
                sprintf(tmpbuf, " ");
                w.children[i]->text(tmpbuf);
            }
            e++;
            i++;
            if (i >= w.children.size())
                break;
        }
        if (i >= w.children.size())
            break;
    }
}