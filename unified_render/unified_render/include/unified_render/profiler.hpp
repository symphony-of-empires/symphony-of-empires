// Unified Render - General purpouse game engine
// Copyright (C) 2021, Unified Render contributors
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
#include <list>

namespace UnifiedRender {
    class BenchmarkTask {
    public:
        BenchmarkTask(std::string task, uint32_t argb): name{ task }, color{ argb } {};
        const std::string name;
        const uint32_t color;

        void start();
        void stop();
        float get_average_time_ms();
        float get_largest_time_ms();
    private:
        void clear_old();

        std::list<float> times;
        std::list<std::chrono::system_clock::time_point> start_times;
        std::chrono::system_clock::time_point start_time;
        bool running = false;
    };

    class Profiler {
    public:
        Profiler() {};
        ~Profiler() {};

        void start(std::string name);
        void stop(std::string name);
        void tick_done();
        void render_done();

        float get_fps();
        std::vector<BenchmarkTask*> get_tasks();

    private:
        size_t tick;
        std::unordered_map<std::string, BenchmarkTask> tasks;
        float fps;
        std::chrono::system_clock::time_point fps_clock;
        float fps_timer = 0;
        size_t frames = 0;
        bool render_started = false;
    };
};