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
int width = 1280;
int height = 800;

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
size_t player_nation_id;
size_t selected_province_id;

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

static void do_diplomacy_overview(UI::Widget *, void *) {
	UI::Window * diplomacy_win = new UI::Window(nullptr, 128, 128, 256 + 128, height - 128, "Diplomacy");
	
	for(size_t i = 0; i < g_world->nations.size(); i++) {
		UI::Image * flag = new UI::Image(diplomacy_win, 0, i * 32, 48, 24, nullptr, g_world->nations[i]->default_flag);
		UI::Label * lab = new UI::Label(diplomacy_win, 48, i * 32, g_world->nations[i]->name.c_str());
		UI::Button * btn = new UI::Button(diplomacy_win, 256, i * 32, 64, 24, "Meet");
	}
}

extern void do_info_overview(UI::Widget *, void *);
extern void do_economy_overview(UI::Widget *, void *);
extern void do_province_overview();

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
	UI::Button * diplomacy_btn = new UI::Button(nullptr, 8, 72 * 3, 64, 64);
	UI::Image * diplomacy_btn_icon = new UI::Image(diplomacy_btn, 0, 0, 64, 64, nullptr, &diplomacy_icon);
	diplomacy_btn_icon->on_click = &do_diplomacy_overview;

	Texture help_icon = Texture(Path::get("icons/help.png").c_str());
	help_icon.to_opengl();
	UI::Button * help_btn = new UI::Button( nullptr, 8, 72 * 4, 64, 64);
	UI::Image * help_btn_icon = new UI::Image(help_btn, 0, 0, 64, 64, nullptr, &help_icon);
	help_btn_icon->on_click = &do_info_overview;

	Texture budget_icon = Texture(Path::get("icons/budget.png").c_str());
	budget_icon.to_opengl();
	UI::Button * budget_btn = new UI::Button(nullptr, 8, 72 * 5, 64, 64);
	UI::Image * budget_btn_icon = new UI::Image(budget_btn, 0, 0, 64, 64, nullptr, &budget_icon);
	budget_btn_icon->on_click = &do_economy_overview;

	Texture pol_view_icon = Texture(Path::get("icons/pol_view.png").c_str());
	pol_view_icon.to_opengl();
	UI::Button * pol_view_btn = new UI::Button(nullptr, 8, 72 * 6, 64, 64);
	UI::Image * pol_view_btn_icon = new UI::Image(pol_view_btn, 0, 0, 64, 64, nullptr, &pol_view_icon);
	pol_view_btn_icon->on_click = &do_view_pol_map;

	Texture prov_view_icon = Texture(Path::get("icons/prov_view.png").c_str());
	prov_view_icon.to_opengl();
	UI::Button * prov_view_btn = new UI::Button(nullptr, 8, 72 * 7, 64, 64);
	UI::Image * prov_view_btn_icon = new UI::Image(prov_view_btn, 0, 0, 64, 64, nullptr, &prov_view_icon);
	prov_view_btn_icon->on_click = &do_view_prov_map;

	Texture topo_view_icon = Texture(Path::get("icons/topo_view.png").c_str());
	topo_view_icon.to_opengl();
	UI::Button * topo_view_btn = new UI::Button(nullptr, 8, 72 * 8, 64, 64);
	UI::Image * topo_view_btn_icon = new UI::Image(topo_view_btn, 0, 0, 64, 64, nullptr, &topo_view_icon);
	topo_view_btn_icon->on_click = &do_view_topo_map;

	Texture infra_view_icon = Texture(Path::get("icons/infra_view.png").c_str());
	infra_view_icon.to_opengl();
	UI::Button * infra_view_btn = new UI::Button( nullptr, 8, 72 * 9, 64, 64);
	UI::Image * infra_view_btn_icon = new UI::Image(infra_view_btn, 0, 0, 64, 64, nullptr, &infra_view_icon);
	infra_view_btn_icon->on_click = &do_view_infra_map;

	Texture exit_icon = Texture(Path::get("icons/exit.png").c_str());
	exit_icon.to_opengl();
	UI::Button * exit_btn = new UI::Button(nullptr, 8, 72 * 10, 64, 64);
	UI::Image * exit_btn_icon = new UI::Image(exit_btn, 0, 0, 64, 64, nullptr, &exit_icon);
	exit_btn_icon->on_click = &do_exit;

	// Top nation view
	UI::Window * top_win = new UI::Window(nullptr, 0, 24, width / 2, 64, g_world->nations[player_nation_id]->name.c_str());
	UI::Image * top_flag = new UI::Image(top_win, 8, 8, 96, 48, nullptr, g_world->nations[player_nation_id]->default_flag);
	UI::Label * top_prestige = new UI::Label(top_win, 128, 8, "?");
	UI::Label * top_literacy = new UI::Label(top_win, 256, 8, "?");
	UI::Label * top_diplomacy_pts = new UI::Label(top_win, 128, 32, "?");

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

	UI::Label * delta_time = new UI::Label(nullptr, 0, height - 24, "?");

	std::vector<UI::Label *> map_province_names;
	map_province_names.reserve(g_world->nations.size());
	for(auto& province: g_world->provinces) {
		UI::Label * lab = new UI::Label(nullptr, 0, 0, province->name.c_str());
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

				fmx = (mx - (width / 2.f)) / ((float)width / (-cam.z * 1.33f));
				fmy = (my - (height / 2.f)) / ((float)height / (-cam.z * 0.83f));
				fmx -= cam.x;
				fmy += cam.y;
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

		// Detailed map (more zoom)
		if(cam.z >= -400.f) {
			glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.provinces_wire);
			
			// Display all clouds
			for(auto& cloud: map.clouds) {
				const float x = cloud.x;
				const float y = cloud.y;
				const float size = 64.f;
				
				glBindTexture(GL_TEXTURE_2D, map.cloud_textures[cloud.type]->gl_tex_num);
				glBegin(GL_TRIANGLES);
				glColor3f(1.f, 1.f, 1.f);
				glTexCoord2f(0.f, 0.f);
				glVertex2f(x, y);
				glTexCoord2f(1.f, 0.f);
				glVertex2f(x + size, y);
				glTexCoord2f(1.f, 1.f);
				glVertex2f(x + size, y + size);
				glTexCoord2f(1.f, 1.f);
				glVertex2f(x + size, y + size);
				glTexCoord2f(0.f, 1.f);
				glVertex2f(x, y + size);
				glTexCoord2f(0.f, 0.f);
				glVertex2f(x, y);
				glEnd();

				cloud.x = (x < (float)-size) ? g_world->width : x;
				cloud.x -= 0.5f;
			}
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.nations_wire);

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

		glBegin(GL_QUADS);
		glColor3f(1.f, 1.f, 1.f);
		glVertex2f(fmx, fmy);
		glVertex2f(fmx + 1.f, fmy);
		glVertex2f(fmx + 1.f, fmy + 1.f);
		glVertex2f(fmx, fmy + 1.f);
		glEnd();
		
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
		overview_time_label.text(tmpbuf);
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
		delta_time->text(tmpbuf);

		sprintf(tmpbuf, "P %4.2f", g_world->nations[player_nation_id]->prestige);
		top_prestige->text(tmpbuf);
		sprintf(tmpbuf, "L %4.2f", g_world->nations[player_nation_id]->base_literacy * 100);
		top_literacy->text(tmpbuf);
		sprintf(tmpbuf, "D %4.2f", g_world->nations[player_nation_id]->diplomacy_points);
		top_diplomacy_pts->text(tmpbuf);
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
	UI::Image * menu_bg_img = new UI::Image(nullptr, 0, 0, width, height, nullptr, &menu_bg);

	UI::Label * lab = new UI::Label(nullptr, 64, 64, "Select nation");

	UI::Window * select_window = new UI::Window(nullptr, 64, 128, 512, 512);
	for(size_t i = 0; i < g_world->nations.size(); i++) {
		UI::Image * flag = new UI::Image(select_window, 0, i * 52, 96, 48, nullptr, g_world->nations[i]->default_flag);
		flag->user_data = (void *)g_world->nations[i];
		flag->on_click = &do_select_nation_via_flag;
		UI::Label * name = new UI::Label(select_window, 96, i * 52, g_world->nations[i]->name.c_str());
	}

	UI::Button * play = new UI::Button(nullptr, 64, 640, 128, 24, "Play");
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
	UI::Image * menu_bg_img = new UI::Image(nullptr, 0, 0, width, height, nullptr, &menu_bg);
	Texture menu_title = Texture(Path::get("title.png").c_str());
	menu_title.to_opengl();
	UI::Image * menu_title_img = new UI::Image(nullptr, 64, 8, 320, 128, nullptr, &menu_title);
	UI::Button * new_game = new UI::Button(nullptr, 64, 196, 256, 24, "New game");
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
