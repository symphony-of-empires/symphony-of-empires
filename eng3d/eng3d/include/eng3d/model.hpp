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
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "eng3d/mesh.hpp"
#include "eng3d/io.hpp"

namespace Eng3D {
    class Material;
    namespace OpenGL {
        class Program;
    };
};

namespace Eng3D {
    // A simple object - use these to store "simple" objects that MAY repeat
    /// @todo We should use instancing tricks on simple objects
    class SimpleModel : public Eng3D::Mesh<glm::vec3, glm::vec2> {
    public:
        SimpleModel(enum Eng3D::MeshMode _mode);
        ~SimpleModel() {};
        SimpleModel(const SimpleModel&) = default;
        SimpleModel(SimpleModel&&) noexcept = default;
        SimpleModel& operator=(const SimpleModel&) = default;
        virtual void draw(const Eng3D::OpenGL::Program& shader) const;

        const Material* material = nullptr;
    };

    // A complex object being composed by many simple objects
    class Model {
    public:
        Model() {};
        virtual ~Model() {};
        Model(const Model&) = default;
        Model(Model&&) noexcept = default;
        Model& operator=(const Model&) = default;
        virtual void draw(const Eng3D::OpenGL::Program& shader) const {
            for(auto& model : simple_models)
                model.draw(shader);
        }

        Eng3D::SimpleModel process_simple_model(aiMesh& mesh, const aiScene& scene);
        void process_node(aiNode& node, const aiScene& scene);

        std::vector<Eng3D::SimpleModel> simple_models;
    };

    class ModelManager {
        std::map<std::string, std::shared_ptr<Eng3D::Model>> models;
    public:
        ~ModelManager() {};
        std::shared_ptr<Eng3D::Model> load(const std::string& path);
        std::shared_ptr<Eng3D::Model> load(std::shared_ptr<Eng3D::IO::Asset::Base> asset);
    };
}