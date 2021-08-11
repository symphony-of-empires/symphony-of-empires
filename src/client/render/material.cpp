#include "material.hpp"
#include <algorithm>

const UnifiedRender::Material& UnifiedRender::MaterialManager::load_material(std::string path) {
    return *((const Material *)NULL);
}

UnifiedRender::MaterialManager* g_material_manager;