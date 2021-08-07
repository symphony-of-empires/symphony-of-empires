#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace UnifiedRender::OpenGl {
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
        GLuint vbo, vao;
        GLint mode;
        
        PackedModel(GLint _mode) : mode(_mode) {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
        };

        ~PackedModel() {
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
        };

        virtual void draw(void) {
            glBindVertexArray(vao);
            glDrawArrays(mode, 0, buffer.size());
        };
    };
}

#endif