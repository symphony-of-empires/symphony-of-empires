#pragma once

#include "../map.hpp"
#include "client/ui.hpp"

class Map;
class MapDevView: public UI::Window {
public:
	MapDevView(Map* map);

	Map* map;
	UI::Button* reload_shader_button;
};