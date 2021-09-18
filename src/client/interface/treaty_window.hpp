#ifndef TREATY_WINDOW_H
#define TREADY_WINDOW_H

#include "../../diplomacy.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"

std::string treaty_to_text(const Treaty& treaty);

// TODO: Make it close it's children when closing

class TreatyWindow;
class TreatySelectNation : public UI::Window {
   public:
    TreatySelectNation(TreatyWindow* treaty_window);

    TreatyWindow* treaty_win;
};

class TreatyPerClauseWindow : public UI::Window {
   public:
    TreatyPerClauseWindow(TreatyWindow* treaty_window);

    TreatyWindow* treaty_win;
};

class TreatyClauseWindow : public UI::Window {
   public:
    TreatyClauseWindow(TreatyWindow* treaty_window);

    TreatyWindow* treaty_win;
};

class TreatyWindow : public UI::Window {
   public:
    TreatyWindow(GameState& gs, UI::Window* top_win);

    UI::Label* recv_nation_lab;
    UI::Image* recv_nation_flag;
    TreatySelectNation* select_win;
    Nation* recv_nation = nullptr;
    Treaty treaty_draft;
    GameState& gs;
};

class TreatyPopup : public UI::Window {
   public:
    TreatyPopup(GameState& gs, Treaty* treaty);
    Treaty* treaty;
    GameState& gs;
};
#endif