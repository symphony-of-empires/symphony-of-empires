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
//      material.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <map>
#include <vector>

#include <glm/vec3.hpp>

namespace UnifiedRender {
    class Texture;
    
    /// A definition for a surface/color/texture to be applied to a model
    class Material {
    public:
        Material();
        ~Material();

        glm::vec3 diffuse_color;
        std::shared_ptr<UnifiedRender::Texture> diffuse_map;
        glm::vec3 ambient_color;
        std::shared_ptr<UnifiedRender::Texture> ambient_map;
        glm::vec3 specular_color;
        std::shared_ptr<UnifiedRender::Texture> specular_map;
        std::shared_ptr<UnifiedRender::Texture> displacement_map;
        std::shared_ptr<UnifiedRender::Texture> occlussion_map;
        std::shared_ptr<UnifiedRender::Texture> luminance_map;
        std::shared_ptr<UnifiedRender::Texture> height_map;
        std::shared_ptr<UnifiedRender::Texture> normal_map;
        std::shared_ptr<UnifiedRender::Texture> roughness_map;

        glm::vec3 color;
        float specular_exp;
        float optical_density;
    };

    class MaterialManager {
    private:
        std::map<std::string, Material*> materials;
    public:
        MaterialManager();
        ~MaterialManager();
        std::vector<std::pair<Material*, std::string>> load_wavefront(const std::string& path, const std::string& model_name);
        const Material* load(const std::string& path);
    };
}