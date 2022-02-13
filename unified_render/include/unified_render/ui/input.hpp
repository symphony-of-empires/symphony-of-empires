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
//      unified_render/ui/input.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include "unified_render/ui/widget.hpp"

namespace UI {
	class Context;
    /**
     * @ingroup UI
     * @brief Input widget for keyboard inputs
     *
     */
	class Input : public Widget {
	public:
		Input(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
		virtual ~Input() override {};
		
		static void on_click_default(Widget& w) {
			Input& input = static_cast<UI::Input&>(w);
			input.is_selected = true;
		};

		static void on_click_outside_default(Widget& w) {
			Input& input = static_cast<UI::Input&>(w);
			if(input.is_selected) {
				input.text(input.buffer);
			}
			input.is_selected = false;
		};

		static void on_update_default(Widget& w) {
			UI::Input& input = static_cast<UI::Input&>(w);
			input.timer = (input.timer + 1) % 60;
			std::string cursor = input.timer >= 30 ? "_" : "";
			if(input.is_selected && input.timer % 30 == 0) {
				if(!input.buffer.empty()) {
					input.text(input.buffer + cursor);
				} else if(!cursor.empty()) {
					input.text(cursor);
				}
			}
		};

		std::function<void(UI::Input&, const char*)> on_textinput;
		std::string buffer = "";
		bool is_selected = false;
	private:
		int timer; // TODO: Needs to not be frame dependand
	};
};