#ifndef INTERFACE_DESCISION_HPP
#define INTERFACE_DESCISION_HPP

#include "../../nation.hpp"
#include "../../world.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"

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

#endif