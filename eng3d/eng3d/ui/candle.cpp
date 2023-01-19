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
//      eng3d/ui/candle.cpp
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
#include "eng3d/ui/candle.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/primitive.hpp"
#include "eng3d/state.hpp"

UI::CandleChart::CandleChart(int _x, int _y, unsigned w, unsigned h, UI::Widget* _parent)
    : UI::Widget(_parent, _x, _y, w, h, UI::WidgetType::LABEL)
{
    auto& s = Eng3D::State::get_instance();
    this->current_texture = s.tex_man.load(s.package_man.get_unique("gfx/top_win_chart.png"));
}

void UI::CandleChart::set_data(std::vector<UI::CandleData> new_data) {
    this->data = new_data;
    this->min = this->max = glm::epsilon<float>();
    if(!this->data.empty())
    {
        for(const auto& candle : this->data)
        {
            this->max = glm::max(this->max, candle.max);
            this->min = glm::min(this->min, candle.min);
        }
    }
}

void UI::CandleChart::on_render(UI::Context& ctx, Eng3D::Rect viewport) {
    ctx.obj_shader->set_uniform("diffuse_color", glm::vec4(1.f));
    if(current_texture != nullptr)
        draw_rectangle(0, 0, width, height, viewport, current_texture.get());

    if(get_text_texture().get() != nullptr) {
        ctx.obj_shader->set_uniform("diffuse_color", glm::vec4(text_color.r, text_color.g, text_color.b, 1.f));
        draw_rectangle(4, 2, get_text_texture()->width, get_text_texture()->height, viewport, get_text_texture().get());
    }
    ctx.obj_shader->set_texture(0, "diffuse_map", *Eng3D::State::get_instance().tex_man.get_white());

    // Draw chart itself
    const glm::vec2 scaling{ (1.f / this->data.size()) * this->width, (1.f / this->max) * this->height };
    for(size_t i = 0; i < this->data.size(); i++)
    {
        const auto& slice = this->data[i];
        auto start = glm::vec2{ i, slice.open } * scaling;
        start.y = this->height - start.y;
        auto end = glm::vec2{ i + 1.f, slice.close } * scaling;
        end.y = this->height - end.y;
        
        // The center to draw the candle from
        const auto center = start + ((end - start) / 2.f);

        // Red = closing at lower than open
        // Green = closing at higher than open
        ctx.obj_shader->set_uniform("diffuse_color", glm::vec4(0.75f * (slice.close <= slice.open), 0.75f * (slice.close >= slice.open), 0.25f, 1.f));

        // Line of the candle
        auto line_mesh = Eng3D::Mesh<glm::vec2, glm::vec2>(Eng3D::MeshMode::LINES);
        line_mesh.buffer.resize(2);
        line_mesh.buffer[0] = Eng3D::MeshData(glm::vec2{ center.x, this->height - slice.max * scaling.y }, glm::vec2{});
        line_mesh.buffer[1] = Eng3D::MeshData(glm::vec2{ center.x, this->height - slice.min * scaling.y }, glm::vec2{});
        line_mesh.upload();
        line_mesh.draw();

        // Candlebox
        auto candle_mesh = Eng3D::Mesh<glm::vec2, glm::vec2>(Eng3D::MeshMode::TRIANGLE_STRIP);
        candle_mesh.buffer.resize(4);
        candle_mesh.buffer[0] = Eng3D::MeshData(glm::vec2{ start.x, start.y }, glm::vec2{});
        candle_mesh.buffer[1] = Eng3D::MeshData(glm::vec2{ start.x, end.y }, glm::vec2{});
        candle_mesh.buffer[2] = Eng3D::MeshData(glm::vec2{ end.x, start.y }, glm::vec2{});
        candle_mesh.buffer[3] = Eng3D::MeshData(glm::vec2{ end.x, end.y }, glm::vec2{});
        candle_mesh.upload();
        candle_mesh.draw();
    }
}