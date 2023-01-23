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
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <unordered_map>
#include <string>
#include <ctime>
#include <chrono>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <list>

namespace Eng3D {
    class BenchmarkTask {
        void clear_old();
    public:
        BenchmarkTask(const std::string& task, uint32_t argb)
            : name{ task },
            color{ argb }
        {
            running = false;
        }
        BenchmarkTask& operator=(const BenchmarkTask& lhs) = delete;
        void start();
        void stop();
        float get_average_time_ms();
        float get_largest_time_ms();

        const std::string name;
        const uint32_t color;
    private:
        std::list<float> times;
        std::list<std::chrono::system_clock::time_point> start_times;
        std::chrono::system_clock::time_point start_time;
        bool running;
    };

    struct Profiler {
        Profiler() {
            render_started = false;
        }
        ~Profiler() = default;
        void start(const std::string& name);
        void stop(const std::string& name);
        void tick_done();
        void render_done();
        float get_fps();
        const std::vector<BenchmarkTask*> get_tasks();
    private:
        std::mutex lock;
        size_t tick;
        float fps;
        std::chrono::system_clock::time_point fps_clock;
        float fps_timer = 0;
        size_t frames = 0;
        std::atomic<bool> render_started;
        std::unordered_map<std::string, BenchmarkTask> tasks;
    };
}
