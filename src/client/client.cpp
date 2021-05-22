#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include "world.hpp"
#include "economy.hpp"
#include "texture.hpp"
#include "path.hpp"
#include "ui.hpp"
#include "map.hpp"

extern World * g_world;

const int width = 1280;
const int height = 800;

class Camera {
public:
	float vx;
	float vy;
	float vz;
	float x;
	float y;
	float z;
	float z_angle;
	float vz_angle;
};

UI::Context * ui_ctx;

static void do_exit(UI::Widget *, void *) {
	exit(EXIT_FAILURE);
}

static int mx, my;
static float fmx, fmy;
static int tx, ty;
static size_t player_nation_id;
static size_t selected_province_id;

static bool display_prov = false, display_pol = true, display_topo = true, display_infra = false;

static void do_view_prov_map(UI::Widget *, void *) {
	//map = prov_map;
	display_prov = !display_prov;
}

static void do_view_pol_map(UI::Widget *, void *) {
	//map = pol_map;
	display_pol = !display_pol;
}

static void do_view_topo_map(UI::Widget *, void *) {
	//map = topo_map;
	display_topo = !display_topo;
}

static void do_view_infra_map(UI::Widget *, void *) {
	//map = infra_map;
	display_infra = !display_infra;
}

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
	avail_soldiers->text(ui_ctx, tmpbuf);
	delete[] tmpbuf;
}
static void do_build_unit_on_province(UI::Widget * w, void *) {
	Province * province = (Province *)w->user_data;
	UI::Window * recruit_win = new UI::Window(ui_ctx, nullptr, 96, 196, 512, 512, "Recruit units");
	avail_soldiers = new UI::Label(ui_ctx, recruit_win, 0, 0, "?");
	avail_soldiers->user_data = province;
	avail_soldiers->on_update = &do_build_unit_on_province_on_update;

	char * tmpbuf = new char[255];
	size_t y = 64;
	for(const auto& unit_type: g_world->unit_types) {
		sprintf(tmpbuf, "%s %4.2f attack, %4.2f defense, %4.2f health", unit_type->name.c_str(), unit_type->attack, unit_type->defense, unit_type->max_health);
		UI::Button * recruit_unit = new UI::Button(ui_ctx, recruit_win, 0, y, 128, 24, tmpbuf);
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
		UI::Label * lab = new UI::Label(ui_ctx, prov_pop_win, 8, y, tmpbuf);
		y += 24;
		prov_pop_lab.push_back(lab);
	}
	delete[] tmpbuf;
}
static void do_province_overview_pops(UI::Widget * w, void *) {
	prov_pop_win = new UI::Window(ui_ctx, nullptr, 96, 196, 512, 512, "Province POPs");
	prov_pop_win->user_data = w->user_data;
	prov_pop_win->on_update = &do_province_pop_overview_on_update;
	prov_pop_lab.clear();
}

UI::Window * prov_win;
static void do_province_overview() {
	Province * province = g_world->provinces[selected_province_id];
	prov_win = new UI::Window(ui_ctx, nullptr, width - 512, height / 2, 512, (height / 2) - 24, province->name.c_str());
	
	char * str = new char[255];
	if(province->owner_id == PROVINCE_DISPUTED) {
		sprintf(str, "Disputed");
	} else if(province->owner_id == PROVINCE_NO_ONWER) {
		sprintf(str, "Uncolonized");
	} else {
		sprintf(str, "%s", g_world->nations[province->owner_id]->name.c_str());
	}

	UI::Label * status = new UI::Label(ui_ctx, prov_win, 0, 0, str);

	if(province->owner_id == player_nation_id) {
		UI::Button * recruit = new UI::Button(ui_ctx, prov_win, 0, 64, 128, 24, "Recruit");
		recruit->on_click = &do_build_unit_on_province;
		recruit->user_data = province;

		UI::Button * view_pop = new UI::Button(ui_ctx, prov_win, 0, 64 + 24, 128, 24, "View POPs");
		view_pop->on_click = &do_province_overview_pops;
		view_pop->user_data = province;
	}

	delete[] str;
}

std::vector<UI::Widget *> wm_lab;
UI::Window * wm_win = nullptr;
static void do_view_product_info(UI::Widget * w, void *) {
	Product * product = (Product *)w->user_data;
	UI::Window * prod_win = new UI::Window(ui_ctx, nullptr, 96, 196, 512, 512);

	UI::Chart * supply_chart = new UI::Chart(ui_ctx, prod_win, 24, 0, 482, 64);
	supply_chart->user_data = &product->supply_history;
	UI::Label * supply_chart_lab = new UI::Label(ui_ctx, prod_win, 24, 0, "Supply");

	UI::Chart * demand_chart = new UI::Chart(ui_ctx, prod_win, 24, 128, 482, 64);
	demand_chart->user_data = &product->demand_history;
	UI::Label * demand_chart_lab = new UI::Label(ui_ctx, prod_win, 24, 128, "Demand");

	UI::Chart * price_chart = new UI::Chart(ui_ctx, prod_win, 24, 256, 482, 64);
	price_chart->user_data = &product->price_history;
	UI::Label * price_chart_lab = new UI::Label(ui_ctx, prod_win, 24, 256, "Price");
}
static void do_world_market_overview_or(UI::Widget *, void *) {
	const size_t n_labs = 4;
	char * str = new char[255];
	for(size_t i = 0; i < g_world->products.size(); i++) {
		UI::Label * lab;
		wm_lab[i * 5 + 0]->text(ui_ctx, g_world->goods[g_world->products[i]->good_id]->name.c_str());
		sprintf(str, "%4.2f", g_world->products[i]->price);
		wm_lab[i * 5 + 1]->text(ui_ctx, str);
		sprintf(str, "%4.2f", g_world->products[i]->price_vel);
		wm_lab[i * 5 + 2]->text(ui_ctx, str);
		wm_lab[i * 5 + 3]->text(ui_ctx, g_world->provinces[g_world->products[i]->origin_id]->name.c_str());
	}
	delete[] str;
}
static void do_world_market_overview(UI::Widget *, void *) {
	wm_lab.clear();

	wm_win = new UI::Window(ui_ctx, nullptr, 96, 196, 512 + 256 + 128, height - 256, "World market");
	wm_win->on_update = &do_world_market_overview_or;
	for(size_t i = 0; i < g_world->products.size(); i++) {
		UI::Label * lab;
		UI::Button * info;
		lab = new UI::Label(ui_ctx, wm_win, 0, i * 24, "?");
		wm_lab.push_back(lab);
		lab = new UI::Label(ui_ctx, wm_win, 128, i * 24, "?");
		wm_lab.push_back(lab);
		lab = new UI::Label(ui_ctx, wm_win, 256, i * 24, "?");
		wm_lab.push_back(lab);
		lab = new UI::Label(ui_ctx, wm_win, 400, i * 24, "?");
		wm_lab.push_back(lab);
		info = new UI::Button(ui_ctx, wm_win, 700, i * 24, 32, 24, "?");
		info->user_data = g_world->products[i];
		info->on_click = &do_view_product_info;
		wm_lab.push_back(info);
	}
}

UI::Window * econ_win;
static void do_economy_overview(UI::Widget *, void *) {
	econ_win = new UI::Window(ui_ctx, nullptr, width - 512 - 256, 196, 512 + 256, height - 256, "Economy");
	
	UI::Button * econ_wm_btn = new UI::Button(ui_ctx, econ_win, 0, 0, 256, 24, "World market");
	econ_wm_btn->on_click = &do_world_market_overview;
}

std::vector<UI::Label *> iev_lab;
UI::Window * iev_win;
static void do_info_events_overview_or(UI::Widget *, void *) {
	const size_t n_labs = 1;
	if((iev_lab.size() / n_labs) < g_world->events.size()) {
		size_t i = iev_lab.size();
		while(i < g_world->events.size()) {
			UI::Label * lab = new UI::Label(ui_ctx, iev_win, 256, i * 24, g_world->events[i]->ref_name.c_str());
			iev_lab.push_back(lab);
			i++;
		}
	} while((iev_lab.size() / n_labs) > g_world->events.size()) {
		delete iev_lab[iev_lab.size() - 1];
		iev_lab.pop_back();
	}
	for(size_t i = 0; i < g_world->events.size(); i++) {
		iev_lab[i * n_labs + 0]->text(ui_ctx, g_world->events[i]->ref_name.c_str());
	}
}
static void do_info_events_overview(UI::Widget *, void *) {
	iev_win = new UI::Window(ui_ctx, nullptr, width - 512 - 256, 196, 512, height - 256, "Events");
	iev_lab.clear();
	iev_win->on_update = &do_info_events_overview_or;
}
std::vector<UI::Label *> iprov_lab;
UI::Window * iprov_win;
static void do_info_provinces_overview(UI::Widget *, void *) {
	iprov_win = new UI::Window(ui_ctx, nullptr, width - 512 - 256, 196, 512, height - 256, "Provinces");

	size_t i = 0;
	for(auto& province: g_world->provinces) {
		UI::Label * rn_lab = new UI::Label(ui_ctx, iprov_win, 0, i * 24, province->ref_name.c_str());
		iprov_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(ui_ctx, iprov_win, 256, i * 24, province->name.c_str());
		iprov_lab.push_back(name_lab);
		i++;
	}
}
std::vector<UI::Label *> inat_lab;
UI::Window * inat_win;
static void do_info_nations_overview(UI::Widget *, void *) {
	inat_win = new UI::Window(ui_ctx, nullptr, width - 512 - 256, 196, 512, height - 256, "Nations");

	size_t i = 0;
	for(auto& nation: g_world->nations) {
		UI::Label * rn_lab = new UI::Label(ui_ctx, inat_win, 0, i * 24, nation->ref_name.c_str());
		inat_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(ui_ctx, inat_win, 256, i * 24, nation->name.c_str());
		inat_lab.push_back(name_lab);
		i++;
	}
}
std::vector<UI::Label *> ipt_lab;
UI::Window * ipt_win;
static void do_info_pop_types_overview(UI::Widget *, void *) {
	ipt_win = new UI::Window(ui_ctx, nullptr, width - 512 - 256, 196, 512, height - 256, "POP types");

	size_t i = 0;
	for(auto& pop_type: g_world->pop_types) {
		UI::Label * rn_lab = new UI::Label(ui_ctx, ipt_win, 0, i * 24, pop_type->ref_name.c_str());
		ipt_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(ui_ctx, ipt_win, 256, i * 24, pop_type->name.c_str());
		ipt_lab.push_back(name_lab);
		i++;
	}
}
std::vector<UI::Label *> icult_lab;
UI::Window * icult_win;
static void do_info_cultures_overview(UI::Widget *, void *) {
	icult_win = new UI::Window(ui_ctx, nullptr, width - 512 - 256, 196, 512, height - 256, "Cultures");

	size_t i = 0;
	for(auto& culture: g_world->cultures) {
		UI::Label * rn_lab = new UI::Label(ui_ctx, icult_win, 0, i * 24, culture->ref_name.c_str());
		icult_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(ui_ctx, icult_win, 256, i * 24, culture->name.c_str());
		icult_lab.push_back(name_lab);
		i++;
	}
}
std::vector<UI::Label *> irel_lab;
UI::Window * irel_win;
static void do_info_religions_overview(UI::Widget *, void *) {
	irel_win = new UI::Window(ui_ctx, nullptr, width - 512 - 256, 196, 512, height - 256, "Religions");

	size_t i = 0;
	for(auto& religion: g_world->religions) {
		UI::Label * rn_lab = new UI::Label(ui_ctx, irel_win, 0, i * 24, religion->ref_name.c_str());
		irel_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(ui_ctx, irel_win, 256, i * 24, religion->name.c_str());
		irel_lab.push_back(name_lab);
		i++;
	}
}
std::vector<UI::Widget *> iit_lab;
UI::Window * iit_win;
static void do_view_industry_type_info(UI::Widget * w, void *) {
	IndustryType * industry = (IndustryType *)w->user_data;
	UI::Window * iit_win = new UI::Window(ui_ctx, nullptr, 96, 196, 512, 512, industry->name.c_str());
	UI::Image * image = new UI::Image(ui_ctx, iit_win, 8, 8, 320, 320, nullptr, industry->image);

	size_t y = 8;
	for(auto& input: industry->inputs) {
		UI::Image * icon = new UI::Image(ui_ctx, iit_win, 336, 8 + y, 24, 24, nullptr, g_world->goods[input]->icon);
		y += 24;
	}

	y = 8;
	for(auto& output: industry->outputs) {
		UI::Image * icon = new UI::Image(ui_ctx, iit_win, 336 + 64, 8 + y, 24, 24, nullptr, g_world->goods[output]->icon);
		y += 24;
	}
}
static void do_info_industry_types_overview(UI::Widget *, void *) {
	iit_win = new UI::Window(ui_ctx, nullptr, width - 512 - 256, 196, 512, height - 256, "Industry Types");

	size_t i = 0;
	for(auto& industry_type: g_world->industry_types) {
		UI::Label * rn_lab = new UI::Label(ui_ctx, iit_win, 0, i * 24, industry_type->name.c_str());
		iit_lab.push_back(rn_lab);
		UI::Button * info = new UI::Button(ui_ctx, iit_win, 256, i * 24, 32, 24, "?");
		info->user_data = g_world->industry_types[i];
		info->on_click = &do_view_industry_type_info;
		iit_lab.push_back(info);
		i++;
	}
}
std::vector<UI::Label *> igood_lab;
UI::Window * igood_win;
static void do_info_goods_overview(UI::Widget *, void *) {
	igood_win = new UI::Window(ui_ctx, nullptr, width - 512 - 256, 196, 512, height - 256, "Goods");

	size_t i = 0;
	for(auto& good: g_world->goods) {
		UI::Label * rn_lab = new UI::Label(ui_ctx, igood_win, 0, i * 24, good->ref_name.c_str());
		igood_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(ui_ctx, igood_win, 256, i * 24, good->name.c_str());
		igood_lab.push_back(name_lab);
		i++;
	}
}
static void do_info_overview(UI::Widget *, void *) {
	UI::Window * info_win = new UI::Window(ui_ctx, nullptr, width - 256, 196, 256, height - 256, "Debug info");
	
	UI::Button * info_event_btn = new UI::Button(ui_ctx, info_win, 0, 32 * 0, 256, 24, "Events");
	info_event_btn->on_click = &do_info_events_overview;
	UI::Button * info_province_btn = new UI::Button(ui_ctx, info_win, 0, 32 * 1, 256, 24, "Provinces");
	info_province_btn->on_click = &do_info_provinces_overview;
	UI::Button * info_nation_btn = new UI::Button(ui_ctx, info_win, 0, 32 * 2, 256, 24, "Nations");
	info_nation_btn->on_click = &do_info_nations_overview;
	UI::Button * info_pop_type_btn = new UI::Button(ui_ctx, info_win, 0, 32 * 3, 256, 24, "Pop types");
	info_pop_type_btn->on_click = &do_info_pop_types_overview;
	UI::Button * info_culture_btn = new UI::Button(ui_ctx, info_win, 0, 32 * 4, 256, 24, "Cultures");
	info_culture_btn->on_click = &do_info_cultures_overview;
	UI::Button * info_religion_btn = new UI::Button(ui_ctx, info_win, 0, 32 * 5, 256, 24, "Religions");
	info_religion_btn->on_click = &do_info_religions_overview;
	UI::Button * info_industry_type_btn = new UI::Button(ui_ctx, info_win, 0, 32 * 6, 256, 24, "Industry types");
	info_industry_type_btn->on_click = &do_info_industry_types_overview;
	UI::Button * info_good_btn = new UI::Button(ui_ctx, info_win, 0, 32 * 7, 256, 24, "Goods");
	info_good_btn->on_click = &do_info_goods_overview;
}

static void do_diplomacy_overview(UI::Widget *, void *) {
	UI::Window * diplomacy_win = new UI::Window(ui_ctx, nullptr, 128, 128, 256 + 128, height - 128, "Diplomacy");
	
	for(size_t i = 0; i < g_world->nations.size(); i++) {
		UI::Image * flag = new UI::Image(ui_ctx, diplomacy_win, 0, i * 32, 48, 24, nullptr, g_world->nations[i]->default_flag);
		UI::Label * lab = new UI::Label(ui_ctx, diplomacy_win, 48, i * 32, g_world->nations[i]->name.c_str());
		UI::Button * btn = new UI::Button(ui_ctx, diplomacy_win, 256, i * 32, 64, 24, "Meet");
	}
}

#include <atomic>
extern std::atomic<int> redraw;
extern std::atomic<int> run;

#include <deque>
#include <mutex>
std::mutex render_province_mutex;
std::deque<size_t> render_province;

#include "pathfinding.hpp"
SDL_Window * window;
void do_game_main(UI::Widget *, void *) {
	ui_ctx->clear();

	// Create map in here
	map = Map(g_world);

	// Siderbar buttons
	Texture diplomacy_icon = Texture(Path::get("icons/diplomacy.png").c_str());
	diplomacy_icon.to_opengl();
	UI::Button * diplomacy_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 3, 64, 64);
	UI::Image * diplomacy_btn_icon = new UI::Image(ui_ctx, diplomacy_btn, 0, 0, 64, 64, nullptr, &diplomacy_icon);
	diplomacy_btn_icon->on_click = &do_diplomacy_overview;

	Texture help_icon = Texture(Path::get("icons/help.png").c_str());
	help_icon.to_opengl();
	UI::Button * help_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 4, 64, 64);
	UI::Image * help_btn_icon = new UI::Image(ui_ctx, help_btn, 0, 0, 64, 64, nullptr, &help_icon);
	help_btn_icon->on_click = &do_info_overview;

	Texture budget_icon = Texture(Path::get("icons/budget.png").c_str());
	budget_icon.to_opengl();
	UI::Button * budget_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 5, 64, 64);
	UI::Image * budget_btn_icon = new UI::Image(ui_ctx, budget_btn, 0, 0, 64, 64, nullptr, &budget_icon);
	budget_btn_icon->on_click = &do_economy_overview;

	Texture pol_view_icon = Texture(Path::get("icons/pol_view.png").c_str());
	pol_view_icon.to_opengl();
	UI::Button * pol_view_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 6, 64, 64);
	UI::Image * pol_view_btn_icon = new UI::Image(ui_ctx, pol_view_btn, 0, 0, 64, 64, nullptr, &pol_view_icon);
	pol_view_btn_icon->on_click = &do_view_pol_map;

	Texture prov_view_icon = Texture(Path::get("icons/prov_view.png").c_str());
	prov_view_icon.to_opengl();
	UI::Button * prov_view_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 7, 64, 64);
	UI::Image * prov_view_btn_icon = new UI::Image(ui_ctx, prov_view_btn, 0, 0, 64, 64, nullptr, &prov_view_icon);
	prov_view_btn_icon->on_click = &do_view_prov_map;

	Texture topo_view_icon = Texture(Path::get("icons/topo_view.png").c_str());
	topo_view_icon.to_opengl();
	UI::Button * topo_view_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 8, 64, 64);
	UI::Image * topo_view_btn_icon = new UI::Image(ui_ctx, topo_view_btn, 0, 0, 64, 64, nullptr, &topo_view_icon);
	topo_view_btn_icon->on_click = &do_view_topo_map;

	Texture infra_view_icon = Texture(Path::get("icons/infra_view.png").c_str());
	infra_view_icon.to_opengl();
	UI::Button * infra_view_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 9, 64, 64);
	UI::Image * infra_view_btn_icon = new UI::Image(ui_ctx, infra_view_btn, 0, 0, 64, 64, nullptr, &infra_view_icon);
	infra_view_btn_icon->on_click = &do_view_infra_map;

	Texture exit_icon = Texture(Path::get("icons/exit.png").c_str());
	exit_icon.to_opengl();
	UI::Button * exit_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 10, 64, 64);
	UI::Image * exit_btn_icon = new UI::Image(ui_ctx, exit_btn, 0, 0, 64, 64, nullptr, &exit_icon);
	exit_btn_icon->on_click = &do_exit;

	// Top nation view
	UI::Window * top_win = new UI::Window(ui_ctx, nullptr, 0, 24, width / 2, 64, g_world->nations[player_nation_id]->name.c_str());
	UI::Image * top_flag = new UI::Image(ui_ctx, top_win, 8, 8, 96, 48, nullptr, g_world->nations[player_nation_id]->default_flag);
	UI::Label * top_prestige = new UI::Label(ui_ctx, top_win, 128, 8, "?");
	UI::Label * top_literacy = new UI::Label(ui_ctx, top_win, 256, 8, "?");
	UI::Label * top_diplomacy_pts = new UI::Label(ui_ctx, top_win, 128, 32, "?");

	Camera cam;
	cam.x = -100.f;
	cam.y = 100.f;
	cam.z = -100.f;
	cam.z_angle = 0.f;

	cam.vx = 0.f;
	cam.vy = 0.f;
	cam.vz = 0.f;
	cam.vz_angle = 0.f;
	glClearColor(1.0f, 0.0f, 1.0f, 0.0f);

	UI::Label * delta_time = new UI::Label(ui_ctx, nullptr, 0, height - 24, "?");

	std::vector<UI::Label *> map_province_names;
	map_province_names.reserve(g_world->nations.size());
	for(auto& province: g_world->provinces) {
		UI::Label * lab = new UI::Label(ui_ctx, nullptr, 0, 0, province->name.c_str());
		map_province_names.push_back(lab);
	}
	
	std::vector<Tile *> path;
	Tile * start;
	Tile * end;
	
	char * tmpbuf = new char[128];
	while(run) {
		SDL_Event event;
		int r;
		uint32_t begin_time = SDL_GetTicks();

		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mx, &my);
				
				r = ui_ctx->check_click(mx, my);

				if(!(fmx > 0 && fmx < g_world->width && fmy > 0 && fmy < g_world->height))
					break;
				
				if(event.button.button == SDL_BUTTON_LEFT && !r) {
					// tx and ty are used for tile
					Tile * tile = &g_world->tiles[tx + ty * g_world->width];
					if(tile->province_id != (size_t)-1 && tile->owner_id != (size_t)-1) {
						selected_province_id = tile->province_id;
						do_province_overview();
					}

					start = &g_world->tiles[tx + ty * g_world->width];
					
					// Place unit
					/*Unit * unit = new Unit();
					unit->health = 100.f;
					unit->owner_id = player_nation_id;
					unit->x = fmx;
					unit->y = fmy;
					g_world->units.push_back(unit);*/
				} else if(event.button.button == SDL_BUTTON_RIGHT && !r) {
					end = &g_world->tiles[tx + ty * g_world->width];
					path = find_path(*g_world, start, end);
					
					// Place enemy
					/*Unit * unit = new Unit();
					unit->health = 100.f;
					unit->owner_id = 10;
					unit->x = fmx;
					unit->y = fmy;
					g_world->units.push_back(unit);*/
				}
				break;
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&mx, &my);
				ui_ctx->check_hover(mx, my);

				fmx = mx - (width / 2.f);
				fmx /= ((float)width / (-cam.z * 1.33f));

				fmy = my - (height / 2.f);
				fmy /= ((float)height / (-cam.z * 0.83f));

				fmx += -cam.x;
				fmy += cam.y;

				fmx = (int)fmx;
				fmy = (int)fmy;
				tx = (int)fmx;
				ty = (int)fmy;
				break;
			case SDL_MOUSEWHEEL:
				SDL_GetMouseState(&mx, &my);
				ui_ctx->check_hover(mx, my);
				r = ui_ctx->check_wheel(mx, my, event.wheel.y * 6);
				if(!r) {
					cam.vz += event.wheel.y;
				}
				break;
			case SDL_TEXTINPUT:
				ui_ctx->check_text_input((const char *)&event.text.text);
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym) {
				case SDLK_UP:
				case SDLK_w:
					cam.vy -= fmin(4.f, fmax(0.5f, 0.02f * -cam.z));
					break;
				case SDLK_DOWN:
				case SDLK_s:
					cam.vy += fmin(4.f, fmax(0.5f, 0.02f * -cam.z));
					break;
				case SDLK_LEFT:
				case SDLK_a:
					cam.vx += fmin(4.f, fmax(0.5f, 0.02f * -cam.z));
					break;
				case SDLK_RIGHT:
				case SDLK_d:
					cam.vx -= fmin(4.f, fmax(0.5f, 0.02f * -cam.z));
					break;
				case SDLK_q:
					cam.vz_angle -= fmin(4.f, fmax(0.01f, 0.02f * -cam.z));
					break;
				case SDLK_e:
					cam.vz_angle += fmin(4.f, fmax(0.01f, 0.02f * -cam.z));
					break;
				}
				break;
			case SDL_QUIT:
				run = 0;
				break;
			default:
				break;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0, (float)width / (float)height, 1.0f, 1024.0f);
		glViewport(0, 0, width, height);
		glTranslatef(cam.x, cam.y, cam.z);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(180.f, 1.0f, 0.0f, 0.0f);
		glRotatef(0.f, 0.0f, 1.0f, 0.0f);
		glRotatef(cam.z_angle, 0.0f, 0.0f, 1.0f);
		
		render_province_mutex.lock();
		if(render_province.size() >= 4) {
			size_t min_x, min_y, max_x, max_y;
			
			min_x = render_province.front();
			render_province.pop_front();
			min_y = render_province.front();
			render_province.pop_front();
			max_x = render_province.front();
			render_province.pop_front();
			max_y = render_province.front();
			render_province.pop_front();
			
			map.quad_update_nation(min_x, min_y, max_x, max_y);
		}
		render_province_mutex.unlock();

		// Simpler than looping and indexing as x,y coord
		if(display_topo) {
			for(size_t qx = 0; qx < map.n_horz_quads; qx++) {
				for(size_t qy = 0; qy < map.n_vert_quads; qy++) {
					glBindTexture(GL_TEXTURE_2D, map.topo_tex[qy * map.n_horz_quads + qx]->gl_tex_num);
					
					const size_t x = qx * map.quad_size;
					const size_t y = qy * map.quad_size;
					
					glBegin(GL_TRIANGLES);
					glColor3f(1.f, 1.f, 1.f);
					glTexCoord2f(0.f, 0.f);
					glVertex2f(x, y);
					glTexCoord2f(1.f, 0.f);
					glVertex2f(x + map.quad_size, y);
					glTexCoord2f(1.f, 1.f);
					glVertex2f(x + map.quad_size, y + map.quad_size);
					glTexCoord2f(1.f, 1.f);
					glVertex2f(x + map.quad_size, y + map.quad_size);
					glTexCoord2f(0.f, 1.f);
					glVertex2f(x, y + map.quad_size);
					glTexCoord2f(0.f, 0.f);
					glVertex2f(x, y);
					glEnd();
				}
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			glColor3f(1.f, 1.f, 1.f);
		}
		if(display_prov)
			glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.provinces_fill);
		if(display_pol)
			glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.nations_fill);
		if(display_infra)
			glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.infrastructure_wire);
		
		// Borders should always display, except division borders
		if(cam.z >= -400.f) {
			glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.provinces_wire);
		}
		glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.nations_wire);
			
		glBegin(GL_POLYGON);
		glColor3f(1.f, 1.f, 1.f);
		glVertex2f(fmx, fmy);
		glColor3f(1.f, 1.f, 1.f);
		glVertex2f(fmx + 1.f, fmy);
		glColor3f(1.f, 1.f, 1.f);
		glVertex2f(fmx + 1.f, fmy + 1.f);
		glColor3f(1.f, 1.f, 1.f);
		glVertex2f(fmx, fmy + 1.f);
		glEnd();

		// Draw units
		for(size_t i = 0; i < g_world->units.size(); i++) {
			Unit * unit = g_world->units[i];
			glBindTexture(GL_TEXTURE_2D, g_world->nations[unit->owner_id]->default_flag->gl_tex_num);
			glBegin(GL_TRIANGLES);
			glColor3f(1.f, 1.f, 1.f);
			glTexCoord2f(0.f, 0.f);
			glVertex2f(unit->x, unit->y);
			glTexCoord2f(1.f, 0.f);
			glVertex2f(unit->x + 1.f, unit->y);
			glTexCoord2f(1.f, 1.f);
			glVertex2f(unit->x + 1.f, unit->y + 1.f);
			glTexCoord2f(1.f, 1.f);
			glVertex2f(unit->x + 1.f, unit->y + 1.f);
			glTexCoord2f(0.f, 1.f);
			glVertex2f(unit->x, unit->y + 1.f);
			glTexCoord2f(0.f, 0.f);
			glVertex2f(unit->x, unit->y);
			glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		
		for(const auto& unit: g_world->units) {
			glBegin(GL_LINE_STRIP);
			glColor3f(0.f, 1.f, 0.f);
			glVertex2f(unit->x, unit->y);
			glColor3f(1.f, 0.f, 0.f);
			glVertex2f(unit->tx, unit->ty);
			glEnd();
		}
		
		// Path thing
		glBegin(GL_LINE_STRIP);
		glLineWidth(4.f);
		glColor3f(1.f, 0.f, 0.f);
		for(const auto& node: path) {
			glVertex2f(node->x, node->y);
		}
		glLineWidth(1.f);
		glEnd();
		
		for(size_t i = 0; i < map_province_names.size(); i++) {
			//glBindTexture(GL_TEXTURE_2D, map_province_names[i]->text_texture->gl_tex_num);

			const size_t start_x = g_world->provinces[i]->min_x;
			const size_t start_y = g_world->provinces[i]->min_y;
			const size_t end_x = g_world->provinces[i]->max_x;
			const size_t end_y = g_world->provinces[i]->max_y;

			/*glBegin(GL_LINE_STRIP);
			glColor3f(1.f, 0.f, 0.f);
			glTexCoord2f(0.f, 0.f);
			glVertex2f(start_x, start_y);
			glTexCoord2f(1.f, 0.f);
			glVertex2f(end_x, start_y);
			glTexCoord2f(1.f, 1.f);
			glVertex2f(end_x, end_y);
			glTexCoord2f(0.f, 1.f);
			glVertex2f(start_x, end_y);
			glTexCoord2f(0.f, 1.f);
			glVertex2f(start_x, start_y);
			glEnd();*/

			/*glBegin(GL_QUADS);
			glColor3f(1.f, 1.f, 1.f);
			glTexCoord2f(0.f, 0.f);
			glVertex2f(start_x, start_y);
			glTexCoord2f(1.f, 0.f);
			glVertex2f(end_x, start_y);
			glTexCoord2f(1.f, 1.f);
			glVertex2f(end_x, end_y);
			glTexCoord2f(0.f, 1.f);
			glVertex2f(start_x, end_y);
			glEnd();*/
		}
		
		//glPopMatrix();
		
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.f, (float)width, (float)height, 0.f, 0.0f, 1.f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.f, 0.f, 0.f);
		ui_ctx->render_all();
		glPopMatrix();
		glLoadIdentity();
		glRasterPos2f(-3.0f, -2.0f);
		SDL_GL_SwapWindow(window);

		/*
		int hour = g_world->time % 24;
		int day = g_world->time / 24;

		int is_leap = day / (365 * 4);
		if(is_leap)
			day += (day / (365 * 4));
		int month = day / 31;
		int year = month / 12;

		day %= 31;
		month %= 12;

		sprintf(tmpbuf, "%u/%u/%u - %u", year, month, day, hour);
		overview_time_label.text(ui_ctx, tmpbuf);
		*/

		cam.vx = fmin(16.f, cam.vx);
		cam.vy = fmin(16.f, cam.vy);
		cam.vz = fmin(16.f, cam.vz);

		if(cam.vx >= 0.9f)
			cam.vx -= 0.8f;
		else if(cam.vx <= -0.9f)
			cam.vx += 0.8f;
		else
			cam.vx = 0.f;
		
		if(cam.vy >= 0.9f)
			cam.vy -= 0.8f;
		else if(cam.vy <= -0.9f)
			cam.vy += 0.8f;
		else
			cam.vy = 0.f;
		
		if(cam.vz >= 0.9f)
			cam.vz -= 0.8f;
		else if(cam.vz <= -0.9f)
			cam.vz += 0.8f;
		else
			cam.vz = 0.f;
		
		if(cam.vz_angle >= 0.9f)
			cam.vz_angle -= 0.8f;
		else if(cam.vz_angle <= -0.9f)
			cam.vz_angle += 0.8f;
		else
			cam.vz_angle = 0.f;

		cam.x += cam.vx;
		cam.y += cam.vy;
		cam.z += cam.vz;
		cam.z_angle += cam.vz_angle;

		cam.x = -fmax(0.f, fmin((float)g_world->width, -cam.x));
		cam.y = fmax(0.f, fmin((float)g_world->height, cam.y));
		cam.z = -fmax(0.f, fmin(750.f, -cam.z));

		uint32_t end_time = SDL_GetTicks();
		sprintf(tmpbuf, "%d FPS", 1000 / (end_time - begin_time));
		delta_time->text(ui_ctx, tmpbuf);

		sprintf(tmpbuf, "P %4.2f", g_world->nations[player_nation_id]->prestige);
		top_prestige->text(ui_ctx, tmpbuf);
		sprintf(tmpbuf, "L %4.2f", g_world->nations[player_nation_id]->base_literacy * 100);
		top_literacy->text(ui_ctx, tmpbuf);
		sprintf(tmpbuf, "D %4.2f", g_world->nations[player_nation_id]->diplomacy_points);
		top_diplomacy_pts->text(ui_ctx, tmpbuf);
	}
	delete[] tmpbuf;
}

void do_select_nation_via_flag(UI::Widget *, void * data) {
	Nation * nation = (Nation *)data;
	
	// Find nation by pointer (horrible idea but works)
	for(size_t i = 0; i < g_world->nations.size(); i++) {
		if(g_world->nations[i] != nation)
			continue;
		player_nation_id = i;
		break;
	}
}

void do_select_nation(UI::Widget *, void *) {
	ui_ctx->clear();

	Texture menu_bg = Texture(Path::get("title_bg.png").c_str());
	menu_bg.to_opengl();
	UI::Image * menu_bg_img = new UI::Image(ui_ctx, nullptr, 0, 0, width, height, nullptr, &menu_bg);

	UI::Label * lab = new UI::Label(ui_ctx, nullptr, 64, 64, "Select nation");

	UI::Window * select_window = new UI::Window(ui_ctx, nullptr, 64, 128, 512, 512);
	for(size_t i = 0; i < g_world->nations.size(); i++) {
		UI::Image * flag = new UI::Image(ui_ctx, select_window, 0, i * 52, 96, 48, nullptr, g_world->nations[i]->default_flag);
		flag->user_data = (void *)g_world->nations[i];
		flag->on_click = &do_select_nation_via_flag;
		UI::Label * name = new UI::Label(ui_ctx, select_window, 96, i * 52, g_world->nations[i]->name.c_str());
	}

	UI::Button * play = new UI::Button(ui_ctx, nullptr, 64, 640, 128, 24, "Play");
	play->on_click = &do_game_main;

	while(run) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mx, &my);
				ui_ctx->check_click(mx, my);
				break;
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&mx, &my);
				ui_ctx->check_hover(mx, my);
				break;
			case SDL_MOUSEWHEEL:
				SDL_GetMouseState(&mx, &my);
				ui_ctx->check_hover(mx, my);
				ui_ctx->check_wheel(mx, my, event.wheel.y * 6);
				break;
			case SDL_TEXTINPUT:
				ui_ctx->check_text_input((const char *)&event.text.text);
				break;
			case SDL_QUIT:
				run = 0;
				break;
			default:
				break;
			}
		}

		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.f, (float)width, (float)height, 0.f, 0.0f, 1.f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.f, 0.f, 0.f);

		// Draw UI and stuff
		ui_ctx->render_all();

		// Draw cool tilted flag
		glBindTexture(GL_TEXTURE_2D, g_world->nations[player_nation_id]->default_flag->gl_tex_num);
		glBegin(GL_TRIANGLES);
		glColor3f(0.5f, 0.5f, 1.f);
		glTexCoord2f(0.f, 0.f);
		float x = width - 512;
		float y = height - 256;
		float w = 512;
		float h = 256;
		glVertex2f(x, y);
		glTexCoord2f(1.f, 0.f);
		glVertex2f(x + w, y);
		glTexCoord2f(1.f, 1.f);
		glVertex2f(x + w, y + h);
		glTexCoord2f(1.f, 1.f);
		glVertex2f(x + w, y + h);
		glTexCoord2f(0.f, 1.f);
		glVertex2f(x, y + h);
		glTexCoord2f(0.f, 0.f);
		glVertex2f(x, y);
		glColor3f(1.f, 1.f, 1.f);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);

		glPopMatrix();
		glLoadIdentity();
		glRasterPos2f(-3.0f, -2.0f);
		SDL_GL_SwapWindow(window);
	}
}

void rendering_main(void) {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	player_nation_id = 0;
	
	window = SDL_CreateWindow("Symphony of Empires", 0, 0, width, height, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1); // Enable OpenGL VSYNC

	const GLubyte * version = glGetString(GL_VERSION);
	printf("OpenGL Version: %s\n", version);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	// Render g_world stuff now that we are in opengl
	for(auto& nation: g_world->nations) {
		nation->default_flag->to_opengl();
	}
	for(auto& industry: g_world->industry_types) {
		industry->image->to_opengl();
	}
	for(auto& good: g_world->goods) {
		good->icon->to_opengl();
	}

	ui_ctx = new UI::Context();

	Texture menu_bg = Texture(Path::get("title_bg.png").c_str());
	menu_bg.to_opengl();
	UI::Image * menu_bg_img = new UI::Image(ui_ctx, nullptr, 0, 0, width, height, nullptr, &menu_bg);
	Texture menu_title = Texture(Path::get("title.png").c_str());
	menu_title.to_opengl();
	UI::Image * menu_title_img = new UI::Image(ui_ctx, nullptr, 64, 8, 320, 128, nullptr, &menu_title);
	UI::Button * new_game = new UI::Button(ui_ctx, nullptr, 64, 196, 256, 24, "New game");
	new_game->on_click = &do_select_nation;
	while(run) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mx, &my);
				ui_ctx->check_click(mx, my);
				break;
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&mx, &my);
				ui_ctx->check_hover(mx, my);
				break;
			case SDL_MOUSEWHEEL:
				SDL_GetMouseState(&mx, &my);
				ui_ctx->check_hover(mx, my);
				ui_ctx->check_wheel(mx, my, event.wheel.y * 6);
				break;
			case SDL_TEXTINPUT:
				ui_ctx->check_text_input((const char *)&event.text.text);
				break;
			case SDL_QUIT:
				run = 0;
				break;
			default:
				break;
			}
		}

		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.f, (float)width, (float)height, 0.f, 0.0f, 1.f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.f, 0.f, 0.f);
		ui_ctx->render_all();
		glPopMatrix();
		glLoadIdentity();
		glRasterPos2f(-3.0f, -2.0f);
		SDL_GL_SwapWindow(window);
	}

	TTF_Quit();
	SDL_Quit();
	delete ui_ctx;
	return;
}
