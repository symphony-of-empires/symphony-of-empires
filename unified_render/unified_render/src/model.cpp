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

#include "unified_render/model.hpp"
#include "unified_render/shader.hpp"
#include "unified_render/print.hpp"
#include "unified_render/path.hpp"
#include "unified_render/material.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/shader.hpp"
#include "unified_render/state.hpp"

//
// VAO
//
UnifiedRender::OpenGL::VAO::VAO(void) {
    glGenVertexArrays(1, &id);
}

UnifiedRender::OpenGL::VAO::~VAO(void) {
    glDeleteVertexArrays(1, &id);
}

void UnifiedRender::OpenGL::VAO::bind(void) const {
    glBindVertexArray(id);
}

GLuint UnifiedRender::OpenGL::VAO::get_id(void) const {
    return id;
}

//
// VBO
//
UnifiedRender::OpenGL::VBO::VBO(void) {
    glGenBuffers(1, &id);
}

UnifiedRender::OpenGL::VBO::~VBO(void) {
    glDeleteBuffers(1, &id);
}

void UnifiedRender::OpenGL::VBO::bind(GLenum target) const {
    glBindBuffer(target, id);
}

GLuint UnifiedRender::OpenGL::VBO::get_id(void) const {
    return id;
}

//
// Simple model
//
UnifiedRender::SimpleModel::SimpleModel(enum UnifiedRender::MeshMode _mode)
    : UnifiedRender::Mesh<glm::vec3, glm::vec2>(_mode)
{

}

UnifiedRender::SimpleModel::~SimpleModel(void) {

}

void UnifiedRender::SimpleModel::draw(const UnifiedRender::OpenGL::Program& shader) const {
    // Change color if material wants it
    if(material != nullptr) {
        if(material->diffuse_map != nullptr) {
            shader.set_texture(0, "diffuse_map", *material->diffuse_map);
        }
        else {
            auto& white_tex = UnifiedRender::State::get_instance().tex_man->get_white();
            shader.set_texture(0, "diffuse_map", white_tex);
        }
        shader.set_uniform("ambient_color", material->ambient_color);
        shader.set_uniform("diffuse_color", material->diffuse_color);
    }
    else {
        auto& white_tex = UnifiedRender::State::get_instance().tex_man->get_white();
        shader.set_texture(0, "diffuse_map", white_tex);
        shader.set_uniform("ambient_color", glm::vec3(1.f));
        shader.set_uniform("diffuse_color", glm::vec3(1.f));
    }

    vao.bind();
    glDrawArrays(static_cast<GLenum>(mode), 0, buffer.size());
}

//
// Model
//
UnifiedRender::Model::Model(void) {

}

UnifiedRender::Model::~Model(void) {

}

void UnifiedRender::Model::draw(const UnifiedRender::OpenGL::Program& shader) const {
    std::vector<const UnifiedRender::SimpleModel*>::const_iterator model;
    for(model = simple_models.cbegin(); model != simple_models.cend(); model++) {
        (*model)->draw(shader);
    }
}

UnifiedRender::ModelManager::~ModelManager(void) {
    for(const auto& model : models) {
        delete model.second;
    }
    models.clear();
}

const UnifiedRender::Model& UnifiedRender::ModelManager::load_wavefront(const std::string& path) {
    class WavefrontFace {
    public:
        // Indexes to the actual points
        std::vector<int> vertices, texcoords, normals;
    };

    class WavefrontObj {
    public:
        WavefrontObj(const std::string& _name): name(_name) {};
        std::string name;
        std::vector<WavefrontFace> faces;
        const UnifiedRender::Material* material = nullptr;
    };

    std::ifstream file(path);
    std::string line;

    // Recollect all data from the .OBJ file
    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec2> texcoords;

    std::vector<WavefrontObj> objects;
    objects.push_back(WavefrontObj("default"));
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
        if(cmd == "mtllib") {
            std::string name;
            sline >> name;
            State::get_instance().material_man->load_wavefront(Path::get("models/" + name), path);
        }
        else if(cmd == "usemtl") {
            std::string name = path;
            sline >> name;
            objects.back().material = UnifiedRender::State::get_instance().material_man->load(path + "-" + name);
        }
        else if(cmd == "o") {
            std::string name;
            sline >> name;
            objects.push_back(WavefrontObj(name));
        }
        else if(cmd == "v") {
            glm::vec3 vert;
            sline >> vert.x >> vert.y >> vert.z;
            vertices.push_back(vert);
        }
        else if(cmd == "vt") {
            glm::vec2 tex;
            sline >> tex.x >> tex.y;
            texcoords.push_back(tex);
        }
        else if(cmd == "vn") {
            glm::vec3 norm;
            sline >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);
        }
        else if(cmd == "f") {
            WavefrontFace face = WavefrontFace();
            while(sline.peek() != -1) {
                // Assemble faces - allowing for any number of vertices
                // (and respecting the optional-ity of vt and vn fields)
                int value;
                char ch;

                // Vertices
                sline >> value;
                face.vertices.push_back(value);

                ch = sline.peek();
                if(ch == '/') {
                    sline >> ch;

                    // Texcoords
                    sline >> value;
                    face.texcoords.push_back(value);

                    ch = sline.peek();
                    if(ch == '/') {
                        sline >> ch;

                        // Normals
                        sline >> value;
                        face.normals.push_back(value);

                        ch = sline.peek();
                        if(ch == '/') {
                            print_error("Invalid face declaration");
                            break;
                        }
                    }
                }
            }

            if(face.vertices.size() < 3) {
                print_error("Cannot create polygon - malformed face?");
            }
            else {
                objects.back().faces.push_back(face);
            }
        }
        else if(cmd == "s") {
            std::string light_mode;
            sline >> light_mode;

            if(light_mode != "off") {
                print_error("Unsupported light mode %s", light_mode.c_str());
            }
        }
        else {
            print_error("Unsupported command %s", cmd.c_str());
        }
    }

    // Convert objects into (UnifiedRender) simple objects so we can now use them
    UnifiedRender::Model* final_model = new UnifiedRender::Model();
    for(std::vector<WavefrontObj>::const_iterator obj = objects.cbegin(); obj != objects.cend(); obj++) {
        // Fill up the trigonometric buffers, we will first read all the faces and make them separate
        std::vector<std::vector<UnifiedRender::MeshData<glm::vec3, glm::vec2>>> clusters{};
        for(std::vector<WavefrontFace>::const_iterator face = (*obj).faces.cbegin(); face != (*obj).faces.cend(); face++) {
            std::vector<UnifiedRender::MeshData<glm::vec3, glm::vec2>> cluster{};

            // The faces dictate indices for the vertices and we will also subtract 1 because the indexing is 0 based
            if((*face).vertices.size() == (*face).texcoords.size()) {
                for(unsigned int i = 0; i < (*face).vertices.size(); i++) {
                    cluster.push_back(UnifiedRender::MeshData<glm::vec3, glm::vec2>(
                        glm::vec3(vertices[(*face).vertices[i] - 1]),
                        glm::vec2(texcoords[(*face).texcoords[i] - 1])
                        ));
                }
            }
            else {
                for(unsigned int i = 0; i < (*face).vertices.size(); i++) {
                    cluster.push_back(UnifiedRender::MeshData<glm::vec3, glm::vec2>(
                        glm::vec3(vertices[(*face).vertices[i] - 1]),
                        glm::vec2(0.f, 0.f)
                        ));
                }
            }
            clusters.push_back(cluster);
        }

        // TODO: This can be optimized even more

        // Now it's time to merge it all into a single mesh

        // It's time to merge clusters which share triangle nodes, in such case we reorganize the triangles so they
        // are to "follow" the triangle fan
        std::vector<std::vector<UnifiedRender::MeshData<glm::vec3, glm::vec2>>>::const_iterator cluster;
        for(cluster = clusters.cbegin(); cluster != clusters.cend(); cluster++) {
            UnifiedRender::SimpleModel* model = new UnifiedRender::SimpleModel(UnifiedRender::MeshMode::TRIANGLE_FAN);

            std::vector<UnifiedRender::MeshData<glm::vec3, glm::vec2>>::const_iterator v1;
            for(v1 = (*cluster).cbegin(); v1 != (*cluster).cend(); v1++) {
                model->buffer.push_back(*v1);
            }

            print_info("Created new SimpleModel with %zu vertices", model->buffer.size());
            model->material = (*obj).material;
            model->upload();
            final_model->simple_models.push_back(model);
        }
    }

    models[path] = final_model;
    return *final_model;
}

const UnifiedRender::Model& UnifiedRender::ModelManager::load_stl(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});

    UnifiedRender::Model* final_model = new UnifiedRender::Model();
    UnifiedRender::SimpleModel* model = new UnifiedRender::SimpleModel(UnifiedRender::MeshMode::TRIANGLES);

    // TODO: This needs more work
    // 1. we need little endian reading
    uint32_t n_triangles;
    std::memcpy(&n_triangles, &buffer[80], sizeof(uint32_t));

    for(uint32_t i = 0; i < n_triangles; i++) {
        glm::vec3 vert;

        // TODO: We need to guarantee 2 things:
        // 1. that the floating point is 32-bits
        // 2. little endian
        // 3. ieee787 floating point
        memcpy(&vert.x, &buffer[84 + i * (sizeof(float) * 3)], sizeof(float));
        memcpy(&vert.y, &buffer[88 + i * (sizeof(float) * 3)], sizeof(float));
        memcpy(&vert.z, &buffer[92 + i * (sizeof(float) * 3)], sizeof(float));

        model->buffer.push_back(UnifiedRender::MeshData<glm::vec3, glm::vec2>(
            glm::vec3(vert),
            glm::vec2(0.f, 0.f)
            ));
    }

    final_model->simple_models.push_back(model);
    models[path] = final_model;
    return *final_model;
}

const UnifiedRender::Model& UnifiedRender::ModelManager::load(const std::string& path) {
    std::map<std::string, UnifiedRender::Model*>::const_iterator it = models.find(path);
    if(it != models.cend()) {
        return *((*it).second);
    }

    // Wavefront OBJ loader
    try {
        // TODO: This is too horrible, we need a better solution
        if(path.length() > 3 && path[path.length() - 3] == 's' && path[path.length() - 2] == 't' && path[path.length() - 1] == 'l') {
            return load_stl(path);
        }
        else {
            return load_wavefront(path);
        }
    }
    catch(std::ifstream::failure& e) {
        throw ("Model " + path + " not found");
    }
}