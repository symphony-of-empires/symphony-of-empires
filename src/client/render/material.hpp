#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "texture.hpp"
#include <string>
#include <set>
#include <vector>

#include <glm/vec3.hpp>

namespace UnifiedRender {
    /**
     * A definition for a surface/color/texture to be applied to a model
     */
    class Material {
    public:
        const Texture* texture = nullptr;
        glm::vec3 colour;

        glm::vec3 ambient_colour, diffuse_colour, specular_colour;
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
extern UnifiedRender::MaterialManager* g_material_manager;

#endif