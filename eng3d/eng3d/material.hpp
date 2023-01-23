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
//      material.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Eng3D {
    class State;
    class Texture;
    
    /// A definition for a surface/color/texture to be applied to a model
    struct Material {
        Material() = default;
        ~Material() = default;
        Material(const Material&) = default;
        Material(Material&&) noexcept = default;
        Material& operator=(const Material&) = default;

        glm::vec3 color;
        float specular_exp;
        float optical_density;
        glm::vec4 diffuse_color = glm::vec4(1.f);
        std::shared_ptr<Eng3D::Texture> diffuse_map;
        glm::vec4 ambient_color = glm::vec4(1.f);
        std::shared_ptr<Eng3D::Texture> ambient_map;
        glm::vec4 specular_color = glm::vec4(1.f);
        std::shared_ptr<Eng3D::Texture> specular_map;
        std::shared_ptr<Eng3D::Texture> displacement_map;
        std::shared_ptr<Eng3D::Texture> occlussion_map;
        std::shared_ptr<Eng3D::Texture> luminance_map;
        std::shared_ptr<Eng3D::Texture> height_map;
        std::shared_ptr<Eng3D::Texture> normal_map;
        std::shared_ptr<Eng3D::Texture> roughness_map;
    };

    class MaterialManager {
        std::map<std::string, std::shared_ptr<Eng3D::Material>> materials;
        Eng3D::State& s;
    public:
        MaterialManager() = delete;
        MaterialManager(Eng3D::State& _s)
            : s{ _s }
        {

        }
        ~MaterialManager() = default;
        const std::shared_ptr<Eng3D::Material> load(const std::string& name);
    };
}