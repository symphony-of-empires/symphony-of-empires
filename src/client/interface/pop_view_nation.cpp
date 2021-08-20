#include "../ui.hpp"

#include "nation.hpp"
extern Nation* curr_nation;

extern std::pair<int, int> mouse_pos;

#include "world.hpp"
extern World* g_world;

static UI::Window* pop_view_nation_win = nullptr;
static uint8_t pop_view_nation_page_num = 0;

static void pop_view_nation(UI::Widget&, void *) {
    // Do not make duplicate windows
    if(pop_view_nation_win != nullptr) {
        return;
    }
    
    // View the provinces in a country - along with the population in them
    pop_view_nation_win = new UI::Window(mouse_pos.first, mouse_pos.second, 609, 476);

    sprintf(tmpbuf, "Your province's POPs (page %zu)", (size_t)pop_view_nation_page_num);
    pop_view_nation_win->text(tmpbuf);
    
    UI::Button* ok_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, pop_view_nation_win);
    ok_btn->text("OK");
    ok_btn->on_click = [](UI::Widget& w, void *) {
        pop_view_nation_win = nullptr;
        delete w.parent;
    };
    
    UI::Button* prev_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, pop_view_nation_win);
    prev_btn->text("Previous");
    prev_btn->right_side_of(*ok_btn);
    prev_btn->on_click = [](UI::Widget&, void *) {
        pop_view_nation_page_num--;
        sprintf(tmpbuf, "Your province's POPs (page %zu)", (size_t)pop_view_nation_page_num);
        pop_view_nation_win->text(tmpbuf);
    };
    
    UI::Button* next_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, pop_view_nation_win);
    next_btn->text("Next");
    next_btn->right_side_of(*prev_btn);
    next_btn->on_click = [](UI::Widget&, void *) {
        pop_view_nation_page_num++;
        sprintf(tmpbuf, "Your province's POPs (page %zu)", (size_t)pop_view_nation_page_num);
        pop_view_nation_win->text(tmpbuf);
    };
    
    for(size_t i = 0; i < 14; i++) {
        UI::Button* elem = new UI::Button(0, 32 + (i * 24), pop_view_nation_win->width, 24, pop_view_nation_win);
    }

    pop_view_nation_win->on_update = ([](UI::Widget& w, void*) {
        size_t e = pop_view_nation_page_num * 16, i = 3;
        size_t cnt = 0, total_pops = 0;
        for(const auto& province: curr_nation->owned_provinces) {
            total_pops += province->pops.size();
        }

        for(const auto& province: curr_nation->owned_provinces) {
            for(const auto& pop: province->pops) {
                if(cnt < pop_view_nation_page_num * 16) {
                    cnt++;
                    continue;
                }

                if(e < total_pops) {
                    sprintf(tmpbuf, "%s %zu %s %s %s %.2f$ %2.2f", province->name.c_str(), pop.size, g_world->cultures[pop.culture_id]->name.c_str(), g_world->religions[pop.religion_id]->name.c_str(), g_world->pop_types[pop.type_id]->name.c_str(), pop.budget, pop.life_needs_met);
                    w.children[i]->text(tmpbuf);
                } else {
                    sprintf(tmpbuf, " ");
                    w.children[i]->text(tmpbuf);
                }
                e++; i++;
                if(i >= w.children.size())
                    break;
            }
            if(i >= w.children.size())
                break;
        }
    });
}