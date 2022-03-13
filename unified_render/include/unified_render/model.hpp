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
//      model.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include <map>
#include <cstdint>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "unified_render/mesh.hpp"

namespace UnifiedRender {
    class Material;
    namespace OpenGL {
        class Program;
    };
};

namespace UnifiedRender {
    // A simple object - use these to store "simple" objects that MAY repeat
    // TODO: We should use instancing tricks on simple objects
    class SimpleModel : public UnifiedRender::Mesh<glm::vec3, glm::vec2> {
    public:
        SimpleModel(enum UnifiedRender::MeshMode _mode);
        ~SimpleModel();
        SimpleModel(const SimpleModel&) = default;
        SimpleModel(SimpleModel&&) noexcept = default;
        SimpleModel& operator=(const SimpleModel&) = default;
        virtual void draw(const UnifiedRender::OpenGL::Program& shader) const;

        const Material* material = nullptr;
    };

    // A complex object being composed by many simple objects
    class Model {
    public:
        Model();
        ~Model();
        Model(const Model&) = default;
        Model(Model&&) noexcept = default;
        Model& operator=(const Model&) = default;
        virtual void draw(const UnifiedRender::OpenGL::Program& shader) const;

        std::vector<const UnifiedRender::SimpleModel*> simple_models;
    };

    class ModelManager {
        std::map<std::string, UnifiedRender::Model*> models;
        const UnifiedRender::Model& load_wavefront(const std::string& path);
        const UnifiedRender::Model& load_stl(const std::string& path);
    public:
        ~ModelManager();
        const UnifiedRender::Model& load(const std::string& path);
    };
}