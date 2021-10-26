#include "top_window.hpp"

#include "../game_state.hpp"

using namespace Interface;

TopWindow::TopWindow(GameState& _gs)
    : gs{_gs},
      UI::Window(0, 0, 800, 275)
{
    const UnifiedRender::Texture& top_win_chart_tex = g_texture_manager->load_texture(Path::get("ui/top_win_chart.png"));
    const UnifiedRender::Texture& icon_prestige_tex = g_texture_manager->load_texture(Path::get("ui/icons/prestige.png"));
    const UnifiedRender::Texture& icon_economy_score_tex = g_texture_manager->load_texture(Path::get("ui/icons/economy_score.png"));
    const UnifiedRender::Texture& icon_militar_score_tex = g_texture_manager->load_texture(Path::get("ui/icons/militar_score.png"));
    const UnifiedRender::Texture& icon_naval_score_tex = g_texture_manager->load_texture(Path::get("ui/icons/naval_score.png"));
    const UnifiedRender::Texture& icon_money_tex = g_texture_manager->load_texture(Path::get("ui/icons/money.png"));
    const UnifiedRender::Texture& icon_militancy_tex = g_texture_manager->load_texture(Path::get("ui/icons/militancy.png"));
    const UnifiedRender::Texture& icon_big_brain_tex = g_texture_manager->load_texture(Path::get("ui/icons/big_brain.png"));
    const UnifiedRender::Texture& icon_population_tex = g_texture_manager->load_texture(Path::get("ui/icons/population.png"));

    this->text("Overview");

    gdp_chart = new UI::Chart(504, 43, top_win_chart_tex.width, top_win_chart_tex.height, this);
    gdp_chart->text("GDP");
    gdp_chart->current_texture = &top_win_chart_tex;
    gdp_chart->data.clear();
    gdp_chart->on_click = (UI::Callback)([](UI::Widget& w, void*) {
        TopWindow& twin = static_cast<TopWindow&>(*w.parent);
        
    });

    pop_chart = new UI::Chart(504, 120, top_win_chart_tex.width, top_win_chart_tex.height, this);
    pop_chart->text("Population");
    pop_chart->current_texture = &top_win_chart_tex;
    pop_chart->data.clear();
    pop_chart->on_click = (UI::Callback)([](UI::Widget& w, void*) {
        TopWindow& twin = static_cast<TopWindow&>(*w.parent);
        
    });

    hdi_chart = new UI::Chart(504, 197, top_win_chart_tex.width, top_win_chart_tex.height, this);
    hdi_chart->text("HDI");
    hdi_chart->current_texture = &top_win_chart_tex;
    hdi_chart->data.clear();
    hdi_chart->on_click = (UI::Callback)([](UI::Widget& w, void*) {
        TopWindow& twin = static_cast<TopWindow&>(*w.parent);
        
    });

    UI::Image* current_flag = new UI::Image(9, 43, 188, 87, gs.map->nation_flags[g_world->get_id(gs.curr_nation)], this);

    money_icon = new UI::Image(209, 43 + (28 * 0), icon_money_tex.width, icon_money_tex.height, &icon_money_tex, this);
    money_lab = new UI::Label(0, 43 + (28 * 0), "?", this);
    money_lab->right_side_of(*money_icon);

    prestige_icon = new UI::Image(209, 43 + (28 * 1), icon_prestige_tex.width, icon_prestige_tex.height, &icon_prestige_tex, this);
    prestige_lab = new UI::Label(0, 43 + (28 * 1), "?", this);
    prestige_lab->right_side_of(*prestige_icon);

    economy_icon = new UI::Image(209, 43 + (28 * 2), icon_economy_score_tex.width, icon_economy_score_tex.height, &icon_economy_score_tex, this);
    economy_lab = new UI::Label(0, 43 + (28 * 2), "?", this);
    economy_lab->right_side_of(*economy_icon);

    big_brain_icon = new UI::Image(209, 43 + (28 * 3), icon_big_brain_tex.width, icon_big_brain_tex.height, &icon_big_brain_tex, this);
    big_brain_lab = new UI::Label(0, 43 + (28 * 3), "?", this);
    big_brain_lab->right_side_of(*big_brain_icon);

    militancy_icon = new UI::Image(209, 43 + (28 * 4), icon_militancy_tex.width, icon_militancy_tex.height, &icon_militancy_tex, this);
    militancy_lab = new UI::Label(0, 43 + (28 * 4), "?", this);
    militancy_lab->right_side_of(*militancy_icon);

    population_icon = new UI::Image(209, 43 + (28 * 5), icon_population_tex.width, icon_population_tex.height, &icon_population_tex, this);
    population_lab = new UI::Label(0, 43 + (28 * 5), "?", this);
    population_lab->right_side_of(*population_icon);

    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TopWindow&>(w);

        if(g_world->time % 48) {
            return;
        }

        size_t total_pop = 0, n_pops = 0;
        double living_std = 0.f, gdp = 0.f, militancy = 0.f, con = 0.f;
        for(const auto& province : o.gs.curr_nation->owned_provinces) {
            for(const auto& product : g_world->products) {
                gdp += product->price * province->stockpile.at(g_world->get_id(product));
            }

            for(const auto& pop : province->pops) {
                total_pop += pop.size;
                militancy += pop.militancy;
                con += pop.con;
                living_std += pop.life_needs_met;
                n_pops++;
            }
        }
        militancy /= total_pop;
        con /= total_pop;

        o.gdp_chart->data.push_back(gdp);
        if(o.gdp_chart->data.size() >= 30)
            o.gdp_chart->data.pop_front();

        o.pop_chart->data.push_back(total_pop);
        if(o.pop_chart->data.size() >= 30)
            o.pop_chart->data.pop_front();
        
        o.hdi_chart->data.push_back(living_std);
        if(o.hdi_chart->data.size() >= 30)
            o.hdi_chart->data.pop_front();
        
        o.militancy_lab->text(std::to_string(militancy));
        o.big_brain_lab->text(std::to_string(con));
        o.prestige_lab->text(std::to_string(o.gs.curr_nation->prestige));
        o.economy_lab->text(std::to_string(o.gs.curr_nation->economy_score));
        o.money_lab->text(std::to_string(o.gs.curr_nation->budget));
        o.population_lab->text(std::to_string(total_pop));
    });
}