#pragma once

#include "../ui.hpp"

class GameState;
namespace Interface {
    class SelectNation {
    public:
        SelectNation(GameState& gs);
        ~SelectNation(){};

        void change_nation(size_t id);
        size_t curr_selected_nation;

    private:
        GameState& gs;
        UI::Button* curr_country_btn;
        UI::Label* select_country_lab;
        UI::Button* next_country_btn;
        UI::Button* prev_country_btn;
    };
};