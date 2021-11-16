#include "client/interface/map_dev_view.hpp"

MapDevView::MapDevView(Map* _map)
	: map{ _map },
	UI::Window(0, 0, 512, 128)
{
	reload_shader_button = new UI::Button(10, 10, 256, 24, this);
	reload_shader_button->user_data = this;
	reload_shader_button->text("Reload shaders");
	reload_shader_button->on_click = ([](UI::Widget& w, void* data) {
		MapDevView* state = (MapDevView*)data;

		state->map->reload_shaders();
	});

	reload_shader_button = new UI::Button(10, 50, 256, 24, this);
	reload_shader_button->user_data = this;
	reload_shader_button->text("Regular mode");
	reload_shader_button->on_click = ([](UI::Widget& w, void* data) {
		MapDevView* state = (MapDevView*)data;

		state->map->set_view(MapView::PLANE_VIEW);
	});

	reload_shader_button = new UI::Button(10, 90, 256, 24, this);
	reload_shader_button->user_data = this;
	reload_shader_button->text("Based mode");
	reload_shader_button->on_click = ([](UI::Widget& w, void* data) {
		MapDevView* state = (MapDevView*)data;

		state->map->set_view(MapView::SPHERE_VIEW);
	});
}