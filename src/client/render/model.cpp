#include <algorithm>
#include <fstream>
#include "model.hpp"
#include "print.hpp"
#include "path.hpp"
#include "shader.hpp"

UnifiedRender::SimpleModel::SimpleModel(GLint _mode) : UnifiedRender::OpenGl::PackedModel<glm::vec3, glm::vec2, glm::vec3>(_mode) {

}

void UnifiedRender::SimpleModel::draw(UnifiedRender::OpenGl::Program* shader) const {
    glActiveTexture(GL_TEXTURE0);
    if(material != nullptr && material->texture != nullptr) {
        material->texture->bind();
    }
    // Change color if material wants it
    shader->set_uniform("color", 1.f, 1.f, 1.f, 1.f);
    vao.bind();
    glDrawArrays(mode, 0, buffer.size());
}

void UnifiedRender::SimpleModel::upload(void) {
    vao.bind();
    vbo.bind(GL_ARRAY_BUFFER);

    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(buffer[0]), &buffer[0], GL_STATIC_DRAW);
    // Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)0);
    glEnableVertexArrayAttrib(vao.get_id(), 0);
    // Texcoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)(3 * sizeof(float)));
    glEnableVertexArrayAttrib(vao.get_id(), 1);
    // Colours
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)(5 * sizeof(float)));
    glEnableVertexArrayAttrib(vao.get_id(), 2);
}

UnifiedRender::ComplexModel::ComplexModel(void) {

}

void UnifiedRender::ComplexModel::draw(UnifiedRender::OpenGl::Program* shader) const {
    for(auto& model: simple_models) {
        model->draw(shader);
    }
}

const UnifiedRender::SimpleModel& UnifiedRender::ModelManager::load_simple(const std::string& path) {
    return *((const UnifiedRender::SimpleModel *)NULL);
}

class WavefrontFace {
public:
    WavefrontFace() {};
    ~WavefrontFace() {};

    // Indexes to the actual points
    std::vector<int> vertices, texcoords, normals;
};

class WavefrontObj {
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
        if(line[0] == '#')
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
            objects.front().material = &g_material_manager->load_material(name);
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
                    if(c == '/') {
                        // Consume the character
                        sline >> c;
                    }
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
                // The faces dictate indices for the vertices and stuff and we
                // will also subtract 1 because the indexing is 0 based
                model->buffer.push_back(UnifiedRender::OpenGl::PackedData(
                    glm::vec3(obj.vertices[
                        std::min<size_t>(obj.vertices.size(), face.vertices[i] - 1)
                    ]),
                    glm::vec2(obj.texcoords[
                        std::min<size_t>(obj.texcoords.size(), face.texcoords[i] - 1)
                    ]),
                    glm::vec3(1.f, 1.f, 1.f)
                ));
            }
        }
        model->material = obj.material;
        model->upload();

        simple_models.insert(std::pair(model, obj.name));
        final_model->simple_models.push_back(model);
    }
    complex_models.insert(std::pair(final_model, path));
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
        return load_wavefront(path);
    } catch(std::ifstream::failure& e) {
        print_error("Model %s not found", path.c_str());

        UnifiedRender::ComplexModel* complex = new UnifiedRender::ComplexModel();
        complex_models.insert(std::make_pair(complex, path));
        return *(const UnifiedRender::ComplexModel*)complex;
    }
}

UnifiedRender::ModelManager* g_model_manager;
