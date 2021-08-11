#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>

namespace UnifiedRender::OpenGl {
    class VAO {
        GLuint id;
    public:
        VAO(void) {
            glGenVertexArrays(1, &id);
        };
        ~VAO() {
            glDeleteVertexArrays(1, &id);
        };
        VAO(const VAO&) = default;
        VAO(VAO&&) noexcept = default;
        VAO& operator=(const VAO&) = default;

        inline void bind(void) {
            glBindVertexArray(id);
        };
        inline GLuint get_id(void) {
            return id;
        };
    };

    class VBO {
        GLuint id;
    public:
        VBO(void) {
            glGenBuffers(1, &id);
        };
        ~VBO() {
            glDeleteBuffers(1, &id);
        };
        VBO(const VBO&) = default;
        VBO(VBO&&) noexcept = default;
        VBO& operator=(const VBO&) = default;

        inline void bind(GLenum target = GL_ARRAY_BUFFER) {
            glBindBuffer(target, id);
        };
        inline GLuint get_id(void) {
            return id;
        };
    };

    template<typename V, typename T, typename C>
    class PackedData {
    public:
        V vert;
        T tex;
        C colour;

        PackedData(void) {};
        PackedData(V _vert, T _tex, C _colour) : vert(_vert), tex(_tex), colour(_colour) {};
        ~PackedData() {};
        PackedData(const PackedData&) = default;
        PackedData(PackedData&&) noexcept = default;
        PackedData& operator=(const PackedData&) = default;
    };

    /**
     * Packed model - packs both vertices and texcoords into the same buffer
     */
    template<typename V, typename T, typename C>
    class PackedModel {
    public:
        std::vector<PackedData<V, T, C>> buffer;
        VAO vao;
        VBO vbo;
        GLint mode;
        
        PackedModel(GLint _mode) : mode(_mode) {};
        ~PackedModel() {};
        PackedModel(const PackedModel&) = default;
        PackedModel(PackedModel&&) noexcept = default;
        PackedModel& operator=(const PackedModel&) = default;

        virtual void draw(void) {
            vao.bind();
            glDrawArrays(mode, 0, buffer.size());
        };
    };
}

class Material;
namespace UnifiedRender {
    /**
     * A simple object - use these to store "simple" objects that MAY repeat
     * TODO: We should use instancing tricks on simple objects
     */
    class SimpleModel : OpenGl::PackedModel<glm::vec3, glm::vec2, glm::vec3> {
    public:
        SimpleModel(GLint _mode) : PackedModel(_mode) {};
        ~SimpleModel() {};
        SimpleModel(const SimpleModel&) = default;
        SimpleModel(SimpleModel&&) noexcept = default;
        SimpleModel& operator=(const SimpleModel&) = default;

        const Material *material;
    };

    /**
     * A complex object being composed by many simple objects
     */
    class ComplexModel : OpenGl::PackedModel<glm::vec3, glm::vec2, glm::vec3> {
    public:
        ComplexModel(GLint _mode) : PackedModel(_mode) {};
        ~ComplexModel() {};
        ComplexModel(const ComplexModel&) = default;
        ComplexModel(ComplexModel&&) noexcept = default;
        ComplexModel& operator=(const ComplexModel&) = default;

        std::vector<const SimpleModel *> simple_models;
    };
}

#include <string>
#include <set>
namespace UnifiedRender {
    class ModelManager {
    private:
        std::set<std::pair<SimpleModel*, std::string>> simple_models;
        std::set<std::pair<ComplexModel*, std::string>> complex_models;
    public:
        const SimpleModel& load_simple(std::string path);
        const ComplexModel& load_complex(std::string path);
    };
}
extern UnifiedRender::ModelManager* g_model_manager;

#endif