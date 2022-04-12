// Unified Render - General purpouse game engine
// Copyright (C) 2021, Unified Render contributors
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

#include "unified_render/rectangle.hpp"

namespace UnifiedRender {
    namespace OpenGL {
        class Program;
    };
    class Texture;
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
        Label3D(TriangleList* triangles, float size);
        ~Label3D();

        void draw();
        float size;
        glm::mat4 model;
    private:
        TriangleList* triangles;
    };

    class FontSDF {
    public:
        FontSDF(const std::string& filename);
        Label3D* gen_text(const std::string& text, glm::vec3 top, glm::vec3 right, float width, float curve = 0.f);
        void draw(const std::vector<Label3D*>& labels, glm::mat4 projection, glm::mat4 view);
    private:
        std::unordered_map<uint32_t, Glyph> unicode_map;
        std::shared_ptr<UnifiedRender::Texture> atlas;
        std::unique_ptr<UnifiedRender::OpenGL::Program> sdf_font_shader;
    };

};
