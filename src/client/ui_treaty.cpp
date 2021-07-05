#include "texture.hpp"
#include "ui.hpp"
#include "path.hpp"

extern TextureManager* g_texture_manager;
const Texture* button_pvw,* treaty_win_tex;
extern UI::Window* top_win;
extern std::pair<int, int> mouse_pos;

#include "nation.hpp"
extern Nation* curr_nation;

#include "serializer.hpp"
#include "io_impl.hpp"
#include "network.hpp"

UI::Label* recv_nation_lab;
UI::Image* recv_nation_flag;
UI::Window* select_win;
Nation* recv_nation = nullptr;

extern const Texture& get_nation_flag(Nation& nation);

void ui_treaty(void) {
	treaty_win_tex = &g_texture_manager->load_texture(Path::get("ui/debug_win.png"));
	button_pvw = &g_texture_manager->load_texture(Path::get("ui/button_pvw.png"));

	UI::Window* treaty_win = new UI::Window(0, 0, treaty_win_tex->width, treaty_win_tex->height);
	treaty_win->text("Draft Treaty");
	treaty_win->current_texture = treaty_win_tex;
	treaty_win->below_of(dynamic_cast<const UI::Widget&>(*top_win));

	UI::Button* select_receiver_btn = new UI::Button(9, 43, button_pvw->width, button_pvw->height, treaty_win);
	select_receiver_btn->current_texture = button_pvw;
	select_receiver_btn->text("Select nation");
	select_receiver_btn->on_click = [](UI::Widget&, void*) {
		select_win = new UI::Window(0, 0, treaty_win_tex->width, treaty_win_tex->height);
		select_win->text("Select nation");
		select_win->current_texture = treaty_win_tex;

		int y = 0;
		for(const auto& nation: g_world->nations) {
			UI::Button* select_receiver_btn = new UI::Button(9, y, button_pvw->width, button_pvw->height, select_win);
			select_receiver_btn->current_texture = button_pvw;
			select_receiver_btn->text(nation->name.c_str());
			select_receiver_btn->user_data = (void*)nation;
			select_receiver_btn->on_click = [](UI::Widget&, void* data) {
				recv_nation = (Nation*)data;
				delete select_win;
			};
			y += button_pvw->height + 2;
		}
	};

	recv_nation_lab = new UI::Label(64, 44, "...", treaty_win);
	recv_nation_lab->below_of(dynamic_cast<const UI::Widget&>(*select_receiver_btn));
	recv_nation_lab->on_update = [](UI::Widget& w, void*) {
		if(recv_nation == nullptr)
			return;
		w.text(recv_nation->name.c_str());
	};

	recv_nation_flag = new UI::Image(0, recv_nation_lab->y, 58, 24, nullptr, treaty_win);
	recv_nation_flag->on_update = [](UI::Widget& w, void*) {
		if(recv_nation == nullptr)
			return;
		w.current_texture = &get_nation_flag(*recv_nation);
	};

	UI::Button* send_btn = new UI::Button(9, 0, button_pvw->width, button_pvw->height, treaty_win);
	send_btn->below_of(dynamic_cast<const UI::Widget&>(*recv_nation_lab));
	send_btn->current_texture = button_pvw;
	send_btn->text("Select nation");
	send_btn->on_click = [](UI::Widget&, void*) {
		// Send treaty
	};
}
