#include <cstdio>
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

#include "network.hpp"
#include "serializer.hpp"
#include "io_impl.hpp"

extern TextureManager* g_texture_manager;
int width = 1280;
int height = 800;

std::pair<int, int> mouse_pos;
static Map* map;

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
static Camera cam;
UI::Context* ui_ctx;

#include <atomic>
extern std::atomic<int> redraw;
extern std::atomic<bool> run;
extern std::atomic<bool> paused;
extern std::atomic<bool> do_start;

#include "pathfinding.hpp"
#include "array_ops.hpp"
SDL_Window* window;

static UI::Button* curr_country_btn;

enum MapMode {
	MAP_MODE_COUNTRY_SELECT,
	MAP_MODE_NORMAL,
};
MapMode current_mode = MAP_MODE_COUNTRY_SELECT;

static NationId curr_selected_nation = 0;
Nation* curr_nation = nullptr;

static void change_country(size_t id) {
	size_t old_id = curr_selected_nation;
	
	if(!g_world->nations.size()) {
		UI::Window* win = new UI::Window(mouse_pos.first, mouse_pos.second, 512, 32);
		win->text("No selectable countries!!");
		return;
	}
	
	curr_selected_nation = id;
	if(curr_selected_nation >= g_world->nations.size()) {
		curr_selected_nation = 0;
	}

	curr_nation = g_world->nations[curr_selected_nation];
	if(curr_nation->exists() == false) {
		if(curr_selected_nation > old_id) {
			while(curr_nation->exists() == false) {
				curr_selected_nation++;
				if(curr_selected_nation >= g_world->nations.size()) {
					curr_selected_nation = 0;
				}
				curr_nation = g_world->nations[curr_selected_nation];
			}
		} else {
			while(curr_nation->exists() == false) {
				curr_selected_nation--;
				if(curr_selected_nation >= g_world->nations.size()) {
					curr_selected_nation = g_world->nations.size() - 1;
				}
				curr_nation = g_world->nations[curr_selected_nation];
			}
		}
	}
	curr_country_btn->text(curr_nation->name.c_str());

	const Province* capital = curr_nation->capital;
	if(capital != nullptr) {
		cam.x = capital->max_x;
		cam.y = capital->max_y;
		cam.x = -cam.x;
	}
}

UI::Window* top_win;
static UI::Window* province_view_win;
static UI::Chart* gdp_chart,* pop_chart,* hdi_chart;
static UI::Label* total_pop_lab,* total_prestige_lab,* total_economy_lab;
static UI::Label* money_lab,* prestige_lab,* economy_lab,* big_brain_lab,* militancy_lab,* population_lab;
static UI::Image* money_icon,* prestige_icon,* economy_icon,* big_brain_icon,* militancy_icon,* population_icon;
static UI::Window* pop_view_nation_win = nullptr;
static uint8_t pop_view_nation_page_num = 0;
static void pop_view_nation(UI::Widget&, void *) {
	// Do not make duplicate windows
	if(pop_view_nation_win != nullptr) {
		return;
	}
	
	// View the provinces in a country - along with the population in them
	const Texture& province_pop_view_win = g_texture_manager->load_texture(Path::get("ui/province_pop_view_win.png"));
	const Texture& button_ppv = g_texture_manager->load_texture(Path::get("ui/button_ppv.png"));
	
	pop_view_nation_win = new UI::Window(mouse_pos.first, mouse_pos.second, province_pop_view_win.width, province_pop_view_win.height);
	pop_view_nation_win->current_texture = &province_pop_view_win;
	
	char* tmpbuf = new char[255];
	sprintf(tmpbuf, "Your province's POPs (page %zu)", (size_t)pop_view_nation_page_num);
	pop_view_nation_win->text(tmpbuf);
	delete[] tmpbuf;
	
	UI::Button* ok_btn = new UI::Button(9, 413, button_ppv.width, button_ppv.height, pop_view_nation_win);
	ok_btn->text("OK");
	ok_btn->current_texture = &button_ppv;\
	ok_btn->on_click = [](UI::Widget& w, void *) {
		pop_view_nation_win = nullptr;
		delete w.parent;
	};
	
	UI::Button* prev_btn = new UI::Button(193, 413, button_ppv.width, button_ppv.height, pop_view_nation_win);
	prev_btn->text("Previous");
	prev_btn->current_texture = &button_ppv;
	prev_btn->on_click = [](UI::Widget&, void *) {
		char* tmpbuf = new char[255];
		pop_view_nation_page_num--;
		sprintf(tmpbuf, "Your province's POPs (page %zu)", (size_t)pop_view_nation_page_num);
		pop_view_nation_win->text(tmpbuf);
		delete[] tmpbuf;
	};
	
	UI::Button* next_btn = new UI::Button(377, 413, button_ppv.width, button_ppv.height, pop_view_nation_win);
	next_btn->text("Next");
	next_btn->current_texture = &button_ppv;
	next_btn->on_click = [](UI::Widget&, void *) {
		char* tmpbuf = new char[255];
		pop_view_nation_page_num++;
		sprintf(tmpbuf, "Your province's POPs (page %zu)", (size_t)pop_view_nation_page_num);
		pop_view_nation_win->text(tmpbuf);
		delete[] tmpbuf;
	};
	
	for(size_t i = 0; i < 12; i++) {
		UI::Label* lab = new UI::Label(9, 43 + (i* 28), "?", pop_view_nation_win);
	}
}

static UI::Window* industry_view_nation_win = nullptr;
static uint8_t industry_view_nation_page_num = 0;
static void industry_view_nation(UI::Widget&, void *) {
	// Do not make duplicate windows
	if(industry_view_nation_win != nullptr) {
		return;
	}
	
	// View the provinces in a country - along with the population in them
	const Texture& industry_view_win = g_texture_manager->load_texture(Path::get("ui/province_industry_view_win.png"));
	const Texture& button_ppv = g_texture_manager->load_texture(Path::get("ui/button_ppv.png"));
	
	industry_view_nation_win = new UI::Window(mouse_pos.first, mouse_pos.second, industry_view_win.width, industry_view_win.height);
	industry_view_nation_win->current_texture = &industry_view_win;
	
	char* tmpbuf = new char[255];
	sprintf(tmpbuf, "Your province's industries (page %zu)", (size_t)pop_view_nation_page_num);
	industry_view_nation_win->text(tmpbuf);
	delete[] tmpbuf;
	
	UI::Button* ok_btn = new UI::Button(9, 413, button_ppv.width, button_ppv.height, industry_view_nation_win);
	ok_btn->text("OK");
	ok_btn->current_texture = &button_ppv;\
	ok_btn->on_click = [](UI::Widget& w, void *) {
		industry_view_nation_win = nullptr;
		delete w.parent;
	};
	
	UI::Button* prev_btn = new UI::Button(193, 413, button_ppv.width, button_ppv.height, industry_view_nation_win);
	prev_btn->text("Previous");
	prev_btn->current_texture = &button_ppv;
	prev_btn->on_click = [](UI::Widget&, void *) {
		char* tmpbuf = new char[255];
		industry_view_nation_page_num--;
		sprintf(tmpbuf, "Your province's industries (page %zu)", (size_t)industry_view_nation_page_num);
		industry_view_nation_win->text(tmpbuf);
		delete[] tmpbuf;
	};
	
	UI::Button* next_btn = new UI::Button(377, 413, button_ppv.width, button_ppv.height, industry_view_nation_win);
	next_btn->text("Next");
	next_btn->current_texture = &button_ppv;
	next_btn->on_click = [](UI::Widget&, void *) {
		char* tmpbuf = new char[255];
		industry_view_nation_page_num++;
		sprintf(tmpbuf, "Your province's industries (page %zu)", (size_t)industry_view_nation_page_num);
		industry_view_nation_win->text(tmpbuf);
		delete[] tmpbuf;
	};
	
	for(size_t i = 0; i < 12; i++) {
		UI::Label* lab = new UI::Label(9, 43 + (i* 28), "?", industry_view_nation_win);
	}
}

extern void ui_treaty(void);

std::vector<const Texture*> nation_flags;
std::vector<const Texture*> outpost_type_icons;

const Texture& get_nation_flag(Nation& nation) {
	return *nation_flags[g_world->get_id(&nation)];
}

std::mutex render_lock;
static void play_nation(UI::Widget&, void *) {
	ui_ctx->clear();
	current_mode = MAP_MODE_NORMAL;
	
	const Province* capital = curr_nation->capital;
	if(capital != nullptr) {
		cam.x = capital->max_x;
		cam.y = capital->max_y;
		cam.x = -cam.x;
		cam.z = -100.f;
	}
	
	const Texture& top_win_tex = g_texture_manager->load_texture(Path::get("ui/top_win.png"));
	const Texture& top_win_chart_tex = g_texture_manager->load_texture(Path::get("ui/top_win_chart.png"));
	const Texture& icon_prestige_tex = g_texture_manager->load_texture(Path::get("ui/icons/prestige.png"));
	const Texture& icon_economy_score_tex = g_texture_manager->load_texture(Path::get("ui/icons/economy_score.png"));
	const Texture& icon_militar_score_tex = g_texture_manager->load_texture(Path::get("ui/icons/militar_score.png"));
	const Texture& icon_naval_score_tex = g_texture_manager->load_texture(Path::get("ui/icons/naval_score.png"));
	const Texture& icon_money_tex = g_texture_manager->load_texture(Path::get("ui/icons/money.png"));
	const Texture& icon_militancy_tex = g_texture_manager->load_texture(Path::get("ui/icons/militancy.png"));
	const Texture& icon_big_brain_tex = g_texture_manager->load_texture(Path::get("ui/icons/big_brain.png"));
	const Texture& icon_population_tex = g_texture_manager->load_texture(Path::get("ui/icons/population.png"));
	
	// General statics of the nation
	top_win = new UI::Window(0, 0, top_win_tex.width, top_win_tex.height);
	top_win->text("Overview");
	top_win->current_texture = &top_win_tex;
	
	gdp_chart = new UI::Chart(504, 43, top_win_chart_tex.width, top_win_chart_tex.height, top_win);
	gdp_chart->text("GDP");
	gdp_chart->current_texture = &top_win_chart_tex;
	gdp_chart->data.clear();
	gdp_chart->on_click = &industry_view_nation;
	
	pop_chart = new UI::Chart(504, 120, top_win_chart_tex.width, top_win_chart_tex.height, top_win);
	pop_chart->text("Population");
	pop_chart->current_texture = &top_win_chart_tex;
	pop_chart->data.clear();
	pop_chart->on_click = &pop_view_nation;
	
	hdi_chart = new UI::Chart(504, 197, top_win_chart_tex.width, top_win_chart_tex.height, top_win);
	hdi_chart->text("HDI");
	hdi_chart->current_texture = &top_win_chart_tex;
	hdi_chart->data.clear();
	
	extern void ui_reform_policies(UI::Widget&, void *);
	hdi_chart->on_click = &ui_reform_policies;
	
	pop_view_nation_win = nullptr;
	
	UI::Image* current_flag = new UI::Image(9, 43, 188, 87, nation_flags[g_world->get_id(curr_nation)], top_win);
	
	money_icon = new UI::Image(209, 43 + (28* 0), icon_money_tex.width, icon_money_tex.height, &icon_money_tex, top_win);
	money_lab = new UI::Label(0, 43 + (28* 0), "?", top_win);
	money_lab->right_side_of(dynamic_cast<const UI::Widget&>(*money_icon));
	
	prestige_icon = new UI::Image(209, 43 + (28* 1), icon_prestige_tex.width, icon_prestige_tex.height, &icon_prestige_tex, top_win);
	prestige_lab = new UI::Label(0, 43 + (28* 1), "?", top_win);
	prestige_lab->right_side_of(dynamic_cast<const UI::Widget&>(*prestige_icon));
	
	economy_icon = new UI::Image(209, 43 + (28* 2), icon_economy_score_tex.width, icon_economy_score_tex.height, &icon_economy_score_tex, top_win);
	economy_lab = new UI::Label(0, 43 + (28* 2), "?", top_win);
	economy_lab->right_side_of(dynamic_cast<const UI::Widget&>(*economy_icon));
	
	big_brain_icon = new UI::Image(209, 43 + (28* 3), icon_big_brain_tex.width, icon_big_brain_tex.height, &icon_big_brain_tex, top_win);
	big_brain_lab = new UI::Label(0, 43 + (28* 3), "?", top_win);
	big_brain_lab->right_side_of(dynamic_cast<const UI::Widget&>(*big_brain_icon));
	
	militancy_icon = new UI::Image(209, 43 + (28* 4), icon_militancy_tex.width, icon_militancy_tex.height, &icon_militancy_tex, top_win);
	militancy_lab = new UI::Label(0, 43 + (28* 4), "?", top_win);
	militancy_lab->right_side_of(dynamic_cast<const UI::Widget&>(*militancy_icon));
	
	population_icon = new UI::Image(209, 43 + (28* 5), icon_population_tex.width, icon_population_tex.height, &icon_population_tex, top_win);
	population_lab = new UI::Label(0, 43 + (28* 5), "?", top_win);
	population_lab->right_side_of(dynamic_cast<const UI::Widget&>(*population_icon));
	
	do_start = true;

	// Select the nation
	g_client->packet_mutex.lock();
	Packet packet = Packet();
	Archive ar = Archive();
	enum ActionType action = ACTION_SELECT_NATION;
	::serialize(ar, &action);
	::serialize(ar, &curr_selected_nation);
	packet.data(ar.get_buffer(), ar.size());
	g_client->packet_queue.push_back(packet);
	g_client->packet_mutex.unlock();
}

void client_update(void) {
	// We are going to update widgets which require real-time feeding
	// this function **should** be called per tick
	std::unique_lock<std::mutex> lock(render_lock);
	
	if(current_mode == MAP_MODE_COUNTRY_SELECT) {
		return;
	}
	
	const Nation& player_nation = *g_world->nations[curr_selected_nation];
	if((g_world->time % 48) == 16) {
		// Charts for the GDP of the nation
		
		double gdp = 0.f;
		for(const auto& province: player_nation.owned_provinces) {
			for(const auto& product: g_world->products) {
				gdp += product->price* province->stockpile[g_world->get_id(product)];
			}
		}
		gdp_chart->data.push_back(gdp);
		if(gdp_chart->data.size() >= 30) {
			gdp_chart->data.pop_front();
		}
	} else if((g_world->time % 48) == 0) {
		// Charts for population and HDI
		
		size_t total_pop = 0;
		double living_std = 0.f;
		for(const auto& province: player_nation.owned_provinces) {
			for(const auto& pop: province->pops) {
				total_pop += pop.size;
				living_std += pop.life_needs_met;
			}
		}
		
		pop_chart->data.push_back(total_pop);
		if(pop_chart->data.size() >= 30) {
			pop_chart->data.pop_front();
		}
		
		hdi_chart->data.push_back(living_std / total_pop);
		if(hdi_chart->data.size() >= 30) {
			hdi_chart->data.pop_front();
		}
	}
	
	size_t total_pop = 0;
	double militancy = 0.f;
	double consciousness = 0.f;
	for(const auto& province: player_nation.owned_provinces) {
		for(const auto& pop: province->pops) {
			total_pop += pop.size;
			militancy += pop.militancy;
			consciousness += pop.consciousness;
		}
	}
	militancy /= total_pop;
	consciousness /= total_pop;
	
	char* tmpbuf = new char[255];
	
	sprintf(tmpbuf, " %12.2f", militancy);
	militancy_lab->text(tmpbuf);
	
	sprintf(tmpbuf, " %12.2f", consciousness);
	big_brain_lab->text(tmpbuf);
	
	sprintf(tmpbuf, " %12.2f", player_nation.prestige);
	prestige_lab->text(tmpbuf);
	
	sprintf(tmpbuf, " %12.2f", player_nation.economy_score);
	economy_lab->text(tmpbuf);
	
	sprintf(tmpbuf, " %12.2f", player_nation.budget);
	money_lab->text(tmpbuf);
	
	sprintf(tmpbuf, " %14zu", (size_t)total_pop);
	population_lab->text(tmpbuf);
	
	if(pop_view_nation_win != nullptr) {
		size_t e = pop_view_nation_page_num* 10;
		size_t i = 3;
		
		for(const auto& province: player_nation.owned_provinces) {
			if(e >= player_nation.owned_provinces.size()) {
				sprintf(tmpbuf, "?");
				pop_view_nation_win->children[i]->text(tmpbuf);
			} else {
				sprintf(tmpbuf, "%16s %4zu", province->name.c_str(), (size_t)province->total_pops());
				pop_view_nation_win->children[i]->text(tmpbuf);
			}
			
			if(i >= pop_view_nation_win->children.size())
				break;
			
			i++;
			e++;
		}
	} if(industry_view_nation_win != nullptr) {
		size_t e = industry_view_nation_page_num* 12;
		size_t i = 3;
		
		size_t total_industries = 0;
		for(const auto& province: player_nation.owned_provinces) {
			total_industries += province->industries.size();
		}

		for(const auto& province: player_nation.owned_provinces) {
			for(const auto& industry: province->industries) {
				if(e >= total_industries) {
					sprintf(tmpbuf, "?");
					industry_view_nation_win->children[i]->text(tmpbuf);
				} else {
					sprintf(tmpbuf, "%8s %4.2f %4zu %3zu", industry.type->name.c_str(), industry.production_cost, industry.workers);
					industry_view_nation_win->children[i]->text(tmpbuf);
				}
				
				i++;
				e++;
				
				if(i >= industry_view_nation_win->children.size())
					break;
			}
			
			if(i >= industry_view_nation_win->children.size())
				break;
		}
	}
	delete[] tmpbuf;
}

void view_province_pops(void) {
	UI::Window* view_pops_win = new UI::Window(0, province_view_win->disp_y, 0, 0);
}

// TODO: We can come with something better than this
void colonize_province(UI::Widget& w, void* data) {
	Province* province = (Province *)data;
	province->owner = g_world->nations[curr_selected_nation];
	curr_nation->budget -= 10000;
}

extern void ui_build_unit(Outpost* outpost);

std::vector<Event> displayed_events;
void select_nation(void) {
	g_world->client_update = &client_update;
	
	const Texture& province_view_win_tex = g_texture_manager->load_texture(Path::get("ui/province_view_win.png"));
	const Texture& nation_view_win_tex = g_texture_manager->load_texture(Path::get("ui/nation_view_win.png"));
	const Texture& debug_win_tex = g_texture_manager->load_texture(Path::get("ui/debug_win.png"));
	const Texture& generic_descision = g_texture_manager->load_texture(Path::get("ui/generic_descision_win.png"));
	
	const Texture& button_320 = g_texture_manager->load_texture(Path::get("ui/button_320.png"));
	const Texture& button_256 = g_texture_manager->load_texture(Path::get("ui/button_256.png"));
	const Texture& button_196 = g_texture_manager->load_texture(Path::get("ui/button_196.png"));
	const Texture& button_128 = g_texture_manager->load_texture(Path::get("ui/button_128.png"));
	
	const Texture& button_pvw = g_texture_manager->load_texture(Path::get("ui/button_pvw.png"));
	const Texture& button_popup = g_texture_manager->load_texture(Path::get("ui/button_popup.png"));
	
	const Texture& map_overlay = g_texture_manager->load_texture(Path::get("ui/map_overlay.png"));
	
	nation_flags.reserve(g_world->nations.size());
	for(const auto& nation: g_world->nations) {
		std::string pt;
		pt = "ui/flags/" + nation->ref_name + "_monarchy.png";
		nation_flags.push_back(&g_texture_manager->load_texture(Path::get(pt.c_str())));
	}

	outpost_type_icons.reserve(g_world->nations.size());
	for(const auto& outpost_type: g_world->outpost_types) {
		std::string pt;
		pt = "ui/icons/outpost_types/" + outpost_type->ref_name + ".png";
		outpost_type_icons.push_back(&g_texture_manager->load_texture(Path::get(pt.c_str())));
	}
	
	cam.x = -100.f;
	cam.y = 100.f;
	cam.z = -400.f;
	cam.z_angle = 0.f;
	
	cam.vx = 0.f;
	cam.vy = 0.f;
	cam.vz = 0.f;
	cam.vz_angle = 0.f;
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	
	run = true;
	
	std::pair<float, float> select_pos;
	
	UI::Button* select_country_btn = new UI::Button((width / 2) - (button_320.width / 2), 8, button_320.width, button_320.height);
	select_country_btn->current_texture = &button_320;
	select_country_btn->text("Select a country");

	curr_country_btn = new UI::Button((width / 2) - (button_320.width / 2), height - 128, button_320.width, button_320.height);
	curr_country_btn->current_texture = &button_320;
	curr_country_btn->text("!");
	UI::Button* next_country_btn = new UI::Button(0, height - 128, button_128.width, button_128.height);
	next_country_btn->current_texture = &button_128;
	next_country_btn->text("Next");
	next_country_btn->right_side_of(dynamic_cast<const UI::Widget&>(*curr_country_btn));
	next_country_btn->on_click = [](UI::Widget&, void *) {
		change_country(curr_selected_nation + 1);
	};
	UI::Button* prev_country_btn = new UI::Button(0, height - 128, button_128.width, button_128.height);
	prev_country_btn->current_texture = &button_128;
	prev_country_btn->text("Prev");
	prev_country_btn->left_side_of(dynamic_cast<const UI::Widget&>(*curr_country_btn));
	prev_country_btn->on_click = [](UI::Widget&, void *) {
		change_country(curr_selected_nation - 1);
	};
	
	UI::Button* play_btn = new UI::Button((width / 2) - (button_320.width / 2), height - button_320.height, button_320.width, button_320.height);
	play_btn->current_texture = &button_320;
	play_btn->text("Play");
	play_btn->on_click = &play_nation;
	
	Unit* selected_unit = nullptr;
	Outpost* selected_outpost = nullptr;
	
	size_t last_inbox_size = 0;
	uint64_t last_time = 0;

	displayed_events.clear();
	while(run) {
		if(last_time != g_world->time) {
			last_time = g_world->time;
			client_update();
		}
		
		std::unique_lock<std::mutex> lock(render_lock);
		
		SDL_Event event;
		int click_on_ui;
		
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
				ui_ctx->check_drag(mouse_pos.first, mouse_pos.second);
				break;
			case SDL_MOUSEBUTTONUP:
				SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
				click_on_ui = ui_ctx->check_click(mouse_pos.first, mouse_pos.second);
				if(click_on_ui == 0) {
					if(select_pos.first < 0 || select_pos.first >= g_world->width
					|| select_pos.second < 0 || select_pos.second >= g_world->height)
						continue;
					
					const Tile& tile = g_world->get_tile(select_pos.first, select_pos.second);
					
					switch(current_mode) {
					case MAP_MODE_COUNTRY_SELECT:
						change_country(tile.owner_id);
						break;
					case MAP_MODE_NORMAL:
						// See untis that have been clicked on
						if(event.button.button == SDL_BUTTON_LEFT) {
							// Check if we selected an unit
							selected_unit = nullptr;
							for(const auto& unit: g_world->units) {
								const float size = 2.f;
								if((int)select_pos.first > (int)unit->x - size
								&& (int)select_pos.first < (int)unit->x + size
								&& (int)select_pos.second > (int)unit->y - size
								&& (int)select_pos.second < (int)unit->y + size) {
									selected_unit = unit;
									break;
								}
							}
							if(selected_unit != nullptr)
								break;

							// Check if we selected an outpost
							selected_outpost = nullptr;
							for(const auto& outpost: g_world->outposts) {
								const float size = 2.f;
								if((int)select_pos.first > (int)outpost->x - size
								&& (int)select_pos.first < (int)outpost->x + size
								&& (int)select_pos.second > (int)outpost->y - size
								&& (int)select_pos.second < (int)outpost->y + size) {
									selected_outpost = outpost;
									break;
								}
							}
							if(selected_outpost != nullptr)
								break;
							
							// Tell the server about an action for building an outpost
							g_client->packet_mutex.lock();
							Packet packet = Packet();
							Archive ar = Archive();
							enum ActionType action = ACTION_OUTPOST_ADD;
							::serialize(ar, &action);
							Outpost outpost = Outpost();
							outpost.type = g_world->outpost_types[0];
							outpost.x = select_pos.first;
							outpost.y = select_pos.second;
							outpost.working_unit_type = nullptr;
							outpost.working_boat_type = nullptr;
							outpost.req_goods = outpost.type->req_goods;
							outpost.owner = g_world->nations[curr_selected_nation];
							::serialize(ar, &outpost); // OutpostObj
							packet.data(ar.get_buffer(), ar.size());
							g_client->packet_queue.push_back(packet);
							g_client->packet_mutex.unlock();
							break;
						} else if(event.button.button == SDL_BUTTON_RIGHT) {
							if(selected_unit != nullptr) {
								selected_unit->tx = select_pos.first;
								selected_unit->ty = select_pos.second;
								
								g_client->packet_mutex.lock();
								Packet packet = Packet();
								Archive ar = Archive();
								enum ActionType action = ACTION_UNIT_CHANGE_TARGET;
								::serialize(ar, &action);
								UnitId unit_id = g_world->get_id(selected_unit);
								::serialize(ar, &unit_id);
								::serialize(ar, &selected_unit->tx);
								::serialize(ar, &selected_unit->ty);
								packet.data(ar.get_buffer(), ar.size());
								g_client->packet_queue.push_back(packet);
								g_client->packet_mutex.unlock();
								break;
							} else if(selected_outpost != nullptr) {
								ui_build_unit(selected_outpost);
							} else {
								g_client->packet_mutex.lock();
								Packet packet = Packet();
								Archive ar = Archive();
								enum ActionType action = ACTION_UNIT_ADD;
								::serialize(ar, &action);
								Unit unit = Unit();
								unit.type = g_world->unit_types[0];
								unit.x = select_pos.first;
								unit.y = select_pos.second;
								unit.tx = unit.x;
								unit.ty = unit.y;
								unit.owner = g_world->nations[curr_selected_nation];
								unit.size = unit.type->max_health;
								::serialize(ar, &unit);
								packet.data(ar.get_buffer(), ar.size());
								g_client->packet_queue.push_back(packet);
								g_client->packet_mutex.unlock();
								break;
							}
						}
						
						if(tile.province_id != (ProvinceId)-1) {
							province_view_win = new UI::Window(0, 0, province_view_win_tex.width, province_view_win_tex.height);
							province_view_win->text("Province overview");
							province_view_win->current_texture = &province_view_win_tex;
							province_view_win->below_of(dynamic_cast<const UI::Widget&>(*top_win));
							
							const Texture& province_view_decor = g_texture_manager->load_texture(Path::get("ui/province_view_terrain.png"));
							UI::Image* view_province_decor = new UI::Image(9, 43, province_view_decor.width, province_view_decor.height, &province_view_decor, province_view_win);
							view_province_decor->text(g_world->provinces[tile.province_id]->name.c_str());
							
							UI::Button* view_province_pops = new UI::Button(9, 193, button_pvw.width, button_pvw.height, province_view_win);
							view_province_pops->text("Population");
							view_province_pops->current_texture = &button_pvw;
							view_province_pops->user_data = (void *)g_world->provinces[tile.province_id];
							
							UI::Button* view_province_ind = new UI::Button(9, 0, button_pvw.width, button_pvw.height, province_view_win);
							view_province_ind->text("Economic activity");
							view_province_ind->current_texture = &button_pvw;
							view_province_ind->below_of(dynamic_cast<const UI::Widget&>(*view_province_pops));
							view_province_ind->user_data = (void *)g_world->provinces[tile.province_id];
							
							UI::Button* view_province_owner = new UI::Button(9, 0, button_pvw.width, button_pvw.height, province_view_win);
							view_province_owner->text("Owner info");
							view_province_owner->current_texture = &button_pvw;
							view_province_owner->below_of(dynamic_cast<const UI::Widget&>(*view_province_ind));
							view_province_owner->user_data = (void *)g_world->provinces[tile.province_id];
							
							UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, button_pvw.width, button_pvw.height, province_view_win);
							ok_btn->text("OK");
							ok_btn->current_texture = &button_pvw;
							ok_btn->below_of(dynamic_cast<const UI::Widget&>(*view_province_owner));

							if(g_world->provinces[tile.province_id]->owner == nullptr) {
								UI::Button* colonize_province_btn = new UI::Button(9, 0, button_pvw.width, button_pvw.height, province_view_win);
								colonize_province_btn->text("Colonize");
								colonize_province_btn->current_texture = &button_pvw;
								colonize_province_btn->below_of(dynamic_cast<const UI::Widget&>(*ok_btn));
								colonize_province_btn->user_data = (void *)g_world->provinces[tile.province_id];
								colonize_province_btn->on_click = &colonize_province;
							}
						}
						break;
					}
				}
				break;
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
				ui_ctx->check_hover(mouse_pos.first, mouse_pos.second);

				select_pos.first = (mouse_pos.first - (width / 2.f)) / ((float)width / (-cam.z* 1.33f));
				select_pos.second = (mouse_pos.second - (height / 2.f)) / ((float)height / (-cam.z* 0.83f));
				select_pos.first -= cam.x;
				select_pos.second += cam.y;
				break;
			case SDL_MOUSEWHEEL:
				SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
				ui_ctx->check_hover(mouse_pos.first, mouse_pos.second);
				click_on_ui = ui_ctx->check_wheel(mouse_pos.first, mouse_pos.second, event.wheel.y* 6);
				if(!click_on_ui) {
					cam.vz += event.wheel.y;
				}
				break;
			case SDL_TEXTINPUT:
				ui_ctx->check_text_input((const char *)&event.text.text);
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym) {
				case SDLK_UP:
					cam.vy -= std::min(4.f, std::max(0.5f, 0.02f* -cam.z));
					break;
				case SDLK_DOWN:
					cam.vy += std::min(4.f, std::max(0.5f, 0.02f* -cam.z));
					break;
				case SDLK_LEFT:
					cam.vx += std::min(4.f, std::max(0.5f, 0.02f* -cam.z));
					break;
				case SDLK_RIGHT:
					cam.vx -= std::min(4.f, std::max(0.5f, 0.02f* -cam.z));
					break;
				case SDLK_q:
					cam.vz_angle -= std::min(4.f, std::max(0.01f, 0.02f* -cam.z));
					break;
				case SDLK_e:
					cam.vz_angle += std::min(4.f, std::max(0.01f, 0.02f* -cam.z));
					break;
				case SDLK_p:
					paused = !paused;
					break;
				case SDLK_t:
					ui_treaty();
					break;
				case SDLK_d:
					UI::Window* debug_win = new UI::Window(0, 0, debug_win_tex.width, debug_win_tex.height);
					debug_win->text("Debug");
					debug_win->current_texture = &debug_win_tex;
					debug_win->below_of(dynamic_cast<const UI::Widget&>(*top_win));
					
					UI::Button* overpopulate = new UI::Button(9, 43, button_pvw.width, button_pvw.height, debug_win);
					overpopulate->text("Spawn humans");
					overpopulate->current_texture = &button_pvw;
					overpopulate->on_click = [](UI::Widget&, void *) {
						for(const auto& province: g_world->provinces) {
							for(auto& pop: province->pops) {
								pop.size *= 2;
							}
						}
					};
					
					UI::Button* underpopulate = new UI::Button(9, 0, button_pvw.width, button_pvw.height, debug_win);
					underpopulate->text("Kill half humans");
					underpopulate->current_texture = &button_pvw;
					underpopulate->below_of(dynamic_cast<const UI::Widget&>(*overpopulate));
					underpopulate->on_click = [](UI::Widget&, void *) {
						for(const auto& province: g_world->provinces) {
							for(auto& pop: province->pops) {
								pop.size /= 2;
							}
						}
					};
					
					UI::Button* inc_product_price = new UI::Button(9, 0, button_pvw.width, button_pvw.height, debug_win);
					inc_product_price->text("Increment price");
					inc_product_price->current_texture = &button_pvw;
					inc_product_price->below_of(dynamic_cast<const UI::Widget&>(*underpopulate));
					inc_product_price->on_click = [](UI::Widget&, void *) {
						for(auto& product: g_world->products) {
							product->price *= 2;
						}
					};
					
					UI::Button* everyone_rich = new UI::Button(9, 0, button_pvw.width, button_pvw.height, debug_win);
					everyone_rich->text("Everyone is rich");
					everyone_rich->current_texture = &button_pvw;
					everyone_rich->below_of(dynamic_cast<const UI::Widget&>(*inc_product_price));
					everyone_rich->on_click = [](UI::Widget&, void *) {
						for(const auto& province: g_world->provinces) {
							for(auto& pop: province->pops) {
								pop.budget = 1000000.f;
							}
						}
					};
					
					UI::Button* everyone_poor = new UI::Button(9, 0, button_pvw.width, button_pvw.height, debug_win);
					everyone_poor->text("Everyone is poor");
					everyone_poor->current_texture = &button_pvw;
					everyone_poor->below_of(dynamic_cast<const UI::Widget&>(*everyone_rich));
					everyone_poor->on_click = [](UI::Widget&, void *) {
						for(const auto& province: g_world->provinces) {
							for(auto& pop: province->pops) {
								pop.budget = 0.f;
							}
						}
					};
					
					UI::Button* destroy_factories = new UI::Button(9, 0, button_pvw.width, button_pvw.height, debug_win);
					destroy_factories->text("Destroy factories");
					destroy_factories->current_texture = &button_pvw;
					destroy_factories->below_of(dynamic_cast<const UI::Widget&>(*everyone_poor));
					destroy_factories->on_click = [](UI::Widget&, void *) {
						for(auto& province: g_world->provinces) {
							province->industries.clear();
						}
					};
					break;
				}
				break;
			case SDL_QUIT:
				run = false;
				break;
			default:
				break;
			}
		}
		
		// Put popups
		if(current_mode == MAP_MODE_NORMAL) {
			size_t n_descisions = 0;
			for(auto& msg: curr_nation->inbox) {
				auto iter = std::find_if(displayed_events.begin(), displayed_events.end(), [&msg](Event& e) {
					return e.ref_name == msg.ref_name;
				});
				if(iter != displayed_events.end()) {
					continue;
				}

				UI::Window* popup_win = new UI::Window(128, 128, generic_descision.width, generic_descision.height);
				
				// TODO: Allow titles in events
				popup_win->text(msg.title.c_str());
				popup_win->current_texture = &generic_descision;

				// Separate the text line by line
				const char* buf = msg.text.c_str();
				char tmpbuf[24];
				size_t y = 38;
				while(strlen(buf)) {
					size_t t_len = std::min<size_t>(strlen(buf), 18);
					memcpy((char*)&tmpbuf, buf, t_len);
					tmpbuf[t_len] = '\0';
					new UI::Label(8, y, (const char*)&tmpbuf, popup_win);
					y += 24;
					buf += t_len;
				}

				// Buttons for descisions
				const UI::Button* last = nullptr;
				for(const auto& descision: msg.descisions) {
					UI::Button* decide_btn = new UI::Button(9, 558 - button_popup.height, button_popup.width, button_popup.height, popup_win);
					decide_btn->text(descision.name.c_str());
					decide_btn->current_texture = &button_popup;
					decide_btn->user_data = (void *)&descision;
					decide_btn->on_click = [](UI::Widget& w, void* data) {
						delete w.parent;
						
						// Obtain the descision
						Descision* descision = (Descision *)data;
						
						// Find event
						Event* event = nullptr;
						for(auto& e_event: displayed_events) {
							for(const auto& e_descision: e_event.descisions) {
								if(e_descision.ref_name == descision->ref_name) {
									event = &e_event;
									break;
								}
							}
							
							if(event != nullptr)
								break;
						}
						
						// Event not found
						if(event == nullptr) {
							print_error("Event not found, we tried finding by descision %s", descision->name.c_str());
							return;
						}
						
						g_client->packet_mutex.lock();
						Packet packet = Packet(g_client->get_fd());
						Archive ar = Archive();
						enum ActionType action = ACTION_NATION_TAKE_DESCISION;
						::serialize(ar, &action);
						::serialize(ar, &event->ref_name);
						::serialize(ar, &descision->ref_name);
						packet.data(ar.get_buffer(), ar.size());
						g_client->packet_queue.push_back(packet);
						g_client->packet_mutex.unlock();
						return;
					};
					
					if(last != nullptr) {
						decide_btn->above_of(dynamic_cast<const UI::Widget&>(*last));
					}
					last = decide_btn;
				}
				displayed_events.push_back(msg);
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
		
		map->draw(cam.z);
		
		glBindTexture(GL_TEXTURE_2D, map_overlay.gl_tex_num);
		glBegin(GL_QUADS);
		glColor4f(1.f, 1.f, 1.f, 0.8f);
		glTexCoord2f(0.f, 0.f);
		glVertex2f(0.f, 0.f);
		glTexCoord2f(1.f, 0.f);
		glVertex2f(0.f + g_world->width, 0.f);
		glTexCoord2f(1.f, 1.f);
		glVertex2f(0.f + g_world->width, 0.f + g_world->height);
		glTexCoord2f(0.f, 1.f);
		glVertex2f(0.f, 0.f + g_world->height);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		
		g_world->units_mutex.lock();
		for(const auto& unit: g_world->units) {
			const float size = 1.f;
			if(unit->size) {
				glBegin(GL_QUADS);
				glColor3f(0.f, 1.f, 0.f);
				glVertex2f(unit->x, unit->y - 1.f);
				glVertex2f(unit->x + (unit->size / unit->type->max_health), unit->y - 1.f);
				glVertex2f(unit->x + (unit->size / unit->type->max_health), unit->y - 1.f);
				glVertex2f(unit->x + (unit->size / unit->type->max_health), unit->y - 1.25f);
				glVertex2f(unit->x, unit->y - 1.2f);
				glEnd();
			}
			
			glBindTexture(GL_TEXTURE_2D, nation_flags[g_world->get_id(unit->owner)]->gl_tex_num);
			glBegin(GL_QUADS);
			glColor4f(1.f, 1.f, 1.f, 0.8f);
			glTexCoord2f(0.f, 0.f);
			glVertex2f(unit->x, unit->y);
			glTexCoord2f(1.f, 0.f);
			glVertex2f(unit->x + size, unit->y);
			glTexCoord2f(1.f, 1.f);
			glVertex2f(unit->x + size, unit->y + size);
			glTexCoord2f(0.f, 1.f);
			glVertex2f(unit->x, unit->y + size);
			glEnd();
		}
		g_world->units_mutex.unlock();

		g_world->outposts_mutex.lock();
		for(const auto& outpost: g_world->outposts) {
			glBindTexture(GL_TEXTURE_2D, nation_flags[g_world->get_id(outpost->owner)]->gl_tex_num);
			glBegin(GL_QUADS);
			glColor4f(1.f, 1.f, 1.f, 0.8f);
			glTexCoord2f(0.f, 0.f);
			glVertex2f(outpost->x, outpost->y);
			glTexCoord2f(1.f, 0.f);
			glVertex2f(outpost->x + 0.2f, outpost->y);
			glTexCoord2f(1.f, 1.f);
			glVertex2f(outpost->x + 0.2f, outpost->y + 0.2f);
			glTexCoord2f(0.f, 1.f);
			glVertex2f(outpost->x, outpost->y + 0.2f);
			glEnd();

			const float size = 1.f;
			glBindTexture(GL_TEXTURE_2D, outpost_type_icons[g_world->get_id(outpost->type)]->gl_tex_num);
			glBegin(GL_QUADS);
			glColor4f(1.f, 1.f, 1.f, 0.8f);
			glTexCoord2f(0.f, 0.f);
			glVertex2f(outpost->x, outpost->y);
			glTexCoord2f(1.f, 0.f);
			glVertex2f(outpost->x + size, outpost->y);
			glTexCoord2f(1.f, 1.f);
			glVertex2f(outpost->x + size, outpost->y + size);
			glTexCoord2f(0.f, 1.f);
			glVertex2f(outpost->x, outpost->y + size);
			glEnd();
		}
		g_world->outposts_mutex.unlock();
		glBindTexture(GL_TEXTURE_2D, 0);

		if(selected_unit != nullptr) {
			glBegin(GL_LINE_STRIP);
			glColor3f(1.f, 0.f, 0.f);
			glVertex2f(selected_unit->x, selected_unit->y);
			glVertex2f(selected_unit->x + 1.f, selected_unit->y);
			glVertex2f(selected_unit->x + 1.f, selected_unit->y + 1.f);
			glVertex2f(selected_unit->x, selected_unit->y + 1.f);
			glEnd();
		}

		glBegin(GL_QUADS);
		glColor3f(1.f, 1.f, 1.f);
		glVertex2f(select_pos.first, select_pos.second);
		glVertex2f(select_pos.first + 1.f, select_pos.second);
		glVertex2f(select_pos.first + 1.f, select_pos.second + 1.f);
		glVertex2f(select_pos.first, select_pos.second + 1.f);
		glEnd();

		glPopMatrix();
		
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

		cam.vx = std::min(16.f, cam.vx);
		cam.vy = std::min(16.f, cam.vy);
		cam.vz = std::min(16.f, cam.vz);

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

		cam.x = -std::max(0.f, std::min((float)g_world->width, -cam.x));
		cam.y = std::max(0.f, std::min((float)g_world->height, cam.y));
		cam.z = -std::max(0.f, std::min(750.f, -cam.z));
	}
	exit(EXIT_FAILURE);
}

#include <fstream>
void rendering_main(void) {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	
	window = SDL_CreateWindow("Symphony of Empires", 0, 0, width, height, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1); // Enable OpenGL VSYNC

	const GLubyte* version = glGetString(GL_VERSION);
	printf("OpenGL Version: %s\n", version);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	g_texture_manager = new TextureManager();
	ui_ctx = new UI::Context();

	printf("Creating map\n");
	map = new Map(*g_world);

	/*std::ifstream file;
	file.open(Path::get("shaders/test.gl"), std::ios::in);
	if(!file) {
		print_error("Cannot bind shader");
	}
	const size_t ps = file.tellg();
	file.seekg(0, std::ios::end);
	const size_t len = file.tellg();
	file.seekg(0, std::ios::beg);
	char* source = new char[len + 1];
	source[len] = 0;

	unsigned int i = 0;
	while(file.good()) {
		source[i] = file.get();
		if(!file.eof()) {
			i++;
		}
	}
	source[i] = 0;

	GLuint vertex_shader, fragment_shader;
	vertex_shader = glCreateShaderARB(GL_VERTEX_SHADER);
	fragment_shader = glCreateShaderARB(GL_FRAGMENT_SHADER);

	file.close();*/

	printf("Client ready\n");
	select_nation();

	TTF_Quit();
	SDL_Quit();
	delete ui_ctx;
	return;
}
