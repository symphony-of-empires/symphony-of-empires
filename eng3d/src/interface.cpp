// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
#include "eng3d/locale.hpp"

Eng3D::Interface::ProfilerView::ProfilerView(Eng3D::State& _s, Eng3D::Profiler& _profiler)
    : UI::Window(0, 0, 240, _s.width, nullptr),
    s{ _s },
    profiler{ _profiler }
{
    this->padding.x = 0;
    this->padding.y = 48;
    this->text(translate("Performance profiler"));
    this->is_scroll = false;
    this->current_texture.reset();

    this->set_close_btn_function([this](UI::Widget&) {
        this->kill();
    });

    float fps = this->profiler.get_fps();
    auto* fps_lab = new UI::Label(10, 0, "FPS: " + std::to_string((int)fps), this);
    fps_lab->on_update = ([this](UI::Widget& w) {
        float fps = this->profiler.get_fps();
        w.text("FPS: " + std::to_string((int)fps));
    });

    std::vector<UI::ChartData> data{};
    auto tasks = this->profiler.get_tasks();
    for(auto& task : tasks)
        data.push_back(UI::ChartData(task->get_average_time_ms(), task->name, 0));

    auto* task_chart = new UI::BarChart(20, 20, this->width - 40, 20, this);
    task_chart->on_update = ([this](UI::Widget& w) {
        auto& chart = static_cast<UI::BarChart&>(w);
        std::vector<UI::ChartData> data;
        auto tasks = this->profiler.get_tasks();
        for(auto& task : tasks) {
            float time = task->get_average_time_ms();
            data.push_back(UI::ChartData(time, task->name, task->color));
        }
        chart.set_data(data);
    });

    this->on_update = ([this](UI::Widget&) {
        auto tasks = this->profiler.get_tasks();
        auto& task_views = this->task_views;
        if(task_views.size() < tasks.size()) {
            for(size_t i = task_views.size(); i < tasks.size(); i++)
                task_views.push_back(new ProfilerTaskView(this, 10, 50 + i * 25));
        } else if(task_views.size() > tasks.size()) {
            for(size_t i = tasks.size(); i < task_views.size(); i++)
                task_views[i]->kill();
            task_views.erase(task_views.begin() + tasks.size(), task_views.end());
        }
        for(size_t i = 0; i < task_views.size(); i++)
            task_views[i]->set_task(*tasks[i]);
    });
}

Eng3D::Interface::ProfilerTaskView::ProfilerTaskView(ProfilerView* profiler_view, int x, int y):
    UI::Group(x, y, 300, 25, profiler_view)
{
    this->color_box = new UI::Div(0, 0, 20, 20, this);
    this->label = new UI::Label(30, 0, " ", this);
}

void Eng3D::Interface::ProfilerTaskView::set_task(Eng3D::BenchmarkTask& profiler_view) {
    this->color_box->background_color = Eng3D::Color::rgba32(profiler_view.color);
    float time = profiler_view.get_average_time_ms();
    auto format_time = std::to_string((int)time);
    format_time = std::string(3 - glm::min<size_t>(3, format_time.length()), '0') + format_time;
    this->label->text(format_time + " ms " + profiler_view.name);
}
