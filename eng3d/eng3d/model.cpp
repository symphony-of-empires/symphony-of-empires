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
//      model.cpp
//
// Abstract:
//      Defines some minor functions and defines big functions for parsing
//      polygonal data from files.
// ----------------------------------------------------------------------------

#include <algorithm>
#include <fstream>
#include <vector>
#include <iterator>

#ifdef E3D_FEATURE_ASSIMP
#   include <assimp/Importer.hpp>
#   include <assimp/scene.h>
extern "C" {
#   include <assimp/postprocess.h>
#   include <assimp/material.h>
}
#endif

#include "eng3d/model.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/material.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/state.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"

//
// Simple model
//
void Eng3D::SimpleModel::draw(const Eng3D::OpenGL::Program& shader, int) const {
    // Change color if material wants it
    shader.set_texture(0, "diffuse_map", *material->diffuse_map);
    shader.set_uniform("diffuse_color", material->diffuse_color);

    shader.set_texture(1, "ambient_map", *material->ambient_map);
    shader.set_uniform("ambient_color", material->ambient_color);
    
    shader.set_texture(2, "occlussion_map", *material->occlussion_map);
    shader.set_texture(3, "height_map", *material->height_map);
    
    shader.set_texture(4, "specular_map", *material->specular_map);
    shader.set_uniform("specular_color", material->specular_color);

    shader.set_texture(5, "normal_map", *material->normal_map);
    
    ((Eng3D::Mesh<glm::vec3, glm::vec2>*)this)->draw();
}

//
// Model
//
#ifdef E3D_FEATURE_ASSIMP
static inline std::shared_ptr<Eng3D::Texture> get_material_texture(const aiMaterial& material, aiTextureType type) {
    auto& s = Eng3D::State::get_instance();
    for(size_t i = 0; i < material.GetTextureCount(type); i++) {
        aiString str;
        material.GetTexture(type, i, &str);
        Eng3D::Log::debug("assimp", Eng3D::translate_format("Loading texture for material %s", str.C_Str()));
        auto path = std::string("gfx/") + str.C_Str();
        return s.tex_man.load(s.package_man.get_unique(path));
    }
    return s.tex_man.get_white();
}

Eng3D::SimpleModel Eng3D::Model::process_simple_model(aiMesh& mesh, const aiScene& scene) {
    Eng3D::SimpleModel simple_model(Eng3D::MeshMode::TRIANGLES);
    auto& s = Eng3D::State::get_instance();
    simple_model.buffer.resize(mesh.mNumVertices);
    glm::vec3 max_vert{};
    for(size_t i = 0; i < mesh.mNumVertices; i++)
        max_vert = glm::vec3(glm::max(max_vert.x, mesh.mVertices[i].x), glm::max(max_vert.y, mesh.mVertices[i].y), glm::max(max_vert.z, mesh.mVertices[i].z));

    max_vert = glm::vec3(max_vert.x, glm::max(max_vert.y, max_vert.x), glm::max(max_vert.z, max_vert.x));
    max_vert = glm::vec3(glm::max(max_vert.x, max_vert.y), max_vert.y, glm::max(max_vert.z, max_vert.y));
    max_vert = glm::vec3(glm::max(max_vert.x, max_vert.z), glm::max(max_vert.y, max_vert.z), max_vert.z);

    for(size_t i = 0; i < mesh.mNumVertices; i++) {
        auto vertice = glm::vec3(mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z) / max_vert;
        auto texcoord = glm::vec2(0.f, 0.f);
        if(mesh.mTextureCoords[0])
            texcoord = glm::vec2(mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y);
        simple_model.buffer[i] = Eng3D::MeshData<glm::vec3, glm::vec2>(vertice, texcoord);
    }

    for(size_t i = 0; i < mesh.mNumFaces; i++) {
        auto& face = mesh.mFaces[i];
        simple_model.indices.reserve(simple_model.indices.size() + face.mNumIndices);
        for(size_t j = 0; j < face.mNumIndices; j++)
            simple_model.indices.push_back(face.mIndices[j]);
    }

    auto& material = *scene.mMaterials[mesh.mMaterialIndex];

    // Textures
    simple_model.material = s.material_man.load(material.GetName().C_Str());
    simple_model.material->diffuse_map = get_material_texture(material, aiTextureType_DIFFUSE);
    simple_model.material->specular_map = get_material_texture(material, aiTextureType_SPECULAR);
    simple_model.material->ambient_map = get_material_texture(material, aiTextureType_AMBIENT);
    simple_model.material->height_map = get_material_texture(material, aiTextureType_HEIGHT);
    simple_model.material->occlussion_map = get_material_texture(material, aiTextureType_AMBIENT_OCCLUSION);
    simple_model.material->normal_map = get_material_texture(material, aiTextureType_NORMALS);

    // Colors
    aiColor4D diffuse;
    if(AI_SUCCESS == aiGetMaterialColor(&material, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
        simple_model.material->diffuse_color = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
    aiColor4D specular;
    if(AI_SUCCESS == aiGetMaterialColor(&material, AI_MATKEY_COLOR_SPECULAR, &specular))
        simple_model.material->specular_color = glm::vec4(specular.r, specular.g, specular.b, specular.a);
    aiColor4D ambient;
    if(AI_SUCCESS == aiGetMaterialColor(&material, AI_MATKEY_COLOR_AMBIENT, &ambient))
        simple_model.material->ambient_color = glm::vec4(ambient.r, ambient.g, ambient.b, ambient.a);

    simple_model.upload();
    return simple_model;
}

void Eng3D::Model::process_node(aiNode& node, const aiScene& scene) {
    // process all thehis-> node's meshes (if any)
    for(size_t i = 0; i < node.mNumMeshes; i++) {
        auto& mesh = *scene.mMeshes[node.mMeshes[i]];
        this->simple_models.push_back(this->process_simple_model(mesh, scene));
    }
    // then do the same for each of its children
    for(size_t i = 0; i < node.mNumChildren; i++)
        this->process_node(*node.mChildren[i], scene);
}
#endif

//
// ModelManager
//
std::shared_ptr<Eng3D::Model> Eng3D::ModelManager::load(const std::string& path) {
    auto it = models.find(path);
    if(it != models.cend())
        return (*it).second;

    // Wavefront OBJ loader
    std::shared_ptr<Eng3D::Model> model;
    try {
#ifdef E3D_FEATURE_ASSIMP
        /// @todo This is too horrible, we need a better solution
        Assimp::Importer importer;
        const auto* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
            CXX_THROW(std::runtime_error, importer.GetErrorString());
#endif
        model = std::make_shared<Eng3D::Model>();
#ifdef E3D_FEATURE_ASSIMP
        model->process_node(*scene->mRootNode, *scene);
#endif
    } catch(std::runtime_error& e) {
        // Make a dummy model
        model = std::make_shared<Eng3D::Model>();
    }
    models[path] = model;
    return model;
}

std::shared_ptr<Eng3D::Model> Eng3D::ModelManager::load(std::shared_ptr<Eng3D::IO::Asset::Base> asset) {
    return this->load(asset.get() != nullptr ? asset->get_abs_path() : "");
}
