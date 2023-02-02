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
//      profiler.hpp
//
// Abstract:
//      Allows profiling of various systems of the game.
// ----------------------------------------------------------------------------

#include <cassert>
#include <algorithm>
#include <glm/glm.hpp>
#include "eng3d/profiler.hpp"
#include "eng3d/log.hpp"

using namespace Eng3D;

// Keep data for 10 seconds
static const float MS_TO_KEEP_DATA = 10 * 1e3;

void Eng3D::BenchmarkTask::start() {
    start_time = std::chrono::system_clock::now();
    running = true;
}

void Eng3D::BenchmarkTask::stop() {
    assert(running && "Can't stop task which hasn't started yet");
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
    if(times.empty()) return 0.f;
    return *std::max_element(times.cbegin(), times.cend());
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

void Eng3D::Profiler::start(const std::string_view name) {
    auto it = tasks.find(name.data());
    if(it == tasks.end()) {
        Eng3D::BenchmarkTask task(name, tasks.size());
        task.start();
        tasks.insert({ std::string(name), task });
    } else {
        it->second.start();
    }
}

void Eng3D::Profiler::stop(const std::string_view name) {
    auto it = tasks.find(name.data());
    assert(it != tasks.end() && "Tried to stop task that hasn't started yet");
    it->second.stop();
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
