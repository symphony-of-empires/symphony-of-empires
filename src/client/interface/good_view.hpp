#pragma once

#include "../../good.hpp"
#include "../../world.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"

namespace Interface {
    class GoodView : public UI::Window {
        GameState& gs;
        Good* good;

        UI::Image* icon_img;
        UI::Label* name_lab;
        UI::PieChart *sellers_pie;

        UI::CloseButton* close_btn;
    public:
        GoodView(GameState& gs, Good* good);
    };
};