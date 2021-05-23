#include "ui.hpp"
#include "world.hpp"
extern World * g_world;
extern int width;
extern int height;

static std::vector<UI::Label *> iev_lab;
static UI::Window * iev_win;
static void do_info_events_overview_or(UI::Widget *, void *) {
	const size_t n_labs = 1;
	if((iev_lab.size() / n_labs) < g_world->events.size()) {
		size_t i = iev_lab.size();
		while(i < g_world->events.size()) {
			UI::Label * lab = new UI::Label(iev_win, 256, i * 24, g_world->events[i]->ref_name.c_str());
			iev_lab.push_back(lab);
			i++;
		}
	} while((iev_lab.size() / n_labs) > g_world->events.size()) {
		delete iev_lab[iev_lab.size() - 1];
		iev_lab.pop_back();
	}
	for(size_t i = 0; i < g_world->events.size(); i++) {
		iev_lab[i * n_labs + 0]->text(g_world->events[i]->ref_name.c_str());
	}
}
static void do_info_events_overview(UI::Widget *, void *) {
	iev_win = new UI::Window(nullptr, width - 512 - 256, 196, 512, height - 256, "Events");
	iev_lab.clear();
	iev_win->on_update = &do_info_events_overview_or;
}

static std::vector<UI::Label *> iprov_lab;
static UI::Window * iprov_win;
static void do_info_provinces_overview(UI::Widget *, void *) {
	iprov_win = new UI::Window(nullptr, width - 512 - 256, 196, 512, height - 256, "Provinces");

	size_t i = 0;
	for(auto& province: g_world->provinces) {
		UI::Label * rn_lab = new UI::Label(iprov_win, 0, i * 24, province->ref_name.c_str());
		iprov_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(iprov_win, 256, i * 24, province->name.c_str());
		iprov_lab.push_back(name_lab);
		i++;
	}
}

static std::vector<UI::Label *> inat_lab;
static UI::Window * inat_win;
static void do_info_nations_overview(UI::Widget *, void *) {
	inat_win = new UI::Window(nullptr, width - 512 - 256, 196, 512, height - 256, "Nations");

	size_t i = 0;
	for(auto& nation: g_world->nations) {
		UI::Label * rn_lab = new UI::Label(inat_win, 0, i * 24, nation->ref_name.c_str());
		inat_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(inat_win, 256, i * 24, nation->name.c_str());
		inat_lab.push_back(name_lab);
		i++;
	}
}

static std::vector<UI::Label *> ipt_lab;
static UI::Window * ipt_win;
static void do_info_pop_types_overview(UI::Widget *, void *) {
	ipt_win = new UI::Window(nullptr, width - 512 - 256, 196, 512, height - 256, "POP types");

	size_t i = 0;
	for(auto& pop_type: g_world->pop_types) {
		UI::Label * rn_lab = new UI::Label(ipt_win, 0, i * 24, pop_type->ref_name.c_str());
		ipt_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(ipt_win, 256, i * 24, pop_type->name.c_str());
		ipt_lab.push_back(name_lab);
		i++;
	}
}

static std::vector<UI::Label *> icult_lab;
static UI::Window * icult_win;
static void do_info_cultures_overview(UI::Widget *, void *) {
	icult_win = new UI::Window(nullptr, width - 512 - 256, 196, 512, height - 256, "Cultures");

	size_t i = 0;
	for(auto& culture: g_world->cultures) {
		UI::Label * rn_lab = new UI::Label(icult_win, 0, i * 24, culture->ref_name.c_str());
		icult_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(icult_win, 256, i * 24, culture->name.c_str());
		icult_lab.push_back(name_lab);
		i++;
	}
}

static std::vector<UI::Label *> irel_lab;
static UI::Window * irel_win;
static void do_info_religions_overview(UI::Widget *, void *) {
	irel_win = new UI::Window(nullptr, width - 512 - 256, 196, 512, height - 256, "Religions");

	size_t i = 0;
	for(auto& religion: g_world->religions) {
		UI::Label * rn_lab = new UI::Label(irel_win, 0, i * 24, religion->ref_name.c_str());
		irel_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(irel_win, 256, i * 24, religion->name.c_str());
		irel_lab.push_back(name_lab);
		i++;
	}
}

static std::vector<UI::Widget *> iit_lab;
static UI::Window * iit_win;
static void do_view_industry_type_info(UI::Widget * w, void *) {
	IndustryType * industry = (IndustryType *)w->user_data;
	UI::Window * iit_win = new UI::Window(nullptr, 96, 196, 512, 512, industry->name.c_str());
	UI::Image * image = new UI::Image(iit_win, 8, 8, 320, 320, nullptr, industry->image);

	size_t y = 8;
	for(auto& input: industry->inputs) {
		UI::Image * icon = new UI::Image(iit_win, 336, 8 + y, 24, 24, nullptr, g_world->goods[input]->icon);
		y += 24;
	}

	y = 8;
	for(auto& output: industry->outputs) {
		UI::Image * icon = new UI::Image(iit_win, 336 + 64, 8 + y, 24, 24, nullptr, g_world->goods[output]->icon);
		y += 24;
	}
}

static void do_info_industry_types_overview(UI::Widget *, void *) {
	iit_win = new UI::Window(nullptr, width - 512 - 256, 196, 512, height - 256, "Industry Types");

	size_t i = 0;
	for(auto& industry_type: g_world->industry_types) {
		UI::Label * rn_lab = new UI::Label(iit_win, 0, i * 24, industry_type->name.c_str());
		iit_lab.push_back(rn_lab);
		UI::Button * info = new UI::Button(iit_win, 256, i * 24, 32, 24, "?");
		info->user_data = g_world->industry_types[i];
		info->on_click = &do_view_industry_type_info;
		iit_lab.push_back(info);
		i++;
	}
}

static std::vector<UI::Label *> igood_lab;
static UI::Window * igood_win;
static void do_info_goods_overview(UI::Widget *, void *) {
	igood_win = new UI::Window(nullptr, width - 512 - 256, 196, 512, height - 256, "Goods");

	size_t i = 0;
	for(auto& good: g_world->goods) {
		UI::Label * rn_lab = new UI::Label(igood_win, 0, i * 24, good->ref_name.c_str());
		igood_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(igood_win, 256, i * 24, good->name.c_str());
		igood_lab.push_back(name_lab);
		i++;
	}
}

void do_info_overview(UI::Widget *, void *) {
	UI::Window * info_win = new UI::Window(nullptr, width - 256, 196, 256, height - 256, "Debug info");
	
	UI::Button * info_event_btn = new UI::Button(info_win, 0, 32 * 0, 256, 24, "Events");
	info_event_btn->on_click = &do_info_events_overview;
	UI::Button * info_province_btn = new UI::Button(info_win, 0, 32 * 1, 256, 24, "Provinces");
	info_province_btn->on_click = &do_info_provinces_overview;
	UI::Button * info_nation_btn = new UI::Button(info_win, 0, 32 * 2, 256, 24, "Nations");
	info_nation_btn->on_click = &do_info_nations_overview;
	UI::Button * info_pop_type_btn = new UI::Button(info_win, 0, 32 * 3, 256, 24, "Pop types");
	info_pop_type_btn->on_click = &do_info_pop_types_overview;
	UI::Button * info_culture_btn = new UI::Button(info_win, 0, 32 * 4, 256, 24, "Cultures");
	info_culture_btn->on_click = &do_info_cultures_overview;
	UI::Button * info_religion_btn = new UI::Button(info_win, 0, 32 * 5, 256, 24, "Religions");
	info_religion_btn->on_click = &do_info_religions_overview;
	UI::Button * info_industry_type_btn = new UI::Button(info_win, 0, 32 * 6, 256, 24, "Industry types");
	info_industry_type_btn->on_click = &do_info_industry_types_overview;
	UI::Button * info_good_btn = new UI::Button(info_win, 0, 32 * 7, 256, 24, "Goods");
	info_good_btn->on_click = &do_info_goods_overview;
}