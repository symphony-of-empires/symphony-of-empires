#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui.hpp"

namespace Interface {
    class DescisionButton: public UI::Button {
    public:
        DescisionButton(UI::Window* parent, GameState& gs, const Descision& descision, Event& msg);

        GameState& gs;
        const Descision& descision;
        Event& event;
    };

    class DescisionWindow: public UI::Window {
    public:
        DescisionWindow(GameState& gs, Event& msg);
    };
};