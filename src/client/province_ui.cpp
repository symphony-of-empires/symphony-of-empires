#include "ui.hpp"
#include "world.hpp"
extern World * g_world;
extern int width;
extern int height;

extern size_t player_nation_id;
extern size_t selected_province_id;

static UI::Label * avail_soldiers;
static void do_build_unit_on_province_on_update(UI::Widget * w, void *) {
	Province * province = (Province *)w->user_data;

	// Count soldier pops
	size_t avail_manpower = 0;
	for(const auto& pop: province->pops) {
		if(pop->type_id != POP_TYPE_SOLDIER)
			continue;
		
		avail_manpower += pop->size;
	}

	char * tmpbuf = new char[255];
	sprintf(tmpbuf, "Available soldiers: %zu", avail_manpower);
	avail_soldiers->text(tmpbuf);
	delete[] tmpbuf;
}

static void do_build_unit_on_province(UI::Widget * w, void *) {
	Province * province = (Province *)w->user_data;
	UI::Window * recruit_win = new UI::Window(nullptr, 96, 196, 512, 512, "Recruit units");
	avail_soldiers = new UI::Label(recruit_win, 0, 0, "?");
	avail_soldiers->user_data = province;
	avail_soldiers->on_update = &do_build_unit_on_province_on_update;

	char * tmpbuf = new char[255];
	size_t y = 64;
	for(const auto& unit_type: g_world->unit_types) {
		sprintf(tmpbuf, "%s %4.2f attack, %4.2f defense, %4.2f health", unit_type->name.c_str(), unit_type->attack, unit_type->defense, unit_type->max_health);
		UI::Button * recruit_unit = new UI::Button(recruit_win, 0, y, 128, 24, tmpbuf);
		y += 24;
	}
	delete[] tmpbuf;
}

static std::vector<UI::Widget *> prov_pop_lab;
static UI::Window * prov_pop_win;
static void do_province_pop_overview_on_update(UI::Widget * w, void *) {
	Province * province = (Province *)w->user_data;
	for(auto& lab: prov_pop_lab) {
		delete lab;
	}
	prov_pop_lab.clear();
	char * tmpbuf = new char[255];
	size_t y = 8;
	for(const auto& pop: province->pops) {
		sprintf(tmpbuf, "%s %s (%zu) %4.2f", g_world->cultures[pop->culture_id]->name.c_str(), g_world->pop_types[pop->type_id]->name.c_str(), pop->size, pop->budget);
		UI::Label * lab = new UI::Label(prov_pop_win, 8, y, tmpbuf);
		y += 24;
		prov_pop_lab.push_back(lab);
	}
	delete[] tmpbuf;
}

static void do_province_overview_pops(UI::Widget * w, void *) {
	prov_pop_win = new UI::Window(nullptr, 96, 196, 512, 512, "Province POPs");
	prov_pop_win->user_data = w->user_data;
	prov_pop_win->on_update = &do_province_pop_overview_on_update;
	prov_pop_lab.clear();
}

static UI::Window * prov_win;
void do_province_overview() {
	Province * province = g_world->provinces[selected_province_id];
	prov_win = new UI::Window(nullptr, width - 512, height / 2, 512, (height / 2) - 24, province->name.c_str());
	
	char * str = new char[255];
	if(province->owners.size() >= 1) {
		sprintf(str, "Disputed");
	} else if(province->owners.size() == 0) {
		sprintf(str, "Uncolonized");
	} else {
		sprintf(str, "%s", province->owners[0]->name.c_str());
	}

	UI::Label * status = new UI::Label(prov_win, 0, 0, str);

	if(std::find(province->owners.begin(), province->owners.end(), g_world->nations[player_nation_id]) != province->owners.end()) {
		UI::Button * recruit = new UI::Button(prov_win, 0, 64, 128, 24, "Recruit");
		recruit->on_click = &do_build_unit_on_province;
		recruit->user_data = province;
	}

	UI::Button * view_pop = new UI::Button(prov_win, 0, 64 + 24, 128, 24, "View POPs");
	view_pop->on_click = &do_province_overview_pops;
	view_pop->user_data = province;
	
	delete[] str;
}