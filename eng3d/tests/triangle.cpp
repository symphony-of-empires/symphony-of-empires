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
//      triangle.hpp
//
// Abstract:
//      Small program to test that rendering works.
// ----------------------------------------------------------------------------

#include <iostream>

#include "eng3d/state.hpp"
#include "eng3d/string.hpp"
#include "eng3d/event.hpp"
#include "eng3d/profiler.hpp"
#include "eng3d/interface.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/components.hpp"

struct GameState: public Eng3D::State {
    GameState(const std::vector<std::string>& pkg_paths)
        : Eng3D::State::State(pkg_paths)
    {

    }
    ~GameState() = default;

    Eng3D::Interface::ProfilerView* profiler_view = nullptr;
    Eng3D::Profiler profiler;

    void handle_key(const Eng3D::Event::Key& e) override {
        if(e.hold) {
            switch(e.type) {
            case Eng3D::Event::Key::Type::ESC:
                this->run = !this->run;
                break;
            case Eng3D::Event::Key::Type::F2:
                if(this->profiler_view) {
                    this->profiler_view->kill();
                    profiler_view = nullptr;
                }
                else this->profiler_view = new Eng3D::Interface::ProfilerView(*this, this->profiler);
                break;
            default:
                break;
            }
        }
    }
};

int main(int argc, char** argv) {
    std::vector<std::string> pkg_paths;
    for(int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if(arg == "--mod") {
            i++;
            if(i >= argc)
                CXX_THROW(std::runtime_error, translate("Expected an absolute path after --mod"));
            arg = std::string(argv[i]);
            pkg_paths.push_back(arg);
        }
    }

    GameState gs(pkg_paths);
    gs.run = true;

    new UI::Image(0, 0, gs.width, gs.height, "gfx/sky.png", nullptr);

    auto* red_sky = new UI::Image(0, 0, 128, 128, "gfx/sky.png", nullptr);
    red_sky->background_color = Eng3D::Color(1.f, 0.f, 0.f, 1.f);
    red_sky->set_tooltip("The red sky in uh.. hmm... ehm...");
    auto* green_sky = new UI::Image(128, 0, 128, 128, "gfx/sky.png", nullptr);
    green_sky->background_color = Eng3D::Color(0.f, 1.f, 0.f, 1.f);
    green_sky->set_tooltip("The green sky shines in, tuesday?");
    auto* blue_sky = new UI::Image(256, 0, 128, 128, "gfx/sky.png", nullptr);
    blue_sky->background_color = Eng3D::Color(0.f, 0.f, 1.f, 1.f);
    blue_sky->set_tooltip("The blue sky doesn't really exist");

    auto* info_win = new UI::Window(32, 32, 512, 256 + 96);
    info_win->set_text("Economics");
    auto& chart = info_win->make_widget<UI::Chart>(0, 0, 256, 128);
    chart.set_text("Economy");
    chart.set_data({ 1.f, 2.5f, 5.f, 4.f });

    auto& candlechart = info_win->make_widget<UI::CandleChart>(0, 150, 256, 128);
    candlechart.set_text("soil composition (very soil)");

    std::vector<UI::CandleData> candles;
    auto prev_close = 0.f;
    for(size_t i = 0; i < 10; i++)
    {
        UI::CandleData candle{};
        candle.open = prev_close;
        candle.close = candle.open + i;
        candle.max = glm::max(candle.open, candle.close) + i;
        candle.min = glm::min(candle.open, candle.close) - i;

        prev_close = candle.close;
        candles.push_back(candle);
    }
    candlechart.set_data(candles);

    auto& piechart = info_win->make_widget<UI::PieChart>(256, 0, 128, 128);
    piechart.set_data({ UI::ChartData(0.5f, "Eng3D", 0xff00ffff), UI::ChartData(0.2f, "Not Eng3D", 0x00ff00ff) });

    gs.ui_ctx.prompt("شست بلسجليدلطظﻻىرﻻزؤرظءؤزةوئىءؤئمحيبسخ4ه2صثهقعسيبىسب", "σξδφσေိျုူိက့်ိုျ့ငသ်ဆသစနငငသ့ိထာလဘာခလုူ γνακδ ασνし結十と岡統百防能οχψωηδ ασηξφ ξογφφσηγερς");
    gs.ui_ctx.prompt("Hello world", "This is a demo of Eng3D ^_^");

    auto* parliament_win = new UI::Window(320, 32, 256 + 64, 256 + 64);
    parliament_win->set_text("parliamento");
    auto& waffle_chart = parliament_win->make_widget<UI::WaffleChart>(0, 0, 256, 256);
    waffle_chart.set_text("Seat row candidates?");
    waffle_chart.set_data({
        UI::ChartData(5.234f, "Waffle", 0xff00ffff),
        UI::ChartData(2.567f, "Tundra", 0x00ff00ff),
        UI::ChartData(3.23423f, "Fjord", 0xff5f70ff)
    });

    gs.do_run([&gs](){ return gs.run == true; },
        ([&gs]() {
            gs.profiler.start("UI Event handling");
            gs.do_event();
            gs.profiler.stop("UI Event handling");
        }), ([&gs]() {
            gs.profiler.start("Clearing");
            gs.clear();
            gs.profiler.stop("Clearing");
            gs.profiler.render_done();
        })
    );
    std::cout << "Test passed" << std::endl;
    return 0;
}
