#ifndef POP_VIEW_NATION_H
#define POP_VIEW_NATION_H

#include "../game_state.hpp"
#include "../ui.hpp"

class GameState;
// View the provinces in a country - along with the population in them
class PopViewNation {
   public:
    PopViewNation(GameState& gs);

    UI::Window* pop_view_nation_win;
    void show();
    GameState& gs;

    uint8_t page_nr;
    static void on_update(UI::Widget& w, PopViewNation* data);
   private:
};

#endif