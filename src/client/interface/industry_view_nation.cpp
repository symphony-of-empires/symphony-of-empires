#include "../ui.hpp"

static UI::Window* industry_view_nation_win = nullptr;
static uint8_t industry_view_nation_page_num = 0;

#include "nation.hpp"
extern Nation* curr_nation;

#include "industry.hpp"

extern std::pair<int, int> mouse_pos;

#include "world.hpp"
extern World* g_world;

extern char* tmpbuf;
static void industry_view_nation(UI::Widget&, void*) {
    // Do not make duplicate windows
    if (industry_view_nation_win != nullptr) {
        return;
    }

    // View the provinces in a country - along with the population in them
    industry_view_nation_win = new UI::Window(mouse_pos.first, mouse_pos.second, 609, 476);
    industry_view_nation_win->on_update = ([](UI::Widget& w, void*) {
        size_t e = industry_view_nation_page_num * 16, i = 3;
        size_t cnt = 0, total_industries = 0;
        for (const auto& province : curr_nation->owned_provinces) {
            total_industries += province->industries.size();
        }

        for (const auto& province : curr_nation->owned_provinces) {
            for (auto& industry : province->industries) {
                if (cnt < industry_view_nation_page_num * 16) {
                    cnt++;
                    continue;
                }

                if (e < total_industries) {
                    sprintf(tmpbuf, "%s %4.2f %zu %zu", industry.type->name.c_str(), industry.production_cost, industry.workers);
                    w.children[i]->text(tmpbuf);
                    w.children[i]->user_data = &industry;
                } else {
                    w.children[i]->text(" ");
                    w.children[i]->user_data = nullptr;
                }
                e++;
                i++;
                if (i >= w.children.size())
                    break;
            }
            if (i >= w.children.size())
                break;
        }
    });

    sprintf(tmpbuf, "Your province's industries (page %zu)", (size_t)industry_view_nation_page_num);
    industry_view_nation_win->text(tmpbuf);

    UI::Button* ok_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, industry_view_nation_win);
    ok_btn->text("OK");
    ok_btn->on_click = [](UI::Widget& w, void*) {
        industry_view_nation_win = nullptr;
        delete w.parent;
    };

    UI::Button* prev_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, industry_view_nation_win);
    prev_btn->text("Previous");
    prev_btn->right_side_of(*ok_btn);
    prev_btn->on_click = [](UI::Widget&, void*) {
        industry_view_nation_page_num--;
        sprintf(tmpbuf, "Your province's industries (page %zu)", (size_t)industry_view_nation_page_num);
        industry_view_nation_win->text(tmpbuf);
    };

    UI::Button* next_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, industry_view_nation_win);
    next_btn->text("Next");
    next_btn->right_side_of(*prev_btn);
    next_btn->on_click = [](UI::Widget&, void*) {
        industry_view_nation_page_num++;
        sprintf(tmpbuf, "Your province's industries (page %zu)", (size_t)industry_view_nation_page_num);
        industry_view_nation_win->text(tmpbuf);
    };

    // TODO: When a province resizes the industry vector it will break havoc!
    // TODO: Make the industries vector be a pointer instead!
    for (size_t i = 0; i < 16; i++) {
        UI::Button* elem = new UI::Button(0, 32 + (i * 24), industry_view_nation_win->width, 24, industry_view_nation_win);
        elem->on_click = ([](UI::Widget& w, void* data) {
            UI::Window* industry_info_win = new UI::Window(0, 0, 320, 200);
            industry_info_win->text("Industry info");

            UI::Chart* industry_info_workers = new UI::Chart(0, 32, 120, 64, industry_info_win);
            industry_info_workers->user_data = data;
            industry_info_workers->on_update = ([](UI::Widget& w, void* data) {
                if (data == nullptr)
                    return;

                UI::Chart& wc = dynamic_cast<UI::Chart&>(w);
                if (g_world->time % 48 == 35) {
                    wc.data.push_back(((Industry*)data)->workers);
                    if (wc.data.size() >= 30)
                        wc.data.pop_front();
                }
            });
        });
    }
}
