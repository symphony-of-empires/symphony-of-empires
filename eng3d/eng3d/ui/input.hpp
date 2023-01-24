// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      eng3d/ui/input.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include "eng3d/ui/widget.hpp"

namespace UI {
	class Context;
    /// @ingroup UI
    /// @brief Input widget for keyboard inputs
	class Input : public Widget {
	public:
		Input(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
		virtual ~Input() override {}
		void set_buffer(const std::string& _buffer);
		std::string get_buffer() const;

		static void on_click_default(UI::Widget& w);
		static void on_click_outside_default(UI::Widget& w);
		static void on_update_default(UI::Widget& w);

		std::function<void(UI::Input&, const std::string_view)> on_textinput;
		bool is_selected = false;
	private:
		std::string buffer = "";
		int timer; /// @todo Needs to not be frame dependant
        size_t curpos = 0;
	};
}
