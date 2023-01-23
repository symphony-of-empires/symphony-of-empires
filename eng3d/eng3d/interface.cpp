// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
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
//      interface.cpp
//
// Abstract:
//      Provides common UI elements from Eng3D to debug/profile or do other
//      things in-game.
// ----------------------------------------------------------------------------

#include "eng3d/interface.hpp"
#include "eng3d/profiler.hpp"
#include "eng3d/color.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/components.hpp"
#include "eng3d/state.hpp"
#include "eng3d/string.hpp"

Eng3D::Interface::ProfilerView::ProfilerView(Eng3D::State& _s, Eng3D::Profiler& _profiler)
    : UI::Window(0, 0, 240, _s.width, nullptr),
    s{ _s },
    profiler{ _profiler }
{
    this->padding.x = 0;
    this->padding.y = 48;
    this->set_text(translate("Performance profiler"));
    this->is_scroll = false;
    this->current_texture.reset();

    this->set_close_btn_function([this](UI::Widget&) {
        this->kill();
    });

    auto& fps_lab = this->make_widget<UI::Label>(10, 0, "?");
    fps_lab.on_update = ([this](UI::Widget& w) {
        w.set_text(translate_format("FPS: %.2f", this->profiler.get_fps()));
    });
    fps_lab.on_update(fps_lab);

    auto& task_chart = this->make_widget<UI::BarChart>(20, 20, this->width - 40, 20);
    task_chart.on_update = ([this](UI::Widget& w) {
        auto& chart = static_cast<UI::BarChart&>(w);
        std::vector<UI::ChartData> data;
        const auto& tasks = this->profiler.get_tasks();
        std::transform(tasks.cbegin(), tasks.cend(), std::back_inserter(data), [](const auto& e) {
            return UI::ChartData(e->get_average_time_ms(), e->name, Eng3D::Color::get_random(e->color).get_value());
        });
        chart.set_data(data);
    });
    task_chart.on_update(task_chart);

    this->on_update = ([this](UI::Widget&) {
        auto current_tasks = this->profiler.get_tasks();
        auto& current_views = this->task_views;
        if(current_views.size() < current_tasks.size()) {
            for(size_t i = current_views.size(); i < current_tasks.size(); i++)
                current_views.push_back(new ProfilerTaskView(this, 10, 50 + i * 25));
        } else if(current_views.size() > current_tasks.size()) {
            for(size_t i = current_tasks.size(); i < current_views.size(); i++)
                current_views[i]->kill();
            current_views.erase(current_views.begin() + current_tasks.size(), current_views.end());
        }
        for(size_t i = 0; i < current_views.size(); i++)
            current_views[i]->set_task(*current_tasks[i]);
    });
}

Eng3D::Interface::ProfilerTaskView::ProfilerTaskView(ProfilerView* profiler_view, int _x, int _y):
    UI::Group(_x, _y, 300, 25, profiler_view)
{
    this->color_box = &this->make_widget<UI::Div>(0, 0, 20, 20);
    this->label = &this->make_widget<UI::Label>(30, 0, " ");
}

void Eng3D::Interface::ProfilerTaskView::set_task(Eng3D::BenchmarkTask& profiler_view) {
    this->color_box->background_color = Eng3D::Color::rgb32(profiler_view.color);
    float time = profiler_view.get_average_time_ms();
    auto format_time = std::to_string((int)time);
    format_time = std::string(3 - glm::min<size_t>(3, format_time.length()), '0') + format_time;
    this->label->set_text(format_time + " ms " + profiler_view.name);
}
