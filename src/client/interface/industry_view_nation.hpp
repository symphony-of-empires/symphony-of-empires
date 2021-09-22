#ifndef INDUSTRY_VIEW_NATION_H
#define INDUSTRY_VIEW_NATION_H

#include "../game_state.hpp"
#include "../ui.hpp"
#include "../../nation.hpp"
#include "../../world.hpp"

#include <memory>

class GameState;

class IndustryViewNationChart : public UI::Window {
   public:
    IndustryViewNationChart(GameState& gs, Industry* industry);
    GameState& gs;
    Building* industry;
};

class IndustryViewNationButton : public UI::Button {
   public:
    IndustryViewNationButton(GameState& gs, UI::Window* parent, size_t index);
    UI::Window* parent;
    GameState& gs;
    Building* industry;
    std::string text;
};

class IndustryViewNation : public UI::Window {
   public:
    IndustryViewNation(GameState& gs);
    const size_t buttons_nr = 16;
    std::vector<IndustryViewNationButton*> buttons;
    GameState& gs;

   private:

    size_t page_nr = 0;
};

#endif