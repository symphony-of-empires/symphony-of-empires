#pragma once

#include "good.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui.hpp"

namespace Interface {
    class GoodView : public UI::Window {
        GameState& gs;
        Good* good;

        UI::Image* icon_img;
        UI::Label* name_lab;
        UI::PieChart *sellers_pie;

        UI::Group* producing_it_group;
        UI::Group* consumer_it_group;

        UI::CloseButton* close_btn;
    public:
        GoodView(GameState& gs, Good* good);
    };
};