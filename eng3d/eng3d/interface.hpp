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
//      interface.hpp
//
// Abstract:
//      Provides common UI elements from Eng3D to debug/profile or do other
//      things in-game.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include "eng3d/ui/window.hpp"
#include "eng3d/ui/group.hpp"

namespace UI {
    class Label;
    struct Div;
}

namespace Eng3D {
    class State;
    class BenchmarkTask;
    struct Profiler;
}

namespace Eng3D::Interface {
    class ProfilerTaskView;
    class ProfilerView : public UI::Window {
        Eng3D::State& s;
        Eng3D::Profiler& profiler;
        std::vector<ProfilerTaskView*> task_views;
    public:
        ProfilerView(Eng3D::State& s, Eng3D::Profiler& profiler);
    };

    class ProfilerTaskView : public UI::Group {
        UI::Label* label;
        UI::Div* color_box;
    public:
        ProfilerTaskView(ProfilerView* profiler_view, int x, int y);
        void set_task(Eng3D::BenchmarkTask& profiler_view);
    };
}
