// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      font_sdf.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

#include <glm/vec3.hpp>

#include "eng3d/rectangle.hpp"

namespace Eng3D {
    namespace OpenGL {
        class Program;
    };
    class Texture;
    class Camera;
    class TriangleList;

    class Glyph {
    public:
        Glyph(float _advance, Rectangle _atlas_bounds, Rectangle _plane_bounds);
        
        float advance;
        Rectangle atlas_bounds;
        Rectangle plane_bounds;
    };

    class Label3D {
    public:
        Label3D(TriangleList* triangles, float size, glm::vec3 center);
        ~Label3D();
        void draw();

        float size;
        glm::vec3 center;
    private:
        TriangleList* triangles;
    };

    class FontSDF {
    public:
        FontSDF(const std::string& filename);
        Label3D* gen_text(const std::string& text, glm::vec3 top, glm::vec3 right, float width, glm::vec3 center);
        void draw(const std::vector<Label3D*>& labels, Camera* camera, bool sphere);
    private:
        std::unordered_map<uint32_t, Glyph> unicode_map;
        std::shared_ptr<Eng3D::Texture> atlas;
        std::unique_ptr<Eng3D::OpenGL::Program> sphere_shader;
        std::unique_ptr<Eng3D::OpenGL::Program> flat_shader;
    };
};
