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
//      eng3d/ui/candle.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include "eng3d/ui/widget.hpp"
#include "eng3d/rectangle.hpp"

namespace UI {
    class Context;

    struct CandleData {
        CandleData() = default;
        CandleData(float _open, float _close, float _min, float _max)
            : open{ _open },
            close{ _close },
            min{ _min },
            max{ _max }
        {
            assert(open >= min && open <= max);
            assert(close >= min && close <= max);
            this->max = glm::max(this->max, glm::max(this->open, this->close));
            this->min = glm::min(this->min, glm::min(this->open, this->close));
        }
        ~CandleData() = default;

        float open = 0.f;
        float close = 0.f;
        float min = 0.f;
        float max = 0.f;
    };

    /// @ingroup UI
    /// @brief A graph chart
    class CandleChart: public Widget {
    public:
        CandleChart(int x, int y, unsigned w, unsigned h, Widget* _parent = nullptr);
        virtual ~CandleChart() override {}
        virtual void on_render(Context& ctx, Eng3D::Rect viewport);
        void set_data(std::vector<UI::CandleData> data);
    private:
        std::vector<UI::CandleData> data;
        float max = 1.f, min = 1.f;
    };
}
