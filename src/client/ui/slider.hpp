#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include "client/ui/widget.hpp"
#include "client/ui/progress_bar.hpp"
namespace UI {
	class Slider: public ProgressBar {
	public:
		Slider(int x, int y, unsigned w, unsigned h, float min, float max, Widget* _parent = nullptr);
		virtual ~Slider() override {};
		//virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
		float max, min, value;
	};
};
