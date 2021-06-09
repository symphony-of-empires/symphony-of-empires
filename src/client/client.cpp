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

extern TextureManager * g_texture_manager;
int width = 1280;
int height = 800;

static Map * map;

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
UI::Context * ui_ctx;

#include <atomic>
extern std::atomic<int> redraw;
extern std::atomic<bool> run;
extern std::atomic<bool> paused;
extern std::atomic<bool> do_start;

#include "pathfinding.hpp"
#include "array_ops.hpp"
SDL_Window * window;

static UI::Button * curr_country_btn;

enum MapMode {
	MAP_MODE_COUNTRY_SELECT,
	MAP_MODE_NORMAL,
};
MapMode current_mode = MAP_MODE_COUNTRY_SELECT;

static size_t curr_selected_nation = 0;
static void change_country(size_t id) {
	size_t old_id = curr_selected_nation;
	
	curr_selected_nation = id;
	if(curr_selected_nation >= g_world->nations.size()) {
		curr_selected_nation = 0;
	}

	if(g_world->nations[curr_selected_nation]->exists() == false) {
		if(curr_selected_nation > old_id) {
			while(g_world->nations[curr_selected_nation]->exists() == false) {
				curr_selected_nation++;
				if(curr_selected_nation >= g_world->nations.size()) {
					curr_selected_nation = 0;
				}
			}
		} else {
			while(g_world->nations[curr_selected_nation]->exists() == false) {
				curr_selected_nation--;
				if(curr_selected_nation >= g_world->nations.size()) {
					curr_selected_nation = g_world->nations.size() - 1;
				}
			}
		}
	}

	curr_country_btn->text(g_world->nations[curr_selected_nation]->name.c_str());

	const Province * capital = g_world->nations[curr_selected_nation]->capital;
	if(capital != nullptr) {
		cam.x = capital->max_x;
		cam.y = capital->max_y;
		cam.x = -cam.x;
	}
}

static void next_nation(UI::Widget&, void *) {
	change_country(curr_selected_nation + 1);
}

static void prev_nation(UI::Widget&, void *) {
	change_country(curr_selected_nation - 1);
}

static UI::Window * top_win, * province_view_win;

static UI::Chart * gdp_chart, * pop_chart, * hdi_chart;
static UI::Label * total_pop_lab, * total_prestige_lab, * total_economy_lab;

static UI::Label * money_lab, * prestige_lab, * economy_lab, * big_brain_lab, * militancy_lab, * population_lab;
static UI::Image * money_icon, * prestige_icon, * economy_icon, * big_brain_icon, * militancy_icon, * population_icon;

std::mutex render_lock;
static void play_nation(UI::Widget&, void *) {
	ui_ctx->clear();
	current_mode = MAP_MODE_NORMAL;
	
	const Province& capital = *g_world->nations[curr_selected_nation]->capital;
	cam.x = capital.max_x;
	cam.y = capital.max_y;
	cam.x = -cam.x;
	cam.z = -100.f;
	
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
	
	top_win = new UI::Window(0, 0, top_win_tex.width, top_win_tex.height);
	top_win->text("Overview");
	top_win->current_texture = &top_win_tex;
	
	gdp_chart = new UI::Chart(504, 43, top_win_chart_tex.width, top_win_chart_tex.height, top_win);
	gdp_chart->text("GDP");
	gdp_chart->current_texture = &top_win_chart_tex;
	gdp_chart->data.clear();
	
	pop_chart = new UI::Chart(504, 120, top_win_chart_tex.width, top_win_chart_tex.height, top_win);
	pop_chart->text("Population");
	pop_chart->current_texture = &top_win_chart_tex;
	pop_chart->data.clear();
	
	hdi_chart = new UI::Chart(504, 197, top_win_chart_tex.width, top_win_chart_tex.height, top_win);
	hdi_chart->text("HDI");
	hdi_chart->current_texture = &top_win_chart_tex;
	hdi_chart->data.clear();
	
	UI::Image * current_flag = new UI::Image(9, 43, 188, 87, g_world->nations[curr_selected_nation]->default_flag, top_win);
	
	money_icon = new UI::Image(209, 43 + (28 * 0), icon_money_tex.width, icon_money_tex.height, &icon_money_tex, top_win);
	money_lab = new UI::Label(0, 43 + (28 * 0), "?", top_win);
	money_lab->right_side_of(dynamic_cast<const UI::Widget&>(*money_icon));
	
	prestige_icon = new UI::Image(209, 43 + (28 * 1), icon_prestige_tex.width, icon_prestige_tex.height, &icon_prestige_tex, top_win);
	prestige_lab = new UI::Label(0, 43 + (28 * 1), "?", top_win);
	prestige_lab->right_side_of(dynamic_cast<const UI::Widget&>(*prestige_icon));
	
	economy_icon = new UI::Image(209, 43 + (28 * 2), icon_economy_score_tex.width, icon_economy_score_tex.height, &icon_economy_score_tex, top_win);
	economy_lab = new UI::Label(0, 43 + (28 * 2), "?", top_win);
	economy_lab->right_side_of(dynamic_cast<const UI::Widget&>(*economy_icon));
	
	big_brain_icon = new UI::Image(209, 43 + (28 * 3), icon_big_brain_tex.width, icon_big_brain_tex.height, &icon_big_brain_tex, top_win);
	big_brain_lab = new UI::Label(0, 43 + (28 * 3), "?", top_win);
	big_brain_lab->right_side_of(dynamic_cast<const UI::Widget&>(*big_brain_icon));
	
	militancy_icon = new UI::Image(209, 43 + (28 * 4), icon_militancy_tex.width, icon_militancy_tex.height, &icon_militancy_tex, top_win);
	militancy_lab = new UI::Label(0, 43 + (28 * 4), "?", top_win);
	militancy_lab->right_side_of(dynamic_cast<const UI::Widget&>(*militancy_icon));
	
	population_icon = new UI::Image(209, 43 + (28 * 5), icon_population_tex.width, icon_population_tex.height, &icon_population_tex, top_win);
	population_lab = new UI::Label(0, 43 + (28 * 5), "?", top_win);
	population_lab->right_side_of(dynamic_cast<const UI::Widget&>(*population_icon));
	
	do_start = true;
}

void client_update(void) {
	std::unique_lock<std::mutex> lock(render_lock);
	
	const Nation& player_nation = *g_world->nations[curr_selected_nation];
	if((g_world->time % 48) == 16) {
		double gdp = 0.f;
		for(const auto& province: player_nation.owned_provinces) {
			for(const auto& product: g_world->products) {
				gdp += product->price * province->stockpile[g_world->get_id(product)];
			}
		}
		gdp_chart->data.push_back(gdp);
		if(gdp_chart->data.size() >= 30) {
			gdp_chart->data.pop_front();
		}
	} else if((g_world->time % 48) == 0) {
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
	
	char * tmpbuf = new char[255];
	
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
	
	sprintf(tmpbuf, " %14zu", total_pop);
	population_lab->text(tmpbuf);
	
	delete[] tmpbuf;
}

void view_province_pops(void) {
	UI::Window * view_pops_win = new UI::Window(0, province_view_win->disp_y, 0, 0);
}

// TODO: We can come with something better than this
void colonize_province(UI::Widget& w, void * data) {
	Province * province = (Province *)data;
	province->owner = g_world->nations[curr_selected_nation];
	g_world->nations[curr_selected_nation]->budget -= 10000;
}

void select_nation(void) {
	g_world->client_update = &client_update;
	
	const Texture& province_view_win_tex = g_texture_manager->load_texture(Path::get("ui/province_view_win.png"));
	const Texture& nation_view_win_tex = g_texture_manager->load_texture(Path::get("ui/nation_view_win.png"));
	const Texture& debug_win_tex = g_texture_manager->load_texture(Path::get("ui/debug_win.png"));
	
	const Texture& button_320 = g_texture_manager->load_texture(Path::get("ui/button_320.png"));
	const Texture& button_256 = g_texture_manager->load_texture(Path::get("ui/button_256.png"));
	const Texture& button_196 = g_texture_manager->load_texture(Path::get("ui/button_196.png"));
	const Texture& button_128 = g_texture_manager->load_texture(Path::get("ui/button_128.png"));
	
	const Texture& button_pvw = g_texture_manager->load_texture(Path::get("ui/button_pvw.png"));
	
	cam.x = -100.f;
	cam.y = 100.f;
	cam.z = -400.f;
	cam.z_angle = 0.f;
	
	cam.vx = 0.f;
	cam.vy = 0.f;
	cam.vz = 0.f;
	cam.vz_angle = 0.f;
	glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
	
	run = true;
	
	std::pair<int, int> mouse_pos;
	std::pair<float, float> select_pos;
	
	UI::Button * select_country_btn = new UI::Button((width / 2) - (button_320.width / 2), 8, button_320.width, button_320.height);
	select_country_btn->current_texture = &button_320;
	select_country_btn->text("Select a country");

	curr_country_btn = new UI::Button((width / 2) - (button_320.width / 2), height - 128, button_320.width, button_320.height);
	curr_country_btn->current_texture = &button_320;
	curr_country_btn->text("!");
	UI::Button * next_country_btn = new UI::Button(0, height - 128, button_128.width, button_128.height);
	next_country_btn->current_texture = &button_128;
	next_country_btn->text("Next");
	next_country_btn->right_side_of(dynamic_cast<const UI::Widget&>(*curr_country_btn));
	next_country_btn->on_click = &next_nation;
	UI::Button * prev_country_btn = new UI::Button(0, height - 128, button_128.width, button_128.height);
	prev_country_btn->current_texture = &button_128;
	prev_country_btn->text("Prev");
	prev_country_btn->left_side_of(dynamic_cast<const UI::Widget&>(*curr_country_btn));
	prev_country_btn->on_click = &prev_nation;

	UI::Button * play_btn = new UI::Button((width / 2) - (button_320.width / 2), height - button_320.height, button_320.width, button_320.height);
	play_btn->current_texture = &button_320;
	play_btn->text("Play");
	play_btn->on_click = &play_nation;
	
	while(run) {
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
						if(tile.province_id != (ProvinceId)-1) {
							province_view_win = new UI::Window(0, 0, province_view_win_tex.width, province_view_win_tex.height);
							province_view_win->text("Province information");
							province_view_win->current_texture = &province_view_win_tex;
							province_view_win->below_of(dynamic_cast<const UI::Widget&>(*top_win));
							
							UI::Button * view_province_pops = new UI::Button(9, 193, button_pvw.width, button_pvw.height, province_view_win);
							view_province_pops->text("Population");
							view_province_pops->current_texture = &button_pvw;
							view_province_pops->user_data = (void *)g_world->provinces[tile.province_id];
							
							UI::Button * view_province_ind = new UI::Button(9, 0, button_pvw.width, button_pvw.height, province_view_win);
							view_province_ind->text("Economic activity");
							view_province_ind->current_texture = &button_pvw;
							view_province_ind->below_of(dynamic_cast<const UI::Widget&>(*view_province_pops));
							view_province_ind->user_data = (void *)g_world->provinces[tile.province_id];
							
							UI::Button * view_province_owner = new UI::Button(9, 0, button_pvw.width, button_pvw.height, province_view_win);
							view_province_owner->text("Owner info");
							view_province_owner->current_texture = &button_pvw;
							view_province_owner->below_of(dynamic_cast<const UI::Widget&>(*view_province_ind));
							view_province_owner->user_data = (void *)g_world->provinces[tile.province_id];
							
							UI::CloseButton * ok_btn = new UI::CloseButton(9, 0, button_pvw.width, button_pvw.height, province_view_win);
							ok_btn->text("OK");
							ok_btn->current_texture = &button_pvw;
							ok_btn->below_of(dynamic_cast<const UI::Widget&>(*view_province_owner));

							if(g_world->provinces[tile.province_id]->owner == nullptr) {
								UI::Button * colonize_province_btn = new UI::Button(9, 0, button_pvw.width, button_pvw.height, province_view_win);
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

				select_pos.first = (mouse_pos.first - (width / 2.f)) / ((float)width / (-cam.z * 1.33f));
				select_pos.second = (mouse_pos.second - (height / 2.f)) / ((float)height / (-cam.z * 0.83f));
				select_pos.first -= cam.x;
				select_pos.second += cam.y;
				break;
			case SDL_MOUSEWHEEL:
				SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
				ui_ctx->check_hover(mouse_pos.first, mouse_pos.second);
				click_on_ui = ui_ctx->check_wheel(mouse_pos.first, mouse_pos.second, event.wheel.y * 6);
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
					cam.vy -= std::min(4.f, std::max(0.5f, 0.02f * -cam.z));
					break;
				case SDLK_DOWN:
					cam.vy += std::min(4.f, std::max(0.5f, 0.02f * -cam.z));
					break;
				case SDLK_LEFT:
					cam.vx += std::min(4.f, std::max(0.5f, 0.02f * -cam.z));
					break;
				case SDLK_RIGHT:
					cam.vx -= std::min(4.f, std::max(0.5f, 0.02f * -cam.z));
					break;
				case SDLK_q:
					cam.vz_angle -= std::min(4.f, std::max(0.01f, 0.02f * -cam.z));
					break;
				case SDLK_e:
					cam.vz_angle += std::min(4.f, std::max(0.01f, 0.02f * -cam.z));
					break;
				case SDLK_p:
					paused = !paused;
					break;
				case SDLK_d:
					UI::Window * debug_win = new UI::Window(0, 0, debug_win_tex.width, debug_win_tex.height);
					debug_win->text("Debug");
					debug_win->current_texture = &debug_win_tex;
					debug_win->below_of(dynamic_cast<const UI::Widget&>(*top_win));
					
					UI::Button * overpopulate = new UI::Button(9, 43, button_pvw.width, button_pvw.height, debug_win);
					overpopulate->text("Spawn humans");
					overpopulate->current_texture = &button_pvw;
					overpopulate->on_click = [](UI::Widget&, void *) {
						for(const auto& province: g_world->provinces) {
							for(auto& pop: province->pops) {
								pop.size *= 2;
							}
						}
					};
					
					UI::Button * underpopulate = new UI::Button(9, 0, button_pvw.width, button_pvw.height, debug_win);
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
					
					UI::Button * inc_product_price = new UI::Button(9, 0, button_pvw.width, button_pvw.height, debug_win);
					inc_product_price->text("Increment price");
					inc_product_price->current_texture = &button_pvw;
					inc_product_price->below_of(dynamic_cast<const UI::Widget&>(*underpopulate));
					inc_product_price->on_click = [](UI::Widget&, void *) {
						for(auto& product: g_world->products) {
							product->price *= 2;
						}
					};
					
					UI::Button * everyone_rich = new UI::Button(9, 0, button_pvw.width, button_pvw.height, debug_win);
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
					
					UI::Button * everyone_poor = new UI::Button(9, 0, button_pvw.width, button_pvw.height, debug_win);
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
					
					UI::Button * destroy_factories = new UI::Button(9, 0, button_pvw.width, button_pvw.height, debug_win);
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

	const GLubyte * version = glGetString(GL_VERSION);
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
	
	for(auto& nation: g_world->nations) {
		nation->default_flag->to_opengl();
	}

	/*std::ifstream file;
	file.open(Path::get("shaders/test.gl"), std::ios::in);
	if(!file) {
		print_error("Cannot bind shader");
	}
	const size_t ps = file.tellg();
	file.seekg(0, std::ios::end);
	const size_t len = file.tellg();
	file.seekg(0, std::ios::beg);
	char * source = new char[len + 1];
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
