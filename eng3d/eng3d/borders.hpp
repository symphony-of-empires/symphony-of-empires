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
//      borders.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <vector>
#include <unordered_set>
#include <set>
#include "eng3d/curve.hpp"

namespace Eng3D {
    class Texture;
    class Camera;
    namespace OpenGL {
        class Program;
    }
    class State;

    class Borders {
        Eng3D::State& s;
        std::vector<std::unique_ptr<Eng3D::Curve>> curves;
        std::unique_ptr<Eng3D::OpenGL::Program> line_shader;
        std::shared_ptr<Eng3D::Texture> water_tex;
    public:
        Borders(Eng3D::State& s, bool lazy_init = true);
        ~Borders() = default;
        void draw(const Eng3D::Camera& camera);
    private:
        void build_borders();
    };

    // class BorderGenerator
    // {
    // public:
    //     std::set<int> walked_positions;
    //     std::vector<std::vector<glm::vec3>>& borders;
    //     uint32_t* pixels;
    //     int width;
    //     int height;

    //     bool check_neighbor(int new_x, int new_y);

    //     void add_neighbor(int new_x, int new_y, int& connections);

    //     void get_border(int x, int y, int connections);

    //     BorderGenerator(std::vector<std::vector<glm::vec3>>& borders, uint32_t* pixels, int width, int height)
    //         : borders{ borders }, pixels{ pixels }, width{ width }, height{ height }
    //     {
    //         walked_positions = std::set<int>();
    //     };
    //     void build_borders();
    // };
}