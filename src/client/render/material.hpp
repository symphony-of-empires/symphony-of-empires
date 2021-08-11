#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "texture.hpp"
#include <string>
#include <set>

#include <glm/vec3.hpp>

namespace UnifiedRender {
    /**
     * A definition for a surface/color/texture to be applied to a model
     */
    class Material {
        const Texture *texture;
        glm::vec3 colour;
    };

    class MaterialManager {
    private:
        std::set<std::pair<Material*, std::string>> materials;
    public:
        const Material& load_material(std::string path);
    };
}
extern UnifiedRender::MaterialManager* g_material_manager;

#endif