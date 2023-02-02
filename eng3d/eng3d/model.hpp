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

#include "eng3d/mesh.hpp"
#include "eng3d/io.hpp"

namespace Eng3D {
    class State;
    struct Material;
    namespace OpenGL {
        class Program;
    }
}

class aiMesh;
class aiScene;
class aiNode;

namespace Eng3D {
    /// @brief A simple object - use these to store "simple" objects that MAY repeat
    /// @todo We should use instancing tricks on simple objects
    struct SimpleModel : Eng3D::Mesh<glm::vec3, glm::vec2> {
        SimpleModel(enum Eng3D::MeshMode _mode) : Eng3D::Mesh<glm::vec3, glm::vec2>(_mode) {};
        ~SimpleModel() = default;
        SimpleModel(const SimpleModel&) = delete;
        SimpleModel(SimpleModel&&) noexcept = default;
        SimpleModel& operator=(const SimpleModel&) = delete;
        virtual void draw(const Eng3D::OpenGL::Program& shader, int instances = 0) const;
        
        std::shared_ptr<Eng3D::Material> material;
    };

    /// @brief A complex object being composed by many simple objects
    struct Model {
        Model() = default;
        virtual ~Model() = default;
        Model(const Model&) = delete;
        Model(Model&&) noexcept = default;
        Model& operator=(const Model&) = delete;
        
        virtual void draw(const Eng3D::OpenGL::Program& shader, int instances = 0) const {
            for(auto& model : simple_models)
                model.draw(shader, instances);
        }
        
#ifdef E3D_FEATURE_ASSIMP
        Eng3D::SimpleModel process_simple_model(aiMesh& mesh, const aiScene& scene);
        void process_node(aiNode& node, const aiScene& scene);
#endif
        std::vector<Eng3D::SimpleModel> simple_models;
    };

    class ModelManager {
        std::map<std::string, std::shared_ptr<Eng3D::Model>> models;
        Eng3D::State& s;
    public:
        ModelManager() = delete;
        ModelManager(Eng3D::State& _s)
            : s{ _s }
        {

        }
        ~ModelManager() = default;
        std::shared_ptr<Eng3D::Model> load(const std::string_view path);
        std::shared_ptr<Eng3D::Model> load(std::shared_ptr<Eng3D::IO::Asset::Base> asset);
    };
}
