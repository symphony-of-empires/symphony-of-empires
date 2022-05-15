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
//      material.cpp
//
// Abstract:
//      Defines functions for the loading, management, creation and destruction
//      of a material resource.
// ----------------------------------------------------------------------------

#include <algorithm>
#include <fstream>
#include <string>

#include "eng3d/material.hpp"
#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/state.hpp"
#include "eng3d/log.hpp"

//
// Material
//
Eng3D::Material::Material(void) {

}

Eng3D::Material::~Material(void) {

}

//
// Material manager
//
Eng3D::MaterialManager::MaterialManager(void) {

}

Eng3D::MaterialManager::~MaterialManager(void) {
    for(const auto& material : materials) {
        delete material.second;
    }
    materials.clear();
}

std::vector<std::pair<Eng3D::Material*, std::string>> Eng3D::MaterialManager::load_wavefront(
    const std::string& path, const std::string& model_name)
{
    std::ifstream file(path);
    std::string line;

    Eng3D::Log::debug("material", "Loading " + path);

    std::vector<std::pair<Eng3D::Material*, std::string>> tmp_mat;
    Eng3D::Material* curr_mat = nullptr;
    while(std::getline(file, line)) {
        // Skip whitespace
        size_t len = line.find_first_not_of(" \t");
        if(len != std::string::npos) {
            line = line.substr(len, line.length() - len);
        }

        // Comment
        if(line[0] == '#' || line.empty()) {
            continue;
        }

        std::istringstream sline(line);
        std::string cmd;
        sline >> cmd;
        if(cmd != "newmtl" && curr_mat == nullptr) {
            continue;
        }

        if(cmd == "newmtl") {
            std::string name;
            sline >> name;
            curr_mat = new Eng3D::Material();
            tmp_mat.push_back(std::make_pair(curr_mat, model_name + "-" + name));
        } else if(cmd == "Ka") {
            glm::vec3 color;
            sline >> color.x >> color.y >> color.z;
            curr_mat->ambient_color = glm::vec4(color, 1.f);
        } else if(cmd == "Kd") {
            glm::vec3 color;
            sline >> color.x >> color.y >> color.z;
            curr_mat->diffuse_color = glm::vec4(color, 1.f);
        } else if(cmd == "Ks") {
            glm::vec3 color;
            sline >> color.x >> color.y >> color.z;
            curr_mat->specular_color = glm::vec4(color, 1.f);
        } else if(cmd == "Ns") {
            sline >> curr_mat->specular_exp;
        } else if(cmd == "Ni") {
            sline >> curr_mat->optical_density;
        } else if(cmd == "map_Kd") {
            std::string map_path;
            sline >> map_path;

            if(map_path[0] == '.') {
                continue;
            }
            curr_mat->diffuse_map = Eng3D::State::get_instance().tex_man->load(Eng3D::State::get_instance().package_man->get_unique("gfx/" + map_path));
        } else {
            Eng3D::Log::debug("material", "Command " + cmd + " unknown");
        }
    }

    std::vector<std::pair<Eng3D::Material*, std::string>>::const_iterator mat;
    for(mat = tmp_mat.begin(); mat != tmp_mat.end(); mat++) {
        materials[(*mat).second] = (*mat).first;
    }
    return tmp_mat;
}

const Eng3D::Material* Eng3D::MaterialManager::load(const std::string& path) {
    std::map<std::string, Eng3D::Material*>::const_iterator it;
    it = materials.find(path);
    if(it != materials.end()) {
        return ((*it).second);
    }

    Eng3D::Log::error("material", path + " not found");
    return nullptr;
}
