#include <algorithm>
#include <fstream>
#include <vector>
#include <iterator>

#include "unified_render/model.hpp"
#include "unified_render/shader.hpp"
#include "unified_render/print.hpp"
#include "path.hpp"
#include "unified_render/material.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/shader.hpp"

UnifiedRender::SimpleModel::SimpleModel(GLint _mode) : UnifiedRender::OpenGl::PackedModel<glm::vec3, glm::vec2>(_mode) {

}

void UnifiedRender::SimpleModel::draw(UnifiedRender::OpenGl::Program* shader) const {
    // Change color if material wants it
    if(material != nullptr) {
        if(material->texture != nullptr) {
            material->texture->bind();
        } else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        shader->set_uniform("colour", material->ambient_colour.r, material->ambient_colour.g, material->ambient_colour.b, 1.f);
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
        shader->set_uniform("colour", 1.f, 1.f, 1.f, 1.f);
    }

    vao.bind();
    glDrawArrays(mode, 0, buffer.size());
}

void UnifiedRender::SimpleModel::upload(void) {
    vao.bind();
    vbo.bind(GL_ARRAY_BUFFER);
    
    if(buffer.size() == 0) return;
    
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(buffer[0]), &buffer[0], GL_STATIC_DRAW);
    // Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)0);
    glEnableVertexAttribArray(0);
    // Texcoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

UnifiedRender::ComplexModel::ComplexModel(void) {

}

void UnifiedRender::ComplexModel::draw(UnifiedRender::OpenGl::Program* shader) const {
    for(const auto& model: simple_models) {
        model->draw(shader);
    }
}

#ifdef UR_NO_ASSIMP
const UnifiedRender::SimpleModel& UnifiedRender::ModelManager::load_simple(const std::string& path) {
    return *((const UnifiedRender::SimpleModel *)nullptr);
}

struct WavefrontFace {
public:
    WavefrontFace() {};
    ~WavefrontFace() {};

    // Indexes to the actual points
    std::vector<int> vertices, texcoords, normals;
};

struct WavefrontObj {
public:
    WavefrontObj(const std::string& _name) : name(_name) {};
    ~WavefrontObj() {};

    std::string name;

    std::vector<WavefrontFace> faces;
    std::vector<glm::vec3> vertices, texcoords, normals;
    const UnifiedRender::Material* material = nullptr;
};

const UnifiedRender::ComplexModel& UnifiedRender::ModelManager::load_wavefront(const std::string& path) {
    std::ifstream file(path);
    std::string line;

    // Recollect all data from the .OBJ file
    std::vector<WavefrontObj> objects;
    objects.push_back(WavefrontObj("default"));
    while(std::getline(file, line)) {
        // Skip whitespace
        size_t len = line.find_first_not_of(" \t");
        if(len != std::string::npos) {
            line = line.substr(len, line.length() - len);
        }

        // Comment
        if(line[0] == '#' || line.empty())
            continue;
        
        std::istringstream sline(line);
        std::string cmd;
        sline >> cmd;

        if(cmd == "mtllib") {
            std::string name;
            sline >> name;
            name = "3d/" + name;
            g_material_manager->load_wavefront(Path::get(name));
        } else if(cmd == "usemtl") {
            std::string name;
            sline >> name;
            objects.front().material = &g_material_manager->loadMaterial(name);
        } else if(cmd == "o") {
            std::string name;
            sline >> name;
            objects.push_back(WavefrontObj(name));
        } else if(cmd == "v") {
            glm::vec3 vert;
            sline >> vert.x >> vert.y >> vert.z;
            objects.front().vertices.push_back(vert);
        } else if(cmd == "vt") {
            glm::vec3 tex;
            sline >> tex.x >> tex.y >> tex.z;
            objects.front().texcoords.push_back(tex);
        } else if(cmd == "vn") {
            glm::vec3 norm;
            sline >> norm.x >> norm.y >> norm.z;
            objects.front().normals.push_back(norm);
        } else if(cmd == "f") {
            objects.front().faces.push_back(WavefrontFace());
            while(sline.peek() != -1) {
                // Assemble faces - allowing for any number of vertices
                // (and respecting the optional-ity of vt and vn fields)
                int n_elem = 0, value = 1;
                char c = '/';
                while(c == '/' && n_elem < 3 && value != 0) {
                    sline >> value;
                    
                    switch(n_elem) {
                    case 0:
                        objects.front().faces.front().vertices.push_back(value);
                        break;
                    case 1:
                        objects.front().faces.front().texcoords.push_back(value);
                        break;
                    case 2:
                        objects.front().faces.front().normals.push_back(value);
                        break;
                    default:
                        print_error("Unknown index %i", n_elem);
                        break;
                    }
                    n_elem++;

                    c = sline.peek();

                    // Consume the character
                    if(c == '/') sline >> c;
                }
            }

            if(objects.front().faces.front().vertices.size() < 3
            || objects.front().faces.front().texcoords.size() < 3
            || objects.front().faces.front().normals.size() < 3) {
                print_error("Cannot create polygon - malformed face!?!");
                objects.front().faces.erase(objects.front().faces.end());
            }
        }
    }

    // Convert objects into (UnifiedRender) simple objects so we can now use them
    ComplexModel* final_model = new ComplexModel();
    for(const auto& obj: objects) {
        // Register each simple object to the model manager
        SimpleModel* model = new SimpleModel(GL_TRIANGLES);
        for(const auto& face: obj.faces) {
            for(size_t i = 0; i < face.vertices.size(); i++) {
                if(i >= 3) {
                    const size_t last_idx = model->buffer.size() - 1;
                    model->buffer.push_back(model->buffer[last_idx - 1]);
                    model->buffer.push_back(model->buffer[last_idx - 0]);
                }

                // The faces dictate indices for the vertices and stuff and we
                // will also subtract 1 because the indexing is 0 based
                model->buffer.push_back(UnifiedRender::OpenGl::PackedData(
                    glm::vec3(obj.vertices[
                        std::min<size_t>(obj.vertices.size() - 1, face.vertices[i] - 1)
                    ]),
                    glm::vec2(obj.texcoords[
                        std::min<size_t>(obj.texcoords.size() - 1, face.texcoords[i] - 1)
                    ])
                ));
            }
        }
        model->material = obj.material;
        model->upload();

        simple_models.insert(std::make_pair(model, obj.name));
        final_model->simple_models.push_back(model);
    }
    complex_models.insert(std::make_pair(final_model, path));
    return *final_model;
}

const UnifiedRender::ComplexModel& UnifiedRender::ModelManager::load_stl(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});

    ComplexModel* final_model = new ComplexModel();
    SimpleModel* model = new SimpleModel(GL_TRIANGLES);

    // TODO: This needs more work
    // 1. we need little endian reading
    uint32_t n_triangles;
    memcpy(&n_triangles, &buffer[80], sizeof(uint32_t));

    for(uint32_t i = 0; i < n_triangles; i++) {
        glm::vec3 vert;

        // TODO: We need to guarantee 2 things:
        // 1. that the floating point is 32-bits
        // 2. little endian
        // 3. ieee787 floating point
        memcpy(&vert.x, &buffer[84 + i * (sizeof(float) * 3)], sizeof(float));
        memcpy(&vert.y, &buffer[88 + i * (sizeof(float) * 3)], sizeof(float));
        memcpy(&vert.z, &buffer[92 + i * (sizeof(float) * 3)], sizeof(float));

        model->buffer.push_back(UnifiedRender::OpenGl::PackedData(
            glm::vec3(vert),
            glm::vec2(0.f, 0.f)
        ));
    }

    simple_models.insert(std::make_pair(model, path));
    final_model->simple_models.push_back(model);
    complex_models.insert(std::make_pair(final_model, path));
    return *final_model;
}

const UnifiedRender::ComplexModel& UnifiedRender::ModelManager::load_complex(const std::string& path) {
    auto it = std::find_if(complex_models.begin(), complex_models.end(), [&path](const std::pair<UnifiedRender::ComplexModel*, std::string>& element) {
        return (element.second == path);
    });

    if(it != complex_models.end())
        return *((*it).first);
    
    // Wavefront OBJ loader
    try {
        // TODO: This is too horrible, we need a better solution
        if(path[path.length() - 3] == 's'
        && path[path.length() - 2] == 't'
        && path[path.length() - 1] == 'l') {
            return load_stl(path);
        } else {
            return load_wavefront(path);
        }
    } catch(std::ifstream::failure& e) {
        throw ("Model " + path + " not found");
    }
}

UnifiedRender::ModelManager* g_model_manager;
#endif