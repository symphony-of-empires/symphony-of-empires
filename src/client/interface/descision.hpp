#ifndef DESCISION_H
#define DESCISION_H
#include "../../nation.hpp"
#include "../../world.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"

class DescisionButton : public UI::Button {
   public:
    DescisionButton(UI::Window* parent, GameState& gs, const Descision& descision, Event& msg);

    GameState& gs;
    const Descision& descision;
    Event& event;
};

class DescisionWindow : public UI::Window {
   public:
    DescisionWindow(GameState& gs, Event& msg);
};

#endif