// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      client/interface/top_window.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/texture.hpp"
#include "eng3d/ui/components.hpp"
#include "eng3d/ttf.hpp"
#include "eng3d/string.hpp"

#include "client/interface/top_window.hpp"
#include "client/interface/main_menu.hpp"
#include "client/game_state.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
#include "client/map.hpp"
#include "world.hpp"
#include "client/interface/army.hpp"
#include "client/interface/pop_window.hpp"
#include "client/interface/industry_window.hpp"
#include "client/interface/policies.hpp"
#include "client/lua_save_util.hpp"

using namespace Interface;

TopWindow::TopWindow(GameState& _gs)
    : UI::Group(0, 0),
    gs{ _gs }
{
    this->is_scroll = false;
    this->is_pinned = true;

    this->gs.time_win = static_cast<UI::Widget*>(new TimeControlView(gs));

    UI::Image::make_transparent(0, 0, 147, 499, "gfx/top_window.png", this);
    auto& flag_img = this->make_widget<UI::Image>(5, 4, 138, 88, this->gs.tex_man.get_white());
    flag_img.set_on_each_tick([this](UI::Widget& w) {
        w.current_texture = this->gs.get_nation_flag(*this->gs.curr_nation);
    });
    flag_img.on_each_tick(flag_img);
#ifndef E3D_HANDHELD
    this->make_widget<UI::Image>(5, 4, flag_img.width, flag_img.height, "gfx/drop_shadow.png");
#endif

    auto& event_tray_grp = this->make_widget<UI::Image>(150, 32, 356 + 256, 32, "gfx/background2.png", true);
    event_tray_grp.flex = UI::Flex::ROW;
    this->gs.event_tray_grp = &event_tray_grp;

    auto& stats_grp = this->make_widget<UI::Image>(150, 0, 356 + 256, 32, "gfx/background2.png", true);
    stats_grp.flex = UI::Flex::ROW;

    auto& money_grp = stats_grp.make_widget<UI::Div>(0, 0, 24 + 96, 24);
    money_grp.flex = UI::Flex::ROW;
    auto& money_img = money_grp.make_widget<UI::Image>(0, 0, 24, 24, "gfx/economy.png", true);
    money_img.set_tooltip("Money");
    auto& money_lab = money_grp.make_widget< UI::Label>(0, 0, " ");
    money_lab.set_on_each_tick([this](UI::Widget& w) {
        w.set_text(Eng3D::Locale::format_number(this->gs.curr_nation->budget));
    });
    money_lab.on_each_tick(money_lab);

    auto& population_grp = stats_grp.make_widget<UI::Div>(0, 0, 24 + 96, 24);
    population_grp.flex = UI::Flex::ROW;
    auto& population_img = population_grp.make_widget< UI::Image>(0, 0, 24, 24, "gfx/pop.png", true);
    population_img.set_tooltip("Population");
    auto& population_lab = population_grp.make_widget< UI::Label>(0, 0, " ");
    population_lab.set_on_each_tick([this](UI::Widget& w) {
        const auto total = std::accumulate(gs.curr_nation->owned_provinces.cbegin(), gs.curr_nation->owned_provinces.cend(), 0.f, [this](auto&& a, const auto province_id) -> float {
            return a + gs.world->provinces[province_id].total_pops();
        });
        w.set_text(Eng3D::Locale::format_number(total));

        const auto [it1, it2] = std::minmax_element(gs.curr_nation->owned_provinces.cbegin(), gs.curr_nation->owned_provinces.cend(), [this](const auto province_id, const auto other_province_id) {
            return gs.world->provinces[province_id].total_pops() < gs.world->provinces[other_province_id].total_pops();
        });
        const auto& smallest_province = gs.world->provinces[*it1];
        const auto& largest_province = gs.world->provinces[*it2];

        w.set_tooltip(Eng3D::string_format("Average population density: %.0f\nLargest province: %s (%.0f)\nSmallest province: %s (%.0f)", total / gs.curr_nation->owned_provinces.size(), largest_province.name.data(), largest_province.total_pops(), smallest_province.name.data(), smallest_province.total_pops()));
    });
    population_lab.on_each_tick(population_lab);

    auto& military_score_grp = stats_grp.make_widget<UI::Div>(0, 0, 24 + 64, 24);
    military_score_grp.flex = UI::Flex::ROW;
    auto& military_score_img = military_score_grp.make_widget<UI::Image>(0, 0, 24, 24, "gfx/military_score.png", true);
    military_score_img.set_tooltip("Military score");
    auto& military_score_lab = military_score_grp.make_widget<UI::Label>(0, 0, " ");
    military_score_lab.set_on_each_tick([this](UI::Widget& w) {
        const auto total = std::accumulate(gs.curr_nation->owned_provinces.cbegin(), gs.curr_nation->owned_provinces.cend(), 0.f, [this](const auto a, const auto province_id) -> float {
            const auto& province = this->gs.world->provinces[province_id];
            const auto& units = this->gs.world->unit_manager.get_province_units(province.get_id());
            return a + std::accumulate(units.cbegin(), units.cend(), 0, [this](const auto a2, const auto unit_id) {
                const auto& unit = this->gs.world->unit_manager.units[unit_id];
                return a2 + (unit.owner_id == this->gs.curr_nation->get_id() ? unit.get_strength() : 0.f);
            });
        });
        w.set_text(Eng3D::string_format("%.0f", total));
    });
    military_score_lab.on_each_tick(military_score_lab);

    auto& industrial_score_grp = stats_grp.make_widget<UI::Div>(0, 0, 24 + 64, 24);
    industrial_score_grp.flex = UI::Flex::ROW;
    auto& industrial_score_img = industrial_score_grp.make_widget<UI::Image>(0, 0, 24, 24, "gfx/industry.png", true);
    industrial_score_img.set_tooltip("Industrial score");
    auto& industrial_score_lab = industrial_score_grp.make_widget<UI::Label>(0, 0, " ");
    industrial_score_lab.set_on_each_tick([this](UI::Widget& w) {
        const auto total = std::accumulate(gs.curr_nation->owned_provinces.cbegin(), gs.curr_nation->owned_provinces.cend(), 0.f, [this](const auto a, const auto province_id) -> float {
            const auto& province = this->gs.world->provinces[province_id];
            const auto& buildings = province.get_buildings();
            return a + std::accumulate(buildings.cbegin(), buildings.cend(), 0.f, [](const auto a2, const auto& building) {
                return a2 + (building.production_scale * building.level);
            });
        });
        w.set_text(Eng3D::string_format("%.0f", total));
    });
    industrial_score_lab.on_each_tick(industrial_score_lab);

    auto& prestige_score_grp = stats_grp.make_widget<UI::Div>(0, 0, 24 + 64, 24);
    prestige_score_grp.flex = UI::Flex::ROW;
    auto& prestige_score_img = prestige_score_grp.make_widget<UI::Image>(0, 0, 24, 24, "gfx/prestige.png", true);
    prestige_score_img.set_tooltip("Prestige");
    auto& prestige_score_lab = prestige_score_grp.make_widget<UI::Label>(0, 0, " ");
    prestige_score_lab.set_on_each_tick([this](UI::Widget& w) {
        w.set_text(Eng3D::string_format("%.0f", this->gs.curr_nation->prestige));
    });
    prestige_score_lab.on_each_tick(prestige_score_lab);

    auto& flex_column = this->make_widget<UI::Div>(3, 96, 42, 390);
    flex_column.flex = UI::Flex::COLUMN;
    flex_column.flex_justify = UI::FlexJustify::SPACE_AROUND;
    flex_column.flex_align = UI::Align::CENTER;

    int icon_size = 28;

    auto& policy_ibtn = flex_column.make_widget<UI::Image>(0, 0, icon_size, icon_size, "gfx/book.png", true);
    policy_ibtn.set_on_click([this](UI::Widget&) {
        new Interface::PoliciesView(this->gs);
    });
    policy_ibtn.set_tooltip("Laws & Policies");

    auto& economy_ibtn = flex_column.make_widget<UI::Image>(0, 0, icon_size, icon_size, "gfx/economy.png", true);
    economy_ibtn.set_on_click([this](UI::Widget&) {

    });
    economy_ibtn.set_tooltip("Economy");

    auto& pops_ibtn = flex_column.make_widget<UI::Image>(0, 0, icon_size, icon_size, "gfx/pop.png", true);
    pops_ibtn.set_on_click([this](UI::Widget&) {
        new Interface::PopWindow(this->gs);
    });
    pops_ibtn.set_tooltip("Population");

    auto& industry_ibtn = flex_column.make_widget<UI::Image>(0, 0, icon_size, icon_size, "gfx/industry.png", true);
    industry_ibtn.set_on_click([this](UI::Widget&) {
        new Interface::IndustryWindow(this->gs);
    });
    industry_ibtn.set_tooltip("Factories");

    auto& military_ibtn = flex_column.make_widget<UI::Image>(0, 0, icon_size, icon_size, "gfx/military_score.png", true);
    military_ibtn.set_on_click([this](UI::Widget&) {
        new Interface::ArmyView(this->gs);
    });
    military_ibtn.set_tooltip("Military");

    auto& research_ibtn = flex_column.make_widget<UI::Image>(0, 0, icon_size, icon_size, "gfx/tech.png", true);
    research_ibtn.set_on_click([this](UI::Widget&) {

    });
    research_ibtn.set_tooltip("Research");

    auto& save_ibtn = flex_column.make_widget<UI::Image>(0, 0, icon_size, icon_size, "gfx/save.png", true);
    save_ibtn.set_on_click([this](UI::Widget&) {
        LUA_util::save(this->gs, string_format("%s_%zu-%zu-%zu.sc4", gs.curr_nation->ref_name.data(), gs.world->get_year(), gs.world->get_month(), gs.world->get_day()));
    });
    save_ibtn.set_tooltip("Saves the current game");

    auto& load_ibtn = flex_column.make_widget<UI::Image>(9, 275, 25, 25, "gfx/top_bar/save.png", true);
    load_ibtn.set_on_click([this](UI::Widget&) {
        LUA_util::load(this->gs, "autosave.sc4");
    });
    load_ibtn.set_tooltip("Load latest autosave");

    auto& exit_ibtn = flex_column.make_widget<UI::Image>(0, 0, icon_size, icon_size, "gfx/exit.png", true);
    exit_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.ui_ctx.clear();
        this->gs.ui_ctx.use_tooltip(nullptr, { 0, 0 });
        this->gs.paused = true;
        this->gs.client.reset();
        this->gs.server.reset();
        new Interface::MainMenu(this->gs);
    });
    exit_ibtn.set_tooltip("Back to the main menu");
}

TimeControlView::TimeControlView(GameState& _gs)
    : UI::Group(-480, 0),
    gs{ _gs }
{
    this->is_scroll = false;
    this->is_pinned = true;
    this->origin = UI::Origin::UPPER_RIGHT_SCREEN;

    UI::Image::make_transparent(0, 0, 480, 60, "gfx/ui/bg/time_control_bg.png", true, this);

    int btn_size = 30;
    if(!gs.editor) {
        auto& btn_group = this->make_widget<UI::Group>(320, 12);
        auto& speed0_btn = btn_group.make_widget<UI::Image>(0, 0, btn_size, btn_size, "gfx/ui/button/time_control_pause.png", true);
        speed0_btn.set_on_click([this](UI::Widget&) {
            this->gs.paused = true;
        });
        speed0_btn.set_tooltip(translate("Pause"));
        auto& speed1_btn = btn_group.make_widget<UI::Image>(45, 0, btn_size, btn_size, "gfx/ui/button/time_control_1.png", true);
        speed1_btn.set_on_click([this](UI::Widget&) {
            this->gs.paused = false;
            this->gs.ms_delay_speed = 1000;
        });
        speed1_btn.set_tooltip(translate("Turtle speed"));
        auto& speed2_btn = btn_group.make_widget<UI::Image>(80, 0, btn_size, btn_size, "gfx/ui/button/time_control_2.png", true);
        speed2_btn.set_on_click([this](UI::Widget&) {
            this->gs.paused = false;
            this->gs.ms_delay_speed = 500;
        });
        speed2_btn.set_tooltip(translate("Horse speed"));
        auto& speed3_btn = btn_group.make_widget<UI::Image>(115, 0, btn_size, btn_size, "gfx/ui/button/time_control_3.png", true);
        speed3_btn.set_on_click([this](UI::Widget&) {
            this->gs.paused = false;
            this->gs.ms_delay_speed = 100;
        });
        speed3_btn.set_tooltip(translate("Fire speed"));
    }

    auto& time_lab = this->make_widget<UI::Label>(50, 30, " ");
    time_lab.font = gs.ttf_man.load(gs.package_man.get_unique("fonts/neon_euler/euler.ttf"));
    time_lab.text_color = Eng3D::Color(1., 1., 1.);
    time_lab.set_on_each_tick([this](UI::Widget& w) {
        std::tm tm{};
        tm.tm_year = this->gs.world->get_year() - 1900;
        tm.tm_mon = this->gs.world->get_month();
        tm.tm_mday = this->gs.world->get_day() + 1;
        char mbstr[100];
        std::strftime(mbstr, sizeof(mbstr), "%x", &tm);
        w.set_text(mbstr);
    });
    time_lab.on_each_tick(time_lab);
}
