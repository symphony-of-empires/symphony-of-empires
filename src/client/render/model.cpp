#include "model.hpp"
#include <algorithm>

const UnifiedRender::SimpleModel& UnifiedRender::ModelManager::load_simple(std::string path) {
    return *((const UnifiedRender::SimpleModel *)NULL);
}

const UnifiedRender::ComplexModel& UnifiedRender::ModelManager::load_complex(std::string path) {
    return *((const UnifiedRender::ComplexModel *)NULL);
}

UnifiedRender::ModelManager* g_model_manager;