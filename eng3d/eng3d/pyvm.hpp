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
//      pyvm.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string_view>
#include <string>
#include <vector>

namespace Eng3D {
    class PythonException : public std::exception {
        std::string buffer;
    public:
        PythonException(const std::string_view message) {
            buffer = message;
        }
        virtual const char* what() const noexcept {
            return buffer.data();
        }
    };

    class PythonObj {
        void* obj = nullptr;
    public:
        PythonObj() = default;
        PythonObj(void* _obj);
        PythonObj(const PythonObj&);
        PythonObj(PythonObj&&) noexcept;
        ~PythonObj();
    };

    class State;
    class PythonVM {
        State& s;
    public:
        PythonVM(Eng3D::State& _s);
        ~PythonVM();

        void run_string(const std::string_view name, const std::string_view path);
        void add_module(const std::string_view path);

        std::vector<PythonObj> modules;
    };
}
