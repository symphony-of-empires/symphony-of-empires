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
//      client/borders.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <vector>
#include <unordered_set>
#include <set>

namespace UnifiedRender {
    class Curve;
    class Texture;
    namespace OpenGL {
        class Program;
    }
}
class Camera;

class Borders
{
public:
    Borders();
    ~Borders();

    void draw(Camera* camera);
private:
    void build_borders();
    std::vector<UnifiedRender::Curve*> curves;
    std::unique_ptr<UnifiedRender::OpenGL::Program> line_shader;
    std::shared_ptr<UnifiedRender::Texture> water_tex;
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