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
//      profiler.hpp
//
// Abstract:
//      Allows profiling of various systems of the game.
// ----------------------------------------------------------------------------

#include <algorithm>
#include "eng3d/profiler.hpp"
#include "eng3d/log.hpp"

using namespace Eng3D;

// Keep data for 10 seconds
static const float MS_TO_KEEP_DATA = 10 * 1e3;
static const uint32_t colors[] = {
    0xFF000000, 0xFF00FF00, 0xFF0000FF, 0xFFFF0000, 0xFF01FFFE, 0xFFFFA6FE,
    0xFFFFDB66, 0xFF006401, 0xFF010067, 0xFF95003A, 0xFF007DB5, 0xFFFF00F6,
    0xFFFFEEE8, 0xFF774D00, 0xFF90FB92, 0xFF0076FF, 0xFFD5FF00, 0xFFFF937E,
    0xFF6A826C, 0xFFFF029D, 0xFFFE8900, 0xFF7A4782, 0xFF7E2DD2, 0xFF85A900,
    0xFFFF0056, 0xFFA42400, 0xFF00AE7E, 0xFF683D3B, 0xFFBDC6FF, 0xFF263400,
    0xFFBDD393, 0xFF00B917, 0xFF9E008E, 0xFF001544, 0xFFC28C9F, 0xFFFF74A3,
    0xFF01D0FF, 0xFF004754, 0xFFE56FFE, 0xFF788231, 0xFF0E4CA1, 0xFF91D0CB,
    0xFFBE9970, 0xFF968AE8, 0xFFBB8800, 0xFF43002C, 0xFFDEFF74, 0xFF00FFC6,
    0xFFFFE502, 0xFF620E00, 0xFF008F9C, 0xFF98FF52, 0xFF7544B1, 0xFFB500FF,
    0xFF00FF78, 0xFFFF6E41, 0xFF005F39, 0xFF6B6882, 0xFF5FAD4E, 0xFFA75740,
    0xFFA5FFD2, 0xFFFFB167, 0xFF009BFF, 0xFFE85EBE
};

void Eng3D::BenchmarkTask::start() {
    start_time = std::chrono::system_clock::now();
    running = true;
}

void Eng3D::BenchmarkTask::stop() {
    if(!running) {
        Eng3D::Log::error("benchmark", "Tried to stop task '" + name + "', but it hasn't been started yet");
        return;
    }
    running = false;
    auto now = std::chrono::system_clock::now();
    float time = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
    times.push_back(time);
    start_times.push_back(start_time);
}

float Eng3D::BenchmarkTask::get_average_time_ms() {
    this->clear_old();
    float total_time = 0;
    for(auto const& time : times)
        total_time += time;
    return total_time / (MS_TO_KEEP_DATA / 1e3);
}

float Eng3D::BenchmarkTask::get_largest_time_ms() {
    this->clear_old();
    float max_time = 0;
    for(const auto& time : times)
        max_time = std::max<float>(max_time, time);
    return max_time;
}

void Eng3D::BenchmarkTask::clear_old() {
    // Clear old times
    auto now = std::chrono::system_clock::now();
    auto times_it = times.begin();
    auto start_times_it = start_times.begin();
    while(start_times_it != start_times.end()) {
        auto prev_start_time = *start_times_it;
        float time = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev_start_time).count();
        if(time > MS_TO_KEEP_DATA) {
            start_times_it = start_times.erase(start_times_it);
            times_it = times.erase(times_it);
        } else {
            start_times_it++;
            times_it++;
        }
    }
}

void Eng3D::Profiler::start(const std::string& name) {
    auto it = tasks.find(name);
    if(it == tasks.end()) {
        size_t amounts = tasks.size();
        uint32_t color = colors[amounts % 64];
        Eng3D::BenchmarkTask task(name, color);
        task.start();
        tasks.insert({ name, task });
    } else {
        it->second.start();
    }
}

void Eng3D::Profiler::stop(const std::string& name) {
    auto it = tasks.find(name);
    if(it == tasks.end()) {
        Eng3D::Log::error("profiler", "Tried to stop task '" + name + "', but it hasn't been started yet");
    } else {
        it->second.stop();
    }
}

void Eng3D::Profiler::tick_done() {

}

float Eng3D::Profiler::get_fps() {
    return fps;
}

void Eng3D::Profiler::render_done() {
    auto now = std::chrono::system_clock::now();
    if(!render_started) {
        fps_clock = now;
        render_started = true;
    }
    fps_timer += std::chrono::duration_cast<std::chrono::milliseconds>(now - fps_clock).count();
    fps_clock = now;
    frames++;
    if(fps_timer > 1000.0) { // Every second
        fps = (float)frames * 0.5 + fps * 0.5;  // More stable
        frames = 0;
        fps_timer -= 1000.;
    }
}

const std::vector<Eng3D::BenchmarkTask*> Eng3D::Profiler::get_tasks() {
    std::vector<Eng3D::BenchmarkTask*> list(tasks.size());
    std::transform(tasks.begin(), tasks.end(), list.begin(), [](auto& e) {
        return &e.second;
    });
    return list;
}
