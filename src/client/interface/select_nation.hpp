#ifndef SELECT_NATION_H
#define SELECT_NATION_H

#include "../game_state.hpp"
#include "../ui.hpp"

class GameState;
class SelectNation {
public:
	SelectNation(GameState& gs);
	~SelectNation() {};

	void change_nation(size_t id);
    static void next_nation_cb(UI::Widget& w, SelectNation* data);
    static void prev_nation_cb(UI::Widget& w, SelectNation* data);
	size_t curr_selected_nation = 0;		
private:
	GameState& gs;
	UI::Button* curr_country_btn;
};

#endif