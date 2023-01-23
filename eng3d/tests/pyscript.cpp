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
//      pyscript.hpp
//
// Abstract:
//      Play around with the Python3 scripting.
// ----------------------------------------------------------------------------

#include <iostream>

#include "eng3d/state.hpp"
#include "eng3d/string.hpp"
#include "eng3d/event.hpp"
#include "eng3d/profiler.hpp"
#include "eng3d/interface.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/components.hpp"
#include "eng3d/pyvm.hpp"

struct GameState : public Eng3D::State {
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
                } else this->profiler_view = new Eng3D::Interface::ProfilerView(*this, this->profiler);
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

#ifdef E3D_FEATURE_PYTHON
    Eng3D::PythonVM pyvm(gs);
    pyvm.run_string("main", gs.package_man.get_unique("python/main.py")->read_all());
#endif

    gs.do_run([&gs]() { return gs.run == true; },
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
