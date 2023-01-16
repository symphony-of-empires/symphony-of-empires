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
//      eng3d/ui/numeric_label.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>

#include <glm/vec2.hpp>

#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/numeric_label.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/state.hpp"

UI::NumericLabel::NumericLabel(int _x, int _y, UI::Widget* _parent)
    : UI::Label(_x, _y, "", _parent)
{

}

UI::NumericLabel::~NumericLabel() {

}

void UI::NumericLabel::set_value(float v) {
    this->value = v;
    if(this->value < 0.f) // Red for below 0
        this->text_color = Eng3D::Color::rgb32(0xff800000);
    else // Green for above 0
        this->text_color = Eng3D::Color::rgb32(0xff008000);
    
    /// @todo User preferred percentage per locale?
    const auto new_text = this->is_percent
        ? Eng3D::string_format("%+.2f%%", this->value)
        : Eng3D::string_format("%+.2f", this->value);
    this->set_text(new_text);
}
