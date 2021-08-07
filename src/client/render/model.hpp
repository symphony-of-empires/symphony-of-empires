#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

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

#endif