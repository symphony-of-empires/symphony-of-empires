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
//      eng3d/ui/numeric_label.hpp
//
// Abstract:
//      Numeric label, for displaying coloured text depending on the numeric
//      value of 'em.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/label.hpp"

namespace UI {
	class Context;
    /// @ingroup UI
    /// @brief Simple widget for drawing text on the screen, no multiline support
    class NumericLabel: public UI::Label {
        float value = 0.f;
    public:
        NumericLabel(int x, int y, Widget* parent = nullptr);
        virtual ~NumericLabel() override;

        void set_value(float v);

        float get_value() const {
            return this->value;
        }

        bool is_percent = 0.f; // Treat value as percent
    };
}
