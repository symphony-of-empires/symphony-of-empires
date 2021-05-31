#include "ui.hpp"
#include "world.hpp"
extern World * g_world;
extern int width;
extern int height;

static void do_diplomacy_overview_for_nation(UI::Widget * w, void *) {
	const Nation * nation = (Nation *)w->user_data;
	
	UI::Window * diplo_win = new UI::Window(nullptr, 512, 128, 256 + 128, height - 128, "Diplomatic actions");
	
	UI::Button * btn1 = new UI::Button(diplo_win, 32, 24 * 2, 256, 24, "Increase relations");
	UI::Button * btn2 = new UI::Button(diplo_win, 32, 24 * 3, 256, 24, "Decrease relations");
	UI::Button * btn3 = new UI::Button(diplo_win, 32, 24 * 4, 256, 24, "Create alliance");
	UI::Button * btn16 = new UI::Button(diplo_win, 32, 24 * 20, 256, 24, "Goodbye");
}

void do_diplomacy_overview(UI::Widget *, void *) {
	UI::Window * diplomacy_win = new UI::Window(nullptr, 128, 128, 256 + 128, height - 128, "Diplomacy");
	
	for(size_t i = 0; i < g_world->nations.size(); i++) {
		UI::Image * flag = new UI::Image(diplomacy_win, 0, i * 32, 48, 24, nullptr, g_world->nations[i]->default_flag);
		UI::Label * lab = new UI::Label(diplomacy_win, 48, i * 32, g_world->nations[i]->name.c_str());
		UI::Button * btn = new UI::Button(diplomacy_win, 256, i * 32, 64, 24, "Meet");
		btn->user_data = (void *)g_world->nations[i];
		btn->on_click = &do_diplomacy_overview_for_nation;
	}
}