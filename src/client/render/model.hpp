#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>

namespace UnifiedRender::OpenGl {
    /**
     * Packed model - packs both vertices and texcoords into the same buffer
     */
    template<typename V, typename T>
    class PackedModel {
    protected:
        std::vector<std::pair<V, T>> buffer;
        GLuint vbo, vao;
        GLint mode;
    public:
        PackedModel(GLint _mode) : mode(_mode) {};
        ~PackedModel() {};

        virtual void draw(void) {
            glBindVertexArray(vao);
            glDrawArrays(mode, 0, buffer.size());
        }
    };
}

#endif