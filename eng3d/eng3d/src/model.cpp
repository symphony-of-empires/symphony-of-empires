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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "eng3d/model.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/path.hpp"
#include "eng3d/material.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/shader.hpp"
#include "eng3d/state.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"

//
// Simple model
//
Eng3D::SimpleModel::SimpleModel(enum Eng3D::MeshMode _mode)
    : Eng3D::Mesh<glm::vec3, glm::vec2>(_mode)
{

}

void Eng3D::SimpleModel::draw(const Eng3D::OpenGL::Program& shader) const {
    // Change color if material wants it
    if(material.get() != nullptr) {
        shader.set_texture(0, "diffuse_map", *material->diffuse_map);
        shader.set_texture(1, "ambient_map", *material->ambient_map);
        shader.set_texture(2, "occlussion_map", *material->occlussion_map);
        shader.set_texture(3, "height_map", *material->height_map);
        shader.set_uniform("ambient_color", material->ambient_color);
        shader.set_uniform("diffuse_color", material->diffuse_color);
    } else {
        auto white_tex = Eng3D::State::get_instance().tex_man->get_white();
        shader.set_texture(0, "diffuse_map", *white_tex.get());
        shader.set_texture(1, "ambient_map", *white_tex.get());
        shader.set_texture(2, "occlussion_map", *white_tex.get());
        shader.set_texture(3, "height_map", *white_tex.get());
        shader.set_uniform("ambient_color", glm::vec4(1.f));
        shader.set_uniform("diffuse_color", glm::vec4(1.f));
    }

    vao.bind();
    if(!indices.empty()) {
        glDrawElements(static_cast<GLenum>(mode), indices.size(), GL_UNSIGNED_INT, 0);
    } else if(!buffer.empty()) {
        glDrawArrays(static_cast<GLenum>(mode), 0, buffer.size());
    }
}

//
// Model
//
static inline std::shared_ptr<Eng3D::Texture> get_material_texture(const aiMaterial& material, aiTextureType type) {
    auto& s = Eng3D::State::get_instance();
    for(size_t i = 0; i < material.GetTextureCount(type); i++) {
        aiString str;
        material.GetTexture(type, i, &str);
        Eng3D::Log::debug("assimp", std::string() + "Loading texture for material " + str.C_Str());
        return s.tex_man->load(s.package_man->get_unique(str.C_Str()));
    }
    return s.tex_man->get_white();
}

Eng3D::SimpleModel Eng3D::Model::process_simple_model(aiMesh& mesh, const aiScene& scene) {
    Eng3D::SimpleModel simple_model = Eng3D::SimpleModel(Eng3D::MeshMode::TRIANGLES);
    auto& s = Eng3D::State::get_instance();

    glm::vec3 max_vert = glm::vec3(0.00001f, 0.00001f, 0.00001f);
    for(size_t i = 0; i < mesh.mNumVertices; i++) {
        auto vertice = glm::vec3(mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z);
        if(std::abs(vertice.x) > std::abs(max_vert.x)) {
            max_vert.x = vertice.x;   
        } else if(std::abs(vertice.y) > std::abs(max_vert.y)) {
            max_vert.y = vertice.y;   
        } else if(std::abs(vertice.z) > std::abs(max_vert.z)) {
            max_vert.z = vertice.z;   
        }
    }

    // Use the reciprocal for using mutliply instead of fucking division
    max_vert.x = 1.f / max_vert.x;
    max_vert.y = 1.f / max_vert.y;
    max_vert.z = 1.f / max_vert.z;

    simple_model.buffer.resize(mesh.mNumVertices);
    for(size_t i = 0; i < mesh.mNumVertices; i++) {
        auto vertice = glm::vec3(mesh.mVertices[i].x * max_vert.x, mesh.mVertices[i].y * max_vert.y, mesh.mVertices[i].z * max_vert.z);
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

    simple_model.material = s.material_man->load(material.GetName().C_Str());
    simple_model.material->diffuse_map = get_material_texture(material, aiTextureType_DIFFUSE);
    simple_model.material->specular_map = get_material_texture(material, aiTextureType_SPECULAR);
    simple_model.material->ambient_map = get_material_texture(material, aiTextureType_AMBIENT);
    simple_model.material->height_map = get_material_texture(material, aiTextureType_HEIGHT);
    simple_model.material->occlussion_map = get_material_texture(material, aiTextureType_AMBIENT_OCCLUSION);

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

std::shared_ptr<Eng3D::Model> Eng3D::ModelManager::load(const std::string& path) {
    auto it = models.find(path);
    if(it != models.cend())
        return (*it).second;

    // Wavefront OBJ loader
    std::shared_ptr<Eng3D::Model> model;
    try {
        /// @todo This is too horrible, we need a better solution
        // model =
        Assimp::Importer importer;
        const auto* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
            CXX_THROW(std::runtime_error, importer.GetErrorString());

        model = std::make_shared<Eng3D::Model>(Eng3D::Model());
        model->process_node(*scene->mRootNode, *scene);
    } catch(std::runtime_error& e) {
        // Make a dummy model
        model = std::make_shared<Eng3D::Model>(Eng3D::Model());
    }
    models[path] = model;
    return model;
}

std::shared_ptr<Eng3D::Model> Eng3D::ModelManager::load(std::shared_ptr<Eng3D::IO::Asset::Base> asset) {
    return this->load(asset.get() != nullptr ? asset->get_abs_path() : "");
}
