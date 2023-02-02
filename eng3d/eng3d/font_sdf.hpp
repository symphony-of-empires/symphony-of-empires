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
    }
    class Texture;
    class Camera;
    struct TriangleList;

    struct Glyph {
        Glyph() = default;
        Glyph(float _advance, Eng3D::Rectangle _atlas_bounds, Eng3D::Rectangle _plane_bounds);
        ~Glyph() = default;
        
        float advance;
        Eng3D::Rectangle atlas_bounds;
        Eng3D::Rectangle plane_bounds;
    };

    struct Label3D {
        Label3D(Eng3D::TriangleList* triangles, float size, glm::vec3 center);
        ~Label3D() = default;
        void draw();
        float size;
        glm::vec3 center;
    private:
        std::unique_ptr<Eng3D::TriangleList> triangles;
    };

    struct FontSDF {
        FontSDF(const std::string_view filename);
        std::unique_ptr<Eng3D::Label3D> gen_text(const std::string_view text, glm::vec2 pmin, glm::vec2 pmax, glm::vec2 p0, float width);
        void draw(const std::vector<std::unique_ptr<Label3D>>& labels, const Eng3D::Camera& camera, bool sphere);
    private:
        std::unordered_map<uint32_t, Glyph> unicode_map;
        std::shared_ptr<Eng3D::Texture> atlas;
        std::unique_ptr<Eng3D::OpenGL::Program> sphere_shader;
        std::unique_ptr<Eng3D::OpenGL::Program> flat_shader;
    };
}
