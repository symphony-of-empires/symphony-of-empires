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
#include "client/interface/factory_window.hpp"
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
    auto* flag_img = new UI::Image(5, 4, 138, 88, this->gs.get_nation_flag(*this->gs.curr_nation), this);
#ifndef E3D_HANDHELD
    new UI::Image(5, 4, flag_img->width, flag_img->height, "gfx/drop_shadow.png", this);
#endif

    auto* stats_grp = new UI::Image(150, 0, 356, 184, "gfx/background2.png", true, this);
    stats_grp->flex = UI::Flex::ROW;
    stats_grp->overflow = UI::Overflow::WRAP;

    auto* money_grp = new UI::Div(0, 0, 24 + 64, 24, stats_grp);
    auto* money_img = new UI::Image(0, 0, 24, 24, "gfx/economy.png", true, money_grp);
    money_img->set_tooltip("Money");
    auto* money_lab = new UI::Label(0, 0, " ", money_grp);
    money_lab->set_on_each_tick([this](UI::Widget& w) {
        w.text(Eng3D::translate_format("%8.0f", this->gs.curr_nation->budget));
    });
    money_lab->on_each_tick(*money_lab);

    auto* military_score_grp = new UI::Div(0, 0, 24 + 64, 24, stats_grp);
    auto* military_score_img = new UI::Image(0, 0, 24, 24, "gfx/military_score.png", true, military_score_grp);
    military_score_img->set_tooltip("Military score");
    auto* military_score_lab = new UI::Label(0, 0, " ", military_score_grp);
    military_score_lab->set_on_each_tick([this](UI::Widget& w) {
        auto total = 0.f;
        for(const auto& province : this->gs.world->provinces) {
            const auto& units = this->gs.world->unit_manager.get_province_units(province.get_id());
            for(const auto unit_id : units) {
                const auto& unit = this->gs.world->unit_manager.units[unit_id];
                if(unit.owner_id == this->gs.curr_nation->get_id())
                    total += unit.get_strength();
            }
        }
        w.text(Eng3D::string_format("%8.0f", total));
    });
    military_score_lab->on_each_tick(*military_score_lab);

    auto* industrial_score_grp = new UI::Div(0, 0, 24 + 64, 24, stats_grp);
    auto* industrial_score_img = new UI::Image(0, 0, 24, 24, "gfx/factory.png", true, industrial_score_grp);
    industrial_score_img->set_tooltip("Industrial score");
    auto* industrial_score_lab = new UI::Label(0, 0, " ", industrial_score_grp);
    industrial_score_lab->set_on_each_tick([this](UI::Widget& w) {
        auto total = 0.f;
        for(const auto province_id : this->gs.curr_nation->owned_provinces) {
            const auto& province = this->gs.world->provinces[province_id];
            for(const auto& building : province.get_buildings()) {
                total += building.production_scale * building.level;
            }
        }
        w.text(Eng3D::string_format("%8.0f", total));
    });
    industrial_score_lab->on_each_tick(*industrial_score_lab);

    auto* prestige_score_grp = new UI::Div(0, 0, 24 + 64, 24, stats_grp);
    auto* prestige_score_img = new UI::Image(0, 0, 24, 24, "gfx/prestige.png", true, prestige_score_grp);
    prestige_score_img->set_tooltip("Prestige");
    auto* prestige_score_lab = new UI::Label(0, 0, " ", prestige_score_grp);
    prestige_score_lab->set_on_each_tick([this](UI::Widget& w) {
        w.text(Eng3D::string_format("%8.0f", this->gs.curr_nation->prestige));
    });
    prestige_score_lab->on_each_tick(*prestige_score_lab);

    for(const auto& good : this->gs.world->goods) {
        auto* good_grp = new UI::Div(0, 0, 24 + 64, 24, stats_grp);
        auto* good_img = new UI::Image(0, 0, 24, 24, good.get_icon_path(), true, good_grp);
        good_img->set_tooltip(good.name);
        auto* good_lab = new UI::Label(0, 0, " ", good_grp);
        good_lab->set_on_each_tick([this, &good](UI::Widget& w) {
            auto total = 0.f;
            for(const auto province_id : this->gs.curr_nation->owned_provinces) {
                const auto& province = this->gs.world->provinces[province_id];
                total += province.products[good].supply;
            }
            w.text(Eng3D::string_format("%8.0f", total));
        });
        good_lab->on_each_tick(*good_lab);
    }

    auto* flex_column = new UI::Div(3, 96, 42, 390, this);
    flex_column->flex = UI::Flex::COLUMN;
    flex_column->flex_justify = UI::FlexJustify::SPACE_AROUND;
    flex_column->flex_align = UI::Align::CENTER;

    int icon_size = 28;

    auto* policy_ibtn = new UI::Image(0, 0, icon_size, icon_size, "gfx/book.png", true, flex_column);
    policy_ibtn->set_on_click([this](UI::Widget&) {

    });
    policy_ibtn->set_tooltip("Laws & Policies");

    auto* economy_ibtn = new UI::Image(0, 0, icon_size, icon_size, "gfx/economy.png", true, flex_column);
    economy_ibtn->set_on_click([this](UI::Widget&) {

    });
    economy_ibtn->set_tooltip("Economy");

    auto* pops_ibtn = new UI::Image(0, 0, icon_size, icon_size, "gfx/pop.png", true, flex_column);
    pops_ibtn->set_on_click([this](UI::Widget&) {
        new Interface::PopWindow(this->gs);
    });
    pops_ibtn->set_tooltip("Population");

    auto* factory_ibtn = new UI::Image(0, 0, icon_size, icon_size, "gfx/factory.png", true, flex_column);
    factory_ibtn->set_on_click([this](UI::Widget&) {
        new Interface::FactoryWindow(this->gs);
    });
    factory_ibtn->set_tooltip("Factories");

    auto* military_ibtn = new UI::Image(0, 0, icon_size, icon_size, "gfx/military_score.png", true, flex_column);
    military_ibtn->set_on_click([this](UI::Widget&) {
        new Interface::ArmyView(this->gs);
    });
    military_ibtn->set_tooltip("Military");

    auto* research_ibtn = new UI::Image(0, 0, icon_size, icon_size, "gfx/tech.png", true, flex_column);
    research_ibtn->set_on_click([this](UI::Widget&) {
        
    });
    research_ibtn->set_tooltip("Research");

    auto* save_ibtn = new UI::Image(0, 0, icon_size, icon_size, "gfx/save.png", true, flex_column);
    save_ibtn->set_on_click([this](UI::Widget&) {
        LUA_util::save(this->gs);
    });
    /// @todo Save the lua state
    save_ibtn->set_tooltip("Saves the current game");

    auto* load_ibtn = new UI::Image(9, 275, 25, 25, "gfx/top_bar/save.png", true, flex_column);
    load_ibtn->set_on_click([this](UI::Widget&) {
        const auto nation_id = this->gs.curr_nation->get_id();
        this->gs.paused = true;
        Archive ar{};
        ar.from_file("default.sc4");
        std::string creat_date;
        ::deserialize(ar, creat_date);
        if(creat_date != __DATE__)
            this->gs.ui_ctx.prompt(translate("Savefile error"), translate("Savefile is from an incompatible version"));
        ::deserialize(ar, *this->gs.world);
        /// @todo Events aren't properly saved yet
        this->gs.world->events.clear();
        this->gs.world->taken_decisions.clear();
        for(auto& nation : this->gs.world->nations)
            nation.inbox.clear();
        this->gs.world->load_mod();
        this->gs.curr_nation = &this->gs.world->nations[nation_id];
        this->gs.ui_ctx.prompt("Loaded", "Loaded savefile");
    });
    load_ibtn->set_tooltip("Load this savefile");

    auto* exit_ibtn = new UI::Image(0, 0, icon_size, icon_size, "gfx/exit.png", true, flex_column);
    exit_ibtn->set_on_click([this](UI::Widget&) {
        this->gs.ui_ctx.clear();
        this->gs.ui_ctx.use_tooltip(nullptr, { 0, 0 });
        this->gs.paused = true;
        this->gs.client.reset();
        this->gs.server.reset();
        new Interface::MainMenu(this->gs);
    });
    exit_ibtn->set_tooltip("Back to the main menu");
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
        auto btn_group = new UI::Group(320, 12, this);
        auto* speed0_btn = UI::Image::make_transparent(0, 0, btn_size, btn_size, "gfx/ui/button/time_control_pause.png", true, btn_group);
        speed0_btn->set_on_click([this](UI::Widget&) {
            this->gs.paused = true;
        });
        speed0_btn->set_tooltip(translate("Pause"));
        auto* speed1_btn = new UI::Image(45, 0, btn_size, btn_size, "gfx/ui/button/time_control_1.png", true, btn_group);
        speed1_btn->set_on_click([this](UI::Widget&) {
            this->gs.paused = false;
            this->gs.ms_delay_speed = 1000;
        });
        speed1_btn->set_tooltip(translate("Turtle speed"));
        auto* speed2_btn = new UI::Image(80, 0, btn_size, btn_size, "gfx/ui/button/time_control_2.png", true, btn_group);
        speed2_btn->set_on_click([this](UI::Widget&) {
            this->gs.paused = false;
            this->gs.ms_delay_speed = 500;
        });
        speed2_btn->set_tooltip(translate("Horse speed"));
        auto* speed3_btn = new UI::Image(115, 0, btn_size, btn_size, "gfx/ui/button/time_control_3.png", true, btn_group);
        speed3_btn->set_on_click([this](UI::Widget&) {
            this->gs.paused = false;
            this->gs.ms_delay_speed = 100;
        });
        speed3_btn->set_tooltip(translate("Fire speed"));
    }

    auto font = gs.ttf_man.load(gs.package_man.get_unique("fonts/neon_euler/euler.ttf"));
    
    auto* time_lab = new UI::Label(50, 30, " ", this);
    time_lab->font = font;
    time_lab->text_color = Eng3D::Color(1., 1., 1.);
    time_lab->set_on_each_tick([this](UI::Widget& w) {
        std::tm tm{};
        tm.tm_year = this->gs.world->get_year() - 1900;
        tm.tm_mon = this->gs.world->get_month() - 1;
        tm.tm_mday = this->gs.world->get_day() + 1;
        char mbstr[100];
        std::strftime(mbstr, sizeof mbstr, "%x", &tm);
        w.text(mbstr);
    });
    time_lab->on_each_tick(*time_lab);
}