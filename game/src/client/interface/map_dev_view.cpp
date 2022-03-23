// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      client/interface/map_dev_view.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/interface/map_dev_view.hpp"
#include "unified_render/ui/button.hpp"

MapDevView::MapDevView(Map* _map)
	: UI::Window(0, 0, 512, 128),
	map{ _map }
{
	this->reload_shader_button = new UI::Button(10, 10, 256, 24, this);
	this->reload_shader_button->text("Reload shaders");
	this->reload_shader_button->on_click = ([this](UI::Widget& w) {
		this->map->reload_shaders();
	});

}