#ifndef INDUSTRY_VIEW_NATION_H
#define INDUSTRY_VIEW_NATION_H

#include "../game_state.hpp"
#include "../ui.hpp"
#include "industry.hpp"
#include "nation.hpp"
#include "world.hpp"

class GameState;
class IndustryViewNationButton {
   public:
    IndustryViewNationButton(GameState& gs, UI::Window* parent, size_t index);
    GameState& gs;
    Industry* industry;
    std::string text;
};

class IndustryViewNation {
   public:
    IndustryViewNation(GameState& gs);
    void show();
    void hide();
    UI::Window* industry_view_nation_win;
    std::vector<IndustryViewNationButton> buttons;
    GameState& gs;

   private:
    uint8_t page_nr;
};

#endif