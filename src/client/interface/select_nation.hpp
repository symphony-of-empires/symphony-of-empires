#ifndef SELECT_NATION_H
#define SELECT_NATION_H

#include "../game_state.hpp"
#include "../ui.hpp"

class GameState;
class SelectNation {
   public:
    SelectNation(GameState& gs);
    ~SelectNation(){};

    void change_nation(size_t id);
    static void next_nation_cb(UI::Widget& w, SelectNation* data);
    static void prev_nation_cb(UI::Widget& w, SelectNation* data);
    static void play_nation_cb(UI::Widget& w, SelectNation* data);
    size_t curr_selected_nation;

   private:
    GameState& gs;
    UI::Button* curr_country_btn;
    UI::Button* select_country_btn;
    UI::Button* next_country_btn;
    UI::Button* prev_country_btn;
    UI::Button* play_btn;
};

#endif