
#pragma once

#include "client/ui/group.hpp"

namespace UI {
	class Tab: public Group {
	public:
		Tab(int x, int y, unsigned w, unsigned h, UI::Widget* parent = nullptr);
		virtual ~Tab() override {};
	};
};