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

static bool display_prov = false, display_pol = true, display_topo = false, display_infra = false;

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

static void do_province_overview() {
	Province * province = g_world->provinces[selected_province_id];
	UI::Window * prov_win = new UI::Window(ui_ctx, nullptr, width - 512, height / 2, 512, (height / 2) - 24, province->name.c_str());
}

std::vector<UI::Label *> wm_lab;
static void do_world_market_overview(UI::Widget *, void *) {
	UI::Window * wm_win = new UI::Window(ui_ctx, nullptr, 96, 196, 512 + 256, height - 256, "World market");

	char * str = new char[255];
	size_t i = 0;
	for(auto& product: g_world->products) {
		UI::Label * name = new UI::Label(ui_ctx, wm_win, 0, i * 24, g_world->goods[product->good_id]->name.c_str());
		wm_lab.push_back(name);

		sprintf(str, "%4.2f", product->price);
		UI::Label * price = new UI::Label(ui_ctx, wm_win, 128, i * 24, str);
		wm_lab.push_back(price);

		UI::Label * p_name = new UI::Label(ui_ctx, wm_win, 256, i * 24, g_world->provinces[product->origin_id]->name.c_str());
		wm_lab.push_back(p_name);

		sprintf(str, "%4.2f", product->price_vel);
		UI::Label * p_vel = new UI::Label(ui_ctx, wm_win, 256 + 128 + 64 + 64 + 32, i * 24, str);
		wm_lab.push_back(p_vel);

		i++;
	}
	delete[] str;
}

static void do_economy_overview(UI::Widget *, void *) {
	UI::Window * econ_win = new UI::Window(ui_ctx, nullptr, width - 512 - 256, 196, 512 + 256, height - 256, "Economy");
	
	UI::Button * econ_wm_btn = new UI::Button(ui_ctx, econ_win, 0, 0, 256, 24, "World market");
	econ_wm_btn->on_click = &do_world_market_overview;
}

std::vector<UI::Label *> iev_lab;
static void do_info_events_overview(UI::Widget *, void *) {
	UI::Window * iev_win = new UI::Window(ui_ctx, nullptr, width - 512 - 256, 196, 512, height - 256, "Events");

	size_t i = 0;
	for(auto& event: g_world->events) {
		UI::Label * lab = new UI::Label(ui_ctx, iev_win, 256, i * 24, event->ref_name.c_str());
		iev_lab.push_back(lab);
		i++;
	}
}
std::vector<UI::Label *> iprov_lab;
static void do_info_provinces_overview(UI::Widget *, void *) {
	UI::Window * iprov_win = new UI::Window(ui_ctx, nullptr, width - 512 - 256, 196, 512, height - 256, "Provinces");

	size_t i = 0;
	for(auto& province: g_world->provinces) {
		UI::Label * rn_lab = new UI::Label(ui_ctx, iprov_win, 0, i * 24, province->ref_name.c_str());
		iprov_lab.push_back(rn_lab);
		UI::Label * name_lab = new UI::Label(ui_ctx, iprov_win, 256, i * 24, province->name.c_str());
		iprov_lab.push_back(name_lab);
		i++;
	}
}
static void do_info_overview(UI::Widget *, void *) {
	UI::Window * info_win = new UI::Window(ui_ctx, nullptr, width - 96, 196, 96, height - 256, "Debug info");
	
	UI::Button * info_event_btn = new UI::Button(ui_ctx, info_win, 0, 0, 256, 24, "Events");
	info_event_btn->on_click = &do_info_events_overview;
	UI::Button * info_province_btn = new UI::Button(ui_ctx, info_win, 0, 32, 256, 24, "Provinces");
	info_province_btn->on_click = &do_info_provinces_overview;
	UI::Button * info_nation_btn = new UI::Button(ui_ctx, info_win, 0, 32, 256, 24, "Nations");
	info_nation_btn->on_click = &do_info_provinces_overview;
}

#include <atomic>
extern std::atomic<int> redraw;
extern std::atomic<int> run;

#include <deque>
#include <mutex>
std::mutex render_province_mutex;
std::deque<size_t> render_province;

void rendering_main(void) {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	player_nation_id = 0;
	
	SDL_Window * window = SDL_CreateWindow("superleaf1995", 0, 0, width, height, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	const GLubyte * version = glGetString(GL_VERSION);
	printf("OpenGL Version: %s\n", version);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	// Render g_world stuff now that we are in opengl
	map = Map(g_world);
	for(auto& nation: g_world->nations) {
		nation->default_flag->to_opengl();
	}

	ui_ctx = new UI::Context();

	/* left sidebar buttons */
	Texture help_icon = Texture(Resource_GetPath("icons/help.png").c_str());
	help_icon.to_opengl();
	UI::Button * help_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 4, 64, 64);
	help_btn->on_click = &do_info_overview;
	UI::Image * help_btn_icon = new UI::Image(ui_ctx, help_btn, 0, 0, 64, 64, nullptr, &help_icon);

	Texture budget_icon = Texture(Resource_GetPath("icons/budget.png").c_str());
	budget_icon.to_opengl();
	UI::Button * budget_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 5, 64, 64);
	budget_btn->on_click = &do_economy_overview;
	UI::Image * budget_btn_icon = new UI::Image(ui_ctx, budget_btn, 0, 0, 64, 64, nullptr, &budget_icon);

	Texture pol_view_icon = Texture(Resource_GetPath("icons/pol_view.png").c_str());
	pol_view_icon.to_opengl();
	UI::Button * pol_view_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 6, 64, 64);
	pol_view_btn->on_click = &do_view_pol_map;
	UI::Image * pol_view_btn_icon = new UI::Image(ui_ctx, pol_view_btn, 0, 0, 64, 64, nullptr, &pol_view_icon);

	Texture prov_view_icon = Texture(Resource_GetPath("icons/prov_view.png").c_str());
	prov_view_icon.to_opengl();
	UI::Button * prov_view_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 7, 64, 64);
	prov_view_btn->on_click = &do_view_prov_map;
	UI::Image * prov_view_btn_icon = new UI::Image(ui_ctx, prov_view_btn, 0, 0, 64, 64, nullptr, &prov_view_icon);

	Texture topo_view_icon = Texture(Resource_GetPath("icons/topo_view.png").c_str());
	topo_view_icon.to_opengl();
	UI::Button * topo_view_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 8, 64, 64);
	topo_view_btn->on_click = &do_view_topo_map;
	UI::Image * topo_view_btn_icon = new UI::Image(ui_ctx, topo_view_btn, 0, 0, 64, 64, nullptr, &topo_view_icon);

	Texture infra_view_icon = Texture(Resource_GetPath("icons/infra_view.png").c_str());
	infra_view_icon.to_opengl();
	UI::Button * infra_view_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 9, 64, 64);
	infra_view_btn->on_click = &do_view_infra_map;
	UI::Image * infra_view_btn_icon = new UI::Image(ui_ctx, infra_view_btn, 0, 0, 64, 64, nullptr, &infra_view_icon);

	Texture exit_icon = Texture(Resource_GetPath("icons/exit.png").c_str());
	exit_icon.to_opengl();
	UI::Button * exit_btn = new UI::Button(ui_ctx, nullptr, 8, 72 * 10, 64, 64);
	exit_btn->on_click = &do_exit;
	UI::Image * exit_btn_icon = new UI::Image(ui_ctx, exit_btn, 0, 0, 64, 64, nullptr, &exit_icon);

	Camera cam;
	cam.x = -1.f;
	cam.y = 1.f;
	cam.z = -100.f;

	cam.vx = 0.f;
	cam.vy = 0.f;
	cam.vz = 0.f;
	glClearColor(1.0f, 0.0f, 1.0f, 0.0f);

	char * dt_str = (char*)malloc(32);
	while(run) {
		SDL_Event event;
		int r;
		uint32_t beginTime = SDL_GetTicks();

		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mx, &my);
				
				r = ui_ctx->check_click(mx, my);

				if(fmx > 0 && fmx < g_world->width && fmy > 0 && fmy < g_world->height && !r) {
					// tx and ty are used for tile
					Tile * tile = &g_world->tiles[tx + ty * g_world->width];
					if(tile->province_id != (size_t)-1 && tile->owner_id != (size_t)-1) {
						selected_province_id = tile->province_id;
						do_province_overview();
					}
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
					cam.vy -= 0.02f * -cam.z;
					break;
				case SDLK_DOWN:
					cam.vy += 0.02f * -cam.z;
					break;
				case SDLK_LEFT:
					cam.vx += 0.02f * -cam.z;
					break;
				case SDLK_RIGHT:
					cam.vx -= 0.02f * -cam.z;
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
		glRotatef(0.f, 0.0f, 0.0f, 1.0f);

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
		if(display_topo)
			glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.quad_topo_gl_list_num);
		if(display_prov)
			glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.quad_div_gl_list_num);
		if(display_pol)
			glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.quad_pol_gl_list_num);
		if(display_infra)
			glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.infra_layout_list_num);
		
		// Borders should always display
		glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.div_borders_gl_list_num);
		glCallLists(map.n_horz_quads * map.n_vert_quads, GL_UNSIGNED_INT, map.pol_borders_gl_list_num);

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
		
		glBindTexture(GL_TEXTURE_2D, g_world->nations[player_nation_id]->default_flag->gl_tex_num);
		GLUquadricObj * sphere = nullptr;
		sphere = gluNewQuadric();
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluQuadricTexture(sphere, 1);
		gluQuadricNormals(sphere, GLU_SMOOTH);
		gluSphere(sphere, 0.5f, 20, 20);
		gluDeleteQuadric(sphere);
		glBindTexture(GL_TEXTURE_2D, 0);
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

		char str[255];
		sprintf((char *)&str, "%u/%u/%u - %u", year, month, day, hour);
		overview_time_label.text(ui_ctx, (char *)&str);
		*/

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

		cam.x += cam.vx;
		cam.y += cam.vy;
		cam.z += cam.vz;

		/*
		uint32_t endTime = SDL_GetTicks();
		uint32_t dt = endTime - beginTime;
		sprintf(dt_str, "%d", dt);
		delta_time.text(ui_ctx,dt_str);
		*/
	}

	TTF_Quit();
	SDL_Quit();
	delete ui_ctx;
	return;
}