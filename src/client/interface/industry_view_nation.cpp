#include "industry_view_nation.hpp"

#include "../game_state.hpp"
#include "../ui.hpp"
#include "industry.hpp"
#include "nation.hpp"
#include "world.hpp"
extern char* tmpbuf;

IndustryViewNation::IndustryViewNation(GameState& _gs) : gs{_gs} {
}

void IndustryViewNation::show() {
    if (industry_view_nation_win != nullptr) {
        return;
    }

    // TODO: When a province resizes the industry vector it will break havoc!
    // TODO: Make the industries vector be a pointer instead!
    const size_t buttons_nr = 16;
    for (size_t i = 0; i < buttons_nr; i++) {
        buttons.push_back(IndustryViewNationButton(gs, industry_view_nation_win, i));
    }

    // View the provinces in a country - along with the population in them
    industry_view_nation_win = new UI::Window(gs.input.mouse_pos.first, gs.input.mouse_pos.second, 609, 476);
    industry_view_nation_win->user_data = this;
    industry_view_nation_win->on_update = ([](UI::Widget& w, void* data) {
        IndustryViewNation* ivn = (IndustryViewNation*)data;
        size_t e = ivn->page_nr * 16, i = 0;
        size_t cnt = 0, total_industries = 0;
        for (const auto& province : ivn->gs.curr_nation->owned_provinces) {
            total_industries += province->industries.size();
        }

        for (const auto& province : ivn->gs.curr_nation->owned_provinces) {
            for (auto& industry : province->industries) {
                if (cnt < ivn->page_nr * 16) {
                    cnt++;
                    continue;
                }

                if (e < total_industries) {
                    sprintf(tmpbuf, "%s %4.2f %zu %zu", industry.type->name.c_str(), industry.production_cost, industry.workers);

                    ivn->buttons[i].text = std::string(tmpbuf);
                    ivn->buttons[i].industry = &industry;
                } else {
                    ivn->buttons[i].text = std::string(" ");
                    ivn->buttons[i].industry = nullptr;
                }
                e++;
                i++;
                if (i >= buttons_nr)
                    break;
            }
            if (i >= buttons_nr)
                break;
        }
    });

    sprintf(tmpbuf, "Your province's industries (page %zu)", (size_t)page_nr);
    industry_view_nation_win->text(tmpbuf);

    UI::Button* ok_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, industry_view_nation_win);
    ok_btn->text("OK");
    ok_btn->user_data = this;
    ok_btn->on_click = [](UI::Widget& w, void* data) {
        IndustryViewNation* ivn = (IndustryViewNation*)data;
        ivn->buttons.clear();
        ivn->industry_view_nation_win = nullptr;
        delete w.parent;
    };

    UI::Button* prev_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, industry_view_nation_win);
    prev_btn->text("Previous");
    prev_btn->right_side_of(*ok_btn);
    prev_btn->user_data = this;
    prev_btn->on_click = [](UI::Widget&, void* data) {
        IndustryViewNation* ivn = (IndustryViewNation*)data;
        ivn->page_nr--;
        sprintf(tmpbuf, "Your province's industries (page %zu)", (size_t)ivn->page_nr);
        ivn->industry_view_nation_win->text(tmpbuf);
    };

    UI::Button* next_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, industry_view_nation_win);
    next_btn->text("Next");
    next_btn->right_side_of(*prev_btn);
    next_btn->user_data = this;
    next_btn->on_click = [](UI::Widget&, void* data) {
        IndustryViewNation* ivn = (IndustryViewNation*)data;
        ivn->page_nr++;
        sprintf(tmpbuf, "Your province's industries (page %zu)", (size_t)ivn->page_nr);
        ivn->industry_view_nation_win->text(tmpbuf);
    };
};

IndustryViewNationButton::IndustryViewNationButton(GameState& _gs, UI::Window* parent, size_t index) : gs{_gs} {
    UI::Button* elem = new UI::Button(0, 32 + (index * 24), parent->width, 24, parent);
    elem->user_data = this;
    elem->on_click = ([](UI::Widget& w, void* data) {
        IndustryViewNationButton* ivnb = (IndustryViewNationButton*)data;
        UI::Window* industry_info_win = new UI::Window(0, 0, 320, 200);
        industry_info_win->text("Industry info");

        UI::Chart* industry_info_workers = new UI::Chart(0, 32, 120, 64, industry_info_win);
        industry_info_workers->user_data = data;
        industry_info_workers->on_update = ([](UI::Widget& w, void* data) {
            IndustryViewNationButton* ivnb = (IndustryViewNationButton*)data;
            if (ivnb->industry == nullptr)
                return;

            UI::Chart& wc = dynamic_cast<UI::Chart&>(w);
            if (ivnb->gs.world->time % 48 == 35) {
                wc.data.push_back((ivnb->industry)->workers);
                if (wc.data.size() >= 30)
                    wc.data.pop_front();
            }
        });
    });
}