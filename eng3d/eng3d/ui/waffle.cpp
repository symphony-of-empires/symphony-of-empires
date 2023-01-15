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
#include <numeric>
#include <algorithm>

#include <glm/vec2.hpp>
#include <glm/gtc/integer.hpp>

#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/waffle.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/state.hpp"

UI::WaffleChart::WaffleChart(int _x, int _y, unsigned w, unsigned h, UI::Widget* _parent)
    : UI::Widget(_parent, _x, _y, w, h, UI::WidgetType::LABEL)
{

}

void UI::WaffleChart::set_data(std::vector<UI::ChartData> new_data) {
    this->data = new_data;
    this->max = std::accumulate(this->data.begin(), this->data.end(), 0.f, [](const auto a, const auto& e) {
        return a + e.num;
    });
    this->cols = this->width / 8.f;
    this->rows = this->height / 8.f;
}

void UI::WaffleChart::on_render(UI::Context& ctx, Eng3D::Rect viewport) {
    ctx.obj_shader->set_uniform("diffuse_color", glm::vec4(1.f));
    if(current_texture != nullptr)
        draw_rectangle(0, 0, width, height, viewport, current_texture.get());

    if(get_text_texture().get() != nullptr) {
        ctx.obj_shader->set_uniform("diffuse_color", glm::vec4(text_color.r, text_color.g, text_color.b, 1.f));
        draw_rectangle(4, 2, get_text_texture()->width, get_text_texture()->height, viewport, get_text_texture().get());
    }

    ctx.obj_shader->set_texture(0, "diffuse_map", *Eng3D::State::get_instance().tex_man.get_white());

    // Draw chart itself
    const glm::vec2 scaling{ (1.f / this->cols) * this->width, (1.f / this->rows) * this->height };
    const glm::vec2 square_size{ this->width / this->cols, this->height / this->rows };
    const glm::vec2 aspect_ratio{ this->width / this->height, this->height / this->width };

    size_t total_squares = this->cols * this->rows, rem_squares = 0;
    auto it = this->data.cbegin();
    for(size_t sy = 0; sy < this->rows; sy++) {
        for(size_t sx = 0; sx < this->cols; sx++) {
            if(!rem_squares && it != this->data.cend()) {
                const auto& slice = *it++;
                ctx.obj_shader->set_uniform("diffuse_color", glm::vec4(slice.color.r, slice.color.g, slice.color.b, 1.f));
                rem_squares = (slice.num / this->max) * total_squares;
            }

            const auto w = 0.8f, h = 0.8f;
            auto mesh = Eng3D::Mesh<glm::vec2, glm::vec2>(Eng3D::MeshMode::TRIANGLE_STRIP);
            mesh.buffer.resize(4);
            mesh.buffer[0] = Eng3D::MeshData(glm::vec2{ sx, sy + h } * scaling, glm::vec2(0.f));
            mesh.buffer[1] = Eng3D::MeshData(glm::vec2{ sx, sy } * scaling, glm::vec2(0.f));
            mesh.buffer[2] = Eng3D::MeshData(glm::vec2{ sx + w, sy + h } * scaling, glm::vec2(0.f));
            mesh.buffer[3] = Eng3D::MeshData(glm::vec2{ sx + w, sy } * scaling, glm::vec2(0.f));
            mesh.upload();
            mesh.draw();
            rem_squares--;
        }
    }
}