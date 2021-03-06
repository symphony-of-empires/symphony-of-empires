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
//      client/rivers.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <vector>

namespace Eng3D {
    class Curve;
    class Texture;
    class Camera;
    namespace OpenGL {
        class Program;
    }
}

class Rivers
{
public:
    Rivers();
    ~Rivers();

    void draw(Eng3D::Camera* camera);
private:
    void build_rivers();
    std::vector<Eng3D::Curve*> curves;
    std::unique_ptr<Eng3D::OpenGL::Program> line_shader;
    std::shared_ptr<Eng3D::Texture> water_tex;
};
