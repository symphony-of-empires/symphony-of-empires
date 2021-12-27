#pragma once

#include <string>
#include <vector>
#include <cstdlib>

class GameState;
namespace UI {
    class Label;
    class Button;
}
namespace Interface {
    /*class LobbyPlayerView : public UI::Window {
        GameState& gs;
    public:
        LobbyPlayerView(GameState& gs) {};
        ~LobbyPlayerView() {};
    };*/

    struct LoadGameBtnData {
        GameState& gs;
        std::string file;
    };

    class LobbySelectView {
        GameState& gs;
        UI::Button* curr_country_btn;
        UI::Label* select_country_lab;
        UI::Button* next_country_btn;
        UI::Button* prev_country_btn;

        std::vector<struct LoadGameBtnData> ldgame_data;
    public:
        LobbySelectView(GameState& gs);
        ~LobbySelectView() {};

        void change_nation(size_t id);
        size_t curr_selected_nation;
    };
};