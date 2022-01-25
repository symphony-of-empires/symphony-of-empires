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
//      client/interface/profiler_view.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "unified_render/ui/window.hpp"
#include "unified_render/ui/group.hpp"
#include "client/game_state.hpp"
#include <vector>

namespace UI {
    class Label;
    class Div;
}

namespace UnifiedRender {
    class BenchmarkTask;
}

namespace Interface {
    class ProfilerTaskView;
    class ProfilerView: public UI::Window {
        GameState& gs;
        std::vector<ProfilerTaskView*> task_views;
    public:
        ProfilerView(GameState& gs);
    };

    class ProfilerTaskView: public UI::Group {
    public:
        ProfilerTaskView(ProfilerView* profiler_view, int x, int y);
        void set_task(UnifiedRender::BenchmarkTask* profiler_view);
    private:
        UI::Label* label;
        UI::Div* color_box;
    };
};