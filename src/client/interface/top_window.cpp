#include "top_window.hpp"

TopWindow::TopWindow(GameState& _gs) : gs{_gs} {
    const UnifiedRender::Texture& top_win_chart_tex = g_texture_manager->load_texture(Path::get("ui/top_win_chart.png"));
    const UnifiedRender::Texture& icon_prestige_tex = g_texture_manager->load_texture(Path::get("ui/icons/prestige.png"));
    const UnifiedRender::Texture& icon_economy_score_tex = g_texture_manager->load_texture(Path::get("ui/icons/economy_score.png"));
    const UnifiedRender::Texture& icon_militar_score_tex = g_texture_manager->load_texture(Path::get("ui/icons/militar_score.png"));
    const UnifiedRender::Texture& icon_naval_score_tex = g_texture_manager->load_texture(Path::get("ui/icons/naval_score.png"));
    const UnifiedRender::Texture& icon_money_tex = g_texture_manager->load_texture(Path::get("ui/icons/money.png"));
    const UnifiedRender::Texture& icon_militancy_tex = g_texture_manager->load_texture(Path::get("ui/icons/militancy.png"));
    const UnifiedRender::Texture& icon_big_brain_tex = g_texture_manager->load_texture(Path::get("ui/icons/big_brain.png"));
    const UnifiedRender::Texture& icon_population_tex = g_texture_manager->load_texture(Path::get("ui/icons/population.png"));

    top_win = new UI::Window(0, 0, 800, 275);
    top_win->text("Overview");

    gdp_chart = new UI::Chart(504, 43, top_win_chart_tex.width, top_win_chart_tex.height, top_win);
    gdp_chart->text("GDP");
    gdp_chart->current_texture = &top_win_chart_tex;
    gdp_chart->data.clear();
    gdp_chart->user_data = (void*)this;
    gdp_chart->on_click = (UI::Callback)([](UI::Widget& w, void* data) {
        TopWindow* d = (TopWindow*)data;
        d->gs.industry_view_nation = new IndustryViewNation(d->gs);
    });

    pop_chart = new UI::Chart(504, 120, top_win_chart_tex.width, top_win_chart_tex.height, top_win);
    pop_chart->text("Population");
    pop_chart->current_texture = &top_win_chart_tex;
    pop_chart->data.clear();
    pop_chart->user_data = (void*)this;
    pop_chart->on_click = (UI::Callback)([](UI::Widget& w, void* data) {
        TopWindow* d = (TopWindow*)data;
        d->gs.pop_view_nation->show();
    });

    hdi_chart = new UI::Chart(504, 197, top_win_chart_tex.width, top_win_chart_tex.height, top_win);
    hdi_chart->text("HDI");
    hdi_chart->current_texture = &top_win_chart_tex;
    hdi_chart->data.clear();

    extern void ui_reform_policies(UI::Widget&, void*);
    hdi_chart->user_data = (void*)this;
    hdi_chart->on_click = (UI::Callback)([](UI::Widget& w, void* data) {
        TopWindow* d = (TopWindow*)data;
        d->gs.ui_reform->show();
    });

    UI::Image* current_flag = new UI::Image(9, 43, 188, 87, gs.map->nation_flags[g_world->get_id(gs.curr_nation)], top_win);

    money_icon = new UI::Image(209, 43 + (28 * 0), icon_money_tex.width, icon_money_tex.height, &icon_money_tex, top_win);
    money_lab = new UI::Label(0, 43 + (28 * 0), "?", top_win);
    money_lab->right_side_of(*money_icon);

    prestige_icon = new UI::Image(209, 43 + (28 * 1), icon_prestige_tex.width, icon_prestige_tex.height, &icon_prestige_tex, top_win);
    prestige_lab = new UI::Label(0, 43 + (28 * 1), "?", top_win);
    prestige_lab->right_side_of(*prestige_icon);

    economy_icon = new UI::Image(209, 43 + (28 * 2), icon_economy_score_tex.width, icon_economy_score_tex.height, &icon_economy_score_tex, top_win);
    economy_lab = new UI::Label(0, 43 + (28 * 2), "?", top_win);
    economy_lab->right_side_of(*economy_icon);

    big_brain_icon = new UI::Image(209, 43 + (28 * 3), icon_big_brain_tex.width, icon_big_brain_tex.height, &icon_big_brain_tex, top_win);
    big_brain_lab = new UI::Label(0, 43 + (28 * 3), "?", top_win);
    big_brain_lab->right_side_of(*big_brain_icon);

    militancy_icon = new UI::Image(209, 43 + (28 * 4), icon_militancy_tex.width, icon_militancy_tex.height, &icon_militancy_tex, top_win);
    militancy_lab = new UI::Label(0, 43 + (28 * 4), "?", top_win);
    militancy_lab->right_side_of(*militancy_icon);

    population_icon = new UI::Image(209, 43 + (28 * 5), icon_population_tex.width, icon_population_tex.height, &icon_population_tex, top_win);
    population_lab = new UI::Label(0, 43 + (28 * 5), "?", top_win);
    population_lab->right_side_of(*population_icon);
}

void TopWindow::show_industry_cb(UI::Widget& w, TopWindow* data) {
    
}
void TopWindow::show_pops_cb(UI::Widget& w, TopWindow* data) {
    
}
void TopWindow::show_reforms_cb(UI::Widget& w, TopWindow* data) {
    
}