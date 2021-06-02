#include "ui.hpp"
#include "world.hpp"
extern World * g_world;
extern int width;
extern int height;

extern size_t player_nation_id;

void do_policy_overview(UI::Widget *, void *) {
	UI::Label * lab;
	UI::Button * btn;
	
	UI::Window * policy_win;
	
	policy_win = new UI::Window(nullptr, 128, 128, 512 + 128, height - 128, "Diplomacy");
	lab = new UI::Label(policy_win, 0, 0, "Migration");
	btn = new UI::Button(policy_win, 130 * 0, 24, 128, 24, "nobody");
	btn = new UI::Button(policy_win, 130 * 1, 24, 128, 24, "citizen only");
	btn = new UI::Button(policy_win, 130 * 2, 24, 128, 24, "all pay");
	btn = new UI::Button(policy_win, 130 * 3, 24, 128, 24, "all");
	
	lab = new UI::Label(policy_win, 0, 24 * 2, "Immigration");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 3, 128, 24, "nobody");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 3, 128, 24, "citizen only");
	btn = new UI::Button(policy_win, 130 * 2, 24 * 3, 128, 24, "all pay");
	btn = new UI::Button(policy_win, 130 * 3, 24 * 3, 128, 24, "all");
	
	lab = new UI::Label(policy_win, 0, 24 * 4, "Censorsip");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 5, 128, 24, "all");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 5, 128, 24, "state");
	btn = new UI::Button(policy_win, 130 * 2, 24 * 5, 128, 24, "approved");
	btn = new UI::Button(policy_win, 130 * 3, 24 * 5, 128, 24, "none");
	
	lab = new UI::Label(policy_win, 0, 24 * 6, "POPs build infra");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 7, 128, 24, "none");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 7, 128, 24, "approved");
	btn = new UI::Button(policy_win, 130 * 2, 24 * 7, 128, 24, "all");
	
	lab = new UI::Label(policy_win, 0, 24 * 8, "POPs build factories");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 9, 128, 24, "none");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 9, 128, 24, "approved");
	btn = new UI::Button(policy_win, 130 * 2, 24 * 9, 128, 24, "all");
	
	lab = new UI::Label(policy_win, 0, 24 * 10, "National ID card");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 11, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 11, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 12, "Men suffrage");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 13, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 13, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 14, "Men labour");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 15, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 15, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 16, "Women suffrage");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 17, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 17, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 18, "Women labour");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 19, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 19, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 20, "Private property");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 21, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 21, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 22, "Public property");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 23, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 23, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 24, "Corporations allowed");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 25, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 25, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 26, "Public education");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 27, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 27, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 28, "Private education");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 29, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 29, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 30, "Secular education");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 31, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 31, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 32, "Public healthcare");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 33, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 33, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 34, "Social security program");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 35, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 35, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 36, "Legislative parliament");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 37, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 37, 128, 24, "yes");
	
	lab = new UI::Label(policy_win, 0, 24 * 38, "Executive parliament");
	btn = new UI::Button(policy_win, 130 * 0, 24 * 39, 128, 24, "no");
	btn = new UI::Button(policy_win, 130 * 1, 24 * 39, 128, 24, "yes");
}
