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
//      eng3d/ui/chart.cpp
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
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/chart.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/state.hpp"

UI::Chart::Chart(int _x, int _y, unsigned w, unsigned h, UI::Widget* _parent)
    : UI::Widget(_parent, _x, _y, w, h, UI::WidgetType::LABEL)
{
    auto& s = Eng3D::State::get_instance();
    this->current_texture = s.tex_man.load(s.package_man.get_unique("gfx/top_win_chart.png"));
}

void UI::Chart::set_data(std::vector<float> new_data) {
    this->data = new_data;
    this->min = this->max = glm::epsilon<float>();
    if(!this->data.empty())
    {
        this->max = *std::max_element(this->data.cbegin(), this->data.cend());
        this->min = *std::min_element(this->data.cbegin(), this->data.cend());
    }
}

void UI::Chart::on_render(UI::Context& ctx, Eng3D::Rect viewport) {
    ctx.obj_shader->set_uniform("diffuse_color", glm::vec4(1.f));
    if(current_texture != nullptr)
        draw_rectangle(0, 0, width, height, viewport, current_texture.get());

    if(text_texture.get() != nullptr) {
        ctx.obj_shader->set_uniform("diffuse_color", glm::vec4(text_color.r, text_color.g, text_color.b, 1.f));
        draw_rectangle(4, 2, text_texture->width, text_texture->height, viewport, text_texture.get());
    }

    ctx.obj_shader->set_uniform("diffuse_color", glm::vec4(1.f, 0.f, 0.f, 1.f));
    ctx.obj_shader->set_texture(0, "diffuse_map", *Eng3D::State::get_instance().tex_man.get_white());

    // Draw chart itself
    auto mesh = Eng3D::Mesh<glm::vec2, glm::vec2>(Eng3D::MeshMode::LINE_STRIP);
    mesh.buffer.resize(this->data.size());

    const glm::vec2 scaling{ (1.f / (this->data.size() - 1)) * this->width, (1.f / this->max) * this->height };
    for(size_t i = 0; i < this->data.size(); i++)
    {
        const auto& slice = this->data[i];
        auto slice_pos = glm::vec2{ i, slice } * scaling;
        slice_pos.y = this->height - slice_pos.y;
        mesh.buffer[i] = Eng3D::MeshData(slice_pos, glm::vec2(0.f));
    }
    mesh.upload();
    mesh.draw();
}