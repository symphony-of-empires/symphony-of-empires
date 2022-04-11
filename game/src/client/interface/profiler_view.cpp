// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      client/interface/profiler_view.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/profiler.hpp"
#include "unified_render/color.hpp"
#include "unified_render/ui/components.hpp"
#include "unified_render/ui/widget.hpp"

#include "client/interface/profiler_view.hpp"
#include "client/game_state.hpp"
#include "world.hpp"

using namespace Interface;

ProfilerView::ProfilerView(GameState& _gs)
    : UI::Window(0, 0, 512, 400, nullptr),
    gs{ _gs }
{
    this->padding.x = 0;
    this->padding.y = 48;

    this->text("Profiler");
    this->is_scroll = false;

    UnifiedRender::Profiler& profiler = gs.world->profiler;
    float fps = profiler.get_fps();

    auto* fps_lab = new UI::Label(10, 0, "FPS: " + std::to_string((int)fps), this);
    fps_lab->on_update = ([this](UI::Widget& w) {
        UnifiedRender::Profiler& profiler = this->gs.world->profiler;
        float fps = profiler.get_fps();
        w.text("FPS: " + std::to_string((int)fps));
    });

    std::vector<UI::ChartData> data{};
    auto tasks = profiler.get_tasks();
    for(auto& task : tasks) {
        float time = task->get_average_time_ms();
        data.push_back(UI::ChartData(time, task->name, 0));
    }

    auto* task_chart = new UI::BarChart(20, 20, 200, 20, this);
    task_chart->on_update = ([this](UI::Widget& w) {
        auto& chart = static_cast<UI::BarChart&>(w);
        UnifiedRender::Profiler& profiler = this->gs.world->profiler;
        std::vector<UI::ChartData> data;
        auto tasks = profiler.get_tasks();
        for(auto& task : tasks) {
            float time = task->get_average_time_ms();
            data.push_back(UI::ChartData(time, task->name, task->color));
        }
        chart.set_data(data);
    });

    this->on_update = ([this](UI::Widget& w) {
        UnifiedRender::Profiler& profiler = this->gs.world->profiler;
        auto tasks = profiler.get_tasks();
        auto& task_views = this->task_views;

        if(task_views.size() < tasks.size()) {
            for(size_t i = task_views.size(); i < tasks.size(); i++) {
                task_views.push_back(new ProfilerTaskView(this, 10, 50 + i * 25));
            }
        } else if(task_views.size() > tasks.size()) {
            for(size_t i = tasks.size(); i < task_views.size(); i++) {
                task_views[i]->kill();
            }
            task_views.erase(task_views.begin() + tasks.size(), task_views.end());
        }
		
        for(size_t i = 0; i < task_views.size(); i++) {
            task_views[i]->set_task(tasks[i]);
        }
    });
}

ProfilerTaskView::ProfilerTaskView(ProfilerView* profiler_view, int x, int y):
    UI::Group(x, y, 300, 25, profiler_view)
{
    color_box = new UI::Div(0, 0, 20, 20, this);
    label = new UI::Label(30, 0, " ", this);
}

void ProfilerTaskView::set_task(UnifiedRender::BenchmarkTask* profiler_view) {
    color_box->color = UnifiedRender::Color::rgba32(profiler_view->color);
    float time = profiler_view->get_average_time_ms();
    std::string format_time = std::to_string((int)time);
    format_time = std::string(3 - std::min<size_t>(3, format_time.length()), '0') + format_time;
    label->text(format_time + " MS " + profiler_view->name);
}