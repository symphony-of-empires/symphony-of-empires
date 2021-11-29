#include "client/interface/top_window.hpp"
#include "client/game_state.hpp"
#include "path.hpp"
#include "client/map.hpp"
#include "world.hpp"
#include "client/render/texture.hpp"

using namespace Interface;

TopWindow::TopWindow(GameState& _gs)
    : gs{ _gs },
    UI::Window(0, _gs.height - 128, _gs.width, 128)
{
    this->is_scroll = false;
    this->is_pinned = true;

    const UnifiedRender::Texture& top_win_chart_tex = g_texture_manager->load_texture(Path::get("ui/top_win_chart.png"));
    const UnifiedRender::Texture& icon_prestige_tex = g_texture_manager->load_texture(Path::get("ui/icons/prestige.png"));
    const UnifiedRender::Texture& icon_economy_score_tex = g_texture_manager->load_texture(Path::get("ui/icons/economy_score.png"));
    const UnifiedRender::Texture& icon_militar_score_tex = g_texture_manager->load_texture(Path::get("ui/icons/militar_score.png"));
    const UnifiedRender::Texture& icon_naval_score_tex = g_texture_manager->load_texture(Path::get("ui/icons/naval_score.png"));
    const UnifiedRender::Texture& icon_money_tex = g_texture_manager->load_texture(Path::get("ui/icons/money.png"));
    const UnifiedRender::Texture& icon_militancy_tex = g_texture_manager->load_texture(Path::get("ui/icons/militancy.png"));
    const UnifiedRender::Texture& icon_big_brain_tex = g_texture_manager->load_texture(Path::get("ui/icons/big_brain.png"));
    const UnifiedRender::Texture& icon_population_tex = g_texture_manager->load_texture(Path::get("ui/icons/population.png"));

    gdp_chart = new UI::Chart(384, 24, 128, 64, this);
    gdp_chart->text("GDP");
    gdp_chart->current_texture = &top_win_chart_tex;
    gdp_chart->data.clear();
    gdp_chart->on_click = (UI::Callback)([](UI::Widget& w, void*) {
        TopWindow& twin = static_cast<TopWindow&>(*w.parent);
    });
    gdp_chart->tooltip = new UI::Tooltip(gdp_chart, 512, 24);
    gdp_chart->tooltip->text("GDP is the total sum of all products within your nation");

    pop_chart = new UI::Chart(0, 24, 128, 64, this);
    pop_chart->right_side_of(*gdp_chart);
    pop_chart->text("Population");
    pop_chart->current_texture = &top_win_chart_tex;
    pop_chart->data.clear();
    pop_chart->on_click = (UI::Callback)([](UI::Widget& w, void*) {
        TopWindow& twin = static_cast<TopWindow&>(*w.parent);

    });
    pop_chart->tooltip = new UI::Tooltip(pop_chart, 512, 24);
    pop_chart->tooltip->text("The total population of your nation");

    hdi_chart = new UI::Chart(0, 24, 128, 64, this);
    hdi_chart->text("HDI");
    hdi_chart->right_side_of(*pop_chart);
    hdi_chart->current_texture = &top_win_chart_tex;
    hdi_chart->data.clear();
    hdi_chart->on_click = (UI::Callback)([](UI::Widget& w, void*) {
        TopWindow& twin = static_cast<TopWindow&>(*w.parent);

    });
    hdi_chart->tooltip = new UI::Tooltip(hdi_chart, 512, 24);
    hdi_chart->tooltip->text("Average life quality/human development");

    UI::Image* current_flag = new UI::Image(0, 24, 128, 96, gs.map->nation_flags[g_world->get_id(gs.curr_nation)], this);

    money_icon = new UI::Image(128, 24 + (24 * 0), 24, 24, &icon_money_tex, this);
    money_lab = new UI::Label(0, 24 + (24 * 0), "?", this);
    money_lab->right_side_of(*money_icon);
    money_lab->tooltip = new UI::Tooltip(money_lab, 512, 24);
    money_lab->tooltip->text("Money");

    prestige_icon = new UI::Image(128, 24 + (24 * 1), 24, 24, &icon_prestige_tex, this);
    prestige_lab = new UI::Label(0, 24 + (24 * 1), "?", this);
    prestige_lab->right_side_of(*prestige_icon);
    prestige_lab->tooltip = new UI::Tooltip(prestige_lab, 512, 24);
    prestige_lab->tooltip->text("Prestige");

    economy_icon = new UI::Image(128, 24 + (24 * 2), 24, 24, &icon_economy_score_tex, this);
    economy_lab = new UI::Label(0, 24 + (24 * 2), "?", this);
    economy_lab->right_side_of(*economy_icon);
    economy_lab->tooltip = new UI::Tooltip(economy_lab, 512, 24);
    economy_lab->tooltip->text("Economy score");

    big_brain_icon = new UI::Image(256, 24 + (24 * 0), 24, 24, &icon_big_brain_tex, this);
    big_brain_lab = new UI::Label(0, 24 + (24 * 0), "?", this);
    big_brain_lab->right_side_of(*big_brain_icon);
    big_brain_lab->tooltip = new UI::Tooltip(big_brain_lab, 512, 24);
    big_brain_lab->tooltip->text("Conciousness");

    militancy_icon = new UI::Image(256, 24 + (24 * 1), 24, 24, &icon_militancy_tex, this);
    militancy_lab = new UI::Label(0, 24 + (24 * 1), "?", this);
    militancy_lab->right_side_of(*militancy_icon);
    militancy_lab->tooltip = new UI::Tooltip(militancy_lab, 512, 24);
    militancy_lab->tooltip->text("Militancy");

    population_icon = new UI::Image(256, 24 + (24 * 2), 24, 24, &icon_population_tex, this);
    population_lab = new UI::Label(0, 24 + (24 * 2), "?", this);
    population_lab->right_side_of(*population_icon);
    population_lab->tooltip = new UI::Tooltip(population_lab, 512, 24);
    population_lab->tooltip->text("Population");

    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TopWindow&>(w);

        if(g_world->time % 48) return;

        o.text(
            std::to_string(o.gs.world->time / 48 / 30 / 12)
            + "/"
            + std::to_string(o.gs.world->time / 48 / 30 % 12)
            + "/"
            + std::to_string(o.gs.world->time % 48)
            + " - "
            + o.gs.curr_nation->get_client_hint().alt_name
            + " - "
            + o.gs.curr_nation->ideology->name
        );

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
        if(o.gdp_chart->data.size() >= 30) o.gdp_chart->data.pop_front();

        o.pop_chart->data.push_back(total_pop);
        if(o.pop_chart->data.size() >= 30) o.pop_chart->data.pop_front();

        o.hdi_chart->data.push_back(living_std);
        if(o.hdi_chart->data.size() >= 30) o.hdi_chart->data.pop_front();

        o.militancy_lab->text(std::to_string(militancy * 1000.));
        o.big_brain_lab->text(std::to_string(con * 1000.));
        UI::Tooltip* tooltip = new UI::Tooltip(o.big_brain_icon, 100, 100);
        new UI::Label(0, 24, "tooltip", tooltip);
        o.big_brain_lab->set_tooltip(tooltip);
        int prestige = (int)o.gs.curr_nation->prestige;
        o.prestige_lab->text(std::to_string(prestige));
        int economy_score = (int)o.gs.curr_nation->economy_score;
        o.economy_lab->text(std::to_string(economy_score));
        int budget = (int)o.gs.curr_nation->budget;
        o.money_lab->text(std::to_string(budget));
        std::string pop_amount;
        // We should add a format_text() func later but this will do for now
        if(total_pop < 1000) {
            pop_amount = std::to_string((int)total_pop);
        }
        else if(total_pop < 1000 * 1000) {
            pop_amount = std::to_string((int)total_pop / 1000) + " K";
        }
        else if(total_pop < 1000 * 1000 * 1000) {
            pop_amount = std::to_string((int)total_pop / (1000 * 1000)) + " M";
        }
        else {
            pop_amount = std::to_string((int)total_pop / (1000 * 1000 * 1000)) + " B";
        }
        o.population_lab->text(pop_amount);
    });
}