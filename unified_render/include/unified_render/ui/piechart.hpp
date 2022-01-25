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
//      unified_render/ui/piechart.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include <glm/vec2.hpp>
#include "unified_render/rectangle.hpp"
#include "unified_render/color.hpp"
#include "unified_render/ui/widget.hpp"

namespace UI {
	class Context;
    /**
     * @ingroup UI
     * @brief Piechart widget
     *
     */

	class PieChart: public Widget {
	public:
		PieChart(int x, int y, unsigned w, unsigned h, std::vector<ChartData> data = std::vector<ChartData>(), Widget* _parent = nullptr);
		PieChart(int x, int y, unsigned w, unsigned h, Widget* _parent = nullptr);
		virtual ~PieChart() override {};
		virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
		void set_data(std::vector<ChartData> data);

	private:
        static void on_hover_default(Widget& w, glm::ivec2 mouse_pos, glm::ivec2 widget_pos);
		void draw_triangle(float start_angle, float end_angle, UnifiedRender::Color color);
		std::vector<ChartData> data;
		Tooltip* slice_tooltip;
		float max = 1.f;
	};
};
