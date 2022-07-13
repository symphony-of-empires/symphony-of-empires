// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      client/interface/top_window.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/ui/components.hpp"
#include "eng3d/locale.hpp"

#include "client/interface/top_window.hpp"
#include "client/game_state.hpp"
#include "client/map.hpp"
#include "world.hpp"
#include "client/interface/policies.hpp"
#include "client/interface/army.hpp"
#include "client/interface/technology.hpp"
#include "client/interface/pop_window.hpp"
#include "client/interface/factory_window.hpp"
#include "io_impl.hpp"

using namespace Interface;

TopWindow::TopWindow(GameState& _gs)
    : UI::Group(0, 0),
    gs{ _gs }
{
    this->is_scroll = false;
    this->is_pinned = true;

    new TimeControlView(gs);

    UI::Image::make_transparent(0, 0, 147, 499, "gfx/top_window.png", this);
    auto nation_flag = this->gs.get_nation_flag(*gs.curr_nation);
    auto* flag_img = new UI::AspectImage(5, 4, 138, 88, nation_flag, this);
    flag_img->on_each_tick = ([this](UI::Widget& w) {
        w.current_texture = this->gs.get_nation_flag(*this->gs.curr_nation);
    });
    new UI::Image(0, 0, flag_img->width, flag_img->height, gs.tex_man.load(this->gs.package_man.get_unique("gfx/flag_rug.png")), this);

    auto* flex_column = new UI::Div(3, 96, 42, 390, this);
    flex_column->flex = UI::Flex::COLUMN;
    flex_column->flex_justify = UI::FlexJustify::SPACE_AROUND;
    flex_column->flex_align = UI::Align::CENTER;

    int icon_size = 28;

    auto* policy_ibtn = new UI::Image(0, 0, icon_size, icon_size, "gfx/book.png", true, flex_column);
    policy_ibtn->set_on_click([this](UI::Widget&) {
        new Interface::PoliciesScreen(this->gs);
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
        new Interface::TechTreeView(this->gs);
    });
    research_ibtn->set_tooltip("Research");

    auto* save_ibtn = new UI::Image(0, 0, icon_size, icon_size, "gfx/save.png", true, flex_column);
    save_ibtn->set_on_click([this](UI::Widget&) {
        save(this->gs);
    });
    /// @todo Save the lua state
    save_ibtn->set_tooltip("Saves the current game");

    auto* load_ibtn = new UI::Image(9, 275, 25, 25, "gfx/top_bar/save.png", true, flex_column);
    load_ibtn->set_on_click([this](UI::Widget&) {
        const auto nation_id = this->gs.curr_nation->get_id();
        Archive ar = Archive();
        ar.from_file("default.sc4");
        ::deserialize(ar, this->gs.world);
        this->gs.world->load_mod();

        this->gs.curr_nation = &this->gs.world->nations[nation_id];
        this->gs.ui_ctx.prompt("Loaded", "Loaded savefile");
    });
    load_ibtn->set_tooltip("Load this savefile");

    auto* exit_ibtn = new UI::Image(0, 0, icon_size, icon_size, "gfx/exit.png", true, flex_column);
    exit_ibtn->set_on_click([this](UI::Widget&) {
        this->gs.run = false;
    });
    exit_ibtn->set_tooltip("Exits");
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
        speed0_btn->set_tooltip("Pause");

        auto* speed1_btn = new UI::Image(45, 0, btn_size, btn_size, "gfx/ui/button/time_control_1.png", true, btn_group);
        speed1_btn->set_on_click([this](UI::Widget&) {
            this->gs.paused = false;
            this->gs.ms_delay_speed = 1000;
        });
        speed1_btn->set_tooltip("Turtle speed");

        auto* speed2_btn = new UI::Image(80, 0, btn_size, btn_size, "gfx/ui/button/time_control_2.png", true, btn_group);
        speed2_btn->set_on_click([this](UI::Widget&) {
            this->gs.paused = false;
            this->gs.ms_delay_speed = 500;
        });
        speed2_btn->set_tooltip("Horse speed");

        auto* speed3_btn = new UI::Image(115, 0, btn_size, btn_size, "gfx/ui/button/time_control_3.png", true, btn_group);
        speed3_btn->set_on_click([this](UI::Widget&) {
            this->gs.paused = false;
            this->gs.ms_delay_speed = 100;
        });
        speed3_btn->set_tooltip("Fire speed");
    }

    auto font = TTF_OpenFont(gs.package_man.get_unique("fonts/neon_euler/euler.ttf")->get_abs_path().c_str(), 20);
    auto text_color = Eng3D::Color(1., 1., 1.);

    auto* time_lab = new UI::Label(50, 30, " ", this);
    time_lab->on_each_tick = ([this](UI::Widget& w) {
        const std::string day_names[7] = {
            "Monday",
            "Tuesday",
            "Wednesday",
            "Thursday",
            "Friday",
            "Saturday",
            "Sunday"
        };

        const std::string month_names[12] = {
            "January",
            "Febraury",
            "March",
            "April",
            "May",
            "June",
            "July",
            "August",
            "October",
            "September",
            "November",
            "December"
        };

        const World* world = this->gs.world;
        const int day = world->time % world->ticks_per_month;
        const int month = (world->time / world->ticks_per_month) % 12;
        const int year = world->time / world->ticks_per_month / 12;
        w.text(Eng3D::Locale::translate(day_names[day % 7]) + ", " + Eng3D::Locale::translate(month_names[month]) + " " + std::to_string(day + 1) + ", " + std::to_string(year));
    });
    time_lab->font = font;
    time_lab->text_color = text_color;
    time_lab->on_each_tick(*time_lab);
}