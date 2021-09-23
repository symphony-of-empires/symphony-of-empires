#include "industry_view_nation.hpp"
#include "../../nation.hpp"
#include "../../world.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"
#include <memory>

extern char* tmpbuf;

IndustryViewNation::IndustryViewNation(GameState& _gs)
    : gs{_gs},
      page_nr{0},
      UI::Window(_gs.input.mouse_pos.first, _gs.input.mouse_pos.second, 609, 476) {
    // TODO: When a province resizes the industry vector it will break havoc!
    // TODO: Make the industries vector be a pointer instead!
    const size_t buttons_nr = 16;
    for (size_t i = 0; i < buttons_nr; i++) {
        buttons.push_back(new IndustryViewNationButton(gs, this, i));
    }

    on_update = ([](UI::Widget& w, void* data) {
        IndustryViewNation& state = dynamic_cast<IndustryViewNation&>(w);
        size_t cnt = 0, total_industries = 0;
        for (size_t i = 0; i < state.buttons_nr; i++) {
            state.buttons[i]->industry = nullptr;
        }

        size_t i = 0;
        for (auto& industry : state.gs.world->buildings) {
            if (cnt < state.page_nr * 16 || industry->type->is_factory == false) {
                cnt++;
                continue;
            }

            state.buttons[i]->industry = industry;
            i++;
            if (i >= state.buttons_nr)
                break;
        }
    });

    sprintf(tmpbuf, "Your nation's industries (page %zu)", (size_t)page_nr);
    text(tmpbuf);

    UI::Button* ok_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, this);
    ok_btn->text("OK");
    ok_btn->user_data = this;
    ok_btn->on_click = [](UI::Widget& w, void* data) {
        IndustryViewNation* state = (IndustryViewNation*)data;
        state->buttons.clear();
        state->gs.industry_view_nation = nullptr;
        delete w.parent;
    };

    UI::Button* prev_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, this);
    prev_btn->text("Previous");
    prev_btn->right_side_of(*ok_btn);
    prev_btn->user_data = this;
    prev_btn->on_click = [](UI::Widget&, void* data) {
        IndustryViewNation* ivn = (IndustryViewNation*)data;
        ivn->page_nr--;
        sprintf(tmpbuf, "Your nation's industries (page %zu)", (size_t)ivn->page_nr);
        ivn->text(tmpbuf);
    };

    UI::Button* next_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, this);
    next_btn->text("Next");
    next_btn->right_side_of(*prev_btn);
    next_btn->user_data = this;
    next_btn->on_click = [](UI::Widget&, void* data) {
        IndustryViewNation* ivn = (IndustryViewNation*)data;
        if (ivn->gs.world == nullptr) {
            print_error("ERR");
        }
        ivn->page_nr++;
        sprintf(tmpbuf, "Your nation's industries (page %zu)", (size_t)ivn->page_nr);
        ivn->text(tmpbuf);
    };
};

IndustryViewNationButton::IndustryViewNationButton(GameState& _gs, UI::Window* parent, size_t index)
    : gs{_gs},
      industry{nullptr},
      text{std::string()},
      UI::Button(0, 32 + (index * 24), parent->width, 24, parent) {
    //
    on_update = ([](UI::Widget& w, void* data) {
        IndustryViewNationButton& state = dynamic_cast<IndustryViewNationButton&>(w);
        if (state.industry != nullptr) {
            Building* industry = state.industry;
            sprintf(tmpbuf, "%s %4.2f %zu %zu %zu", industry->type->name.c_str(), industry->production_cost, industry->workers);
            w.text(tmpbuf);
        } else {
            w.text(" ");
        }
    });

    on_click = ([](UI::Widget& w, void* data) {
        IndustryViewNationButton& state = dynamic_cast<IndustryViewNationButton&>(w);
        new IndustryViewNationChart(state.gs, state.industry);
    });
}

IndustryViewNationChart::IndustryViewNationChart(GameState& _gs, Building* _industry)
    : gs{_gs},
      industry{_industry},
      UI::Window(0, 0, 320, 200) {
    text("Industry info");

    UI::Chart* industry_info_workers = new UI::Chart(0, 32, 120, 64, this);
    industry_info_workers->user_data = this;
    industry_info_workers->on_update = ([](UI::Widget& w, void* data) {
        IndustryViewNationChart* state = (IndustryViewNationChart*)data;
        if (state->industry == nullptr)
            return;

        UI::Chart& wc = dynamic_cast<UI::Chart&>(w);
        if (state->gs.world->time % 48 == 35) {
            wc.data.push_back((state->industry)->workers);
            if (wc.data.size() >= 30)
                wc.data.pop_front();
        }
    });
}