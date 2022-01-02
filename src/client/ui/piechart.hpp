#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include <glm/vec2.hpp>
#include "unified_render/rectangle.hpp"
#include "unified_render/color.hpp"
#include "client/ui/widget.hpp"

namespace UI {
	class Context;
    class ChartData {
    public:
        ChartData(float _num, std::string _info, UnifiedRender::Color _color): num{_num}, info{_info}, color{_color} {}
        ChartData(float _num, std::string _info, uint32_t rgba): num{_num}, info{_info}, color{UnifiedRender::Color::rgba32(rgba)} {}
        float num;
        std::string info; // Used for tooltips
        UnifiedRender::Color color;
    };

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
