#ifndef INDUSTRY_VIEW_NATION_H
#define INDUSTRY_VIEW_NATION_H

#include "../game_state.hpp"
#include "../ui.hpp"
#include "industry.hpp"
#include "nation.hpp"
#include "../../world.hpp"

#include <memory>

class GameState;

class IndustryViewNationChart {
   public:
    IndustryViewNationChart(GameState& gs, Industry* industry);
    ~IndustryViewNationChart() {};
    GameState& gs;
    Industry* industry;
};

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
    std::vector<std::unique_ptr<IndustryViewNationButton>> buttons;
    GameState& gs;

   private:

    size_t page_nr = 0;
};

#endif