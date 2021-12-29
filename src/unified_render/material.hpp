#pragma once

#include <string>
#include <set>
#include <vector>

#include <glm/vec3.hpp>

namespace UnifiedRender {
    class Texture;
    
    /// A definition for a surface/color/texture to be applied to a model
    class Material {
    public:
        glm::vec3 diffuse_color;
        const Texture* diffuse_map = nullptr;
        glm::vec3 ambient_color;
        const Texture* ambient_map = nullptr;
        glm::vec3 specular_color;
        const Texture* specular_map = nullptr;
        const Texture* displacement_map = nullptr;
        const Texture* occlussion_map = nullptr;
        const Texture* luminance_map = nullptr;
        const Texture* height_map = nullptr;
        const Texture* normal_map = nullptr;
        const Texture* roughness_map = nullptr;

        glm::vec3 color;
        float specular_exp;
        float optical_density;
    };

    class MaterialManager {
    private:
        std::set<std::pair<Material*, std::string>> materials;
    public:
        std::vector<std::pair<Material*, std::string>> load_wavefront(const std::string& path);
        const Material& load_material(const std::string& path);
    };
}