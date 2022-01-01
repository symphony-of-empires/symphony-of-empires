#pragma once

#include <vector>
#include <string>
#include <set>
#include <cstdint>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

namespace UnifiedRender {
    class Material;
    namespace OpenGL {
        class Program;
    };
};

namespace UnifiedRender {
    namespace OpenGL {
        class VAO {
            GLuint id;
        public:
            VAO(void);
            ~VAO(void);
            VAO(const VAO&) = default;
            VAO(VAO&&) noexcept = default;
            VAO& operator=(const VAO&) = default;

            void bind(void) const;
            GLuint get_id(void) const;
        };

        class VBO {
            GLuint id;
        public:
            VBO(void);
            ~VBO(void);
            VBO(const VBO&) = default;
            VBO(VBO&&) noexcept = default;
            VBO& operator=(const VBO&) = default;

            void bind(GLenum target = GL_ARRAY_BUFFER) const;
            GLuint get_id(void) const;
        };

        template<typename V, typename T>
        class PackedData {
        public:
            V vert;
            T tex;

            PackedData(void) {};
            PackedData(V _vert, T _tex): vert(_vert), tex(_tex) {};
            ~PackedData() {};
            PackedData(const PackedData&) = default;
            PackedData(PackedData&&) noexcept = default;
            PackedData& operator=(const PackedData&) = default;
        };

        // Packed model - packs both vertices and texcoords into the same buffer
        template<typename V, typename T>
        class PackedModel {
        public:
            std::vector<PackedData<V, T>> buffer;
            VAO vao;
            VBO vbo;
            GLint mode;

            PackedModel(GLint _mode): mode(_mode) {};
            ~PackedModel() {};
            PackedModel(const PackedModel&) = default;
            PackedModel(PackedModel&&) noexcept = default;
            PackedModel& operator=(const PackedModel&) = default;

            virtual void draw(void) const {
                vao.bind();
                glDrawArrays(mode, 0, buffer.size());
            }
        };
    };

    // A simple object - use these to store "simple" objects that MAY repeat
    // TODO: We should use instancing tricks on simple objects
    class SimpleModel : public OpenGL::PackedModel<glm::vec3, glm::vec2> {
    public:
        SimpleModel(GLint _mode);
        ~SimpleModel(void);
        SimpleModel(const SimpleModel&) = default;
        SimpleModel(SimpleModel&&) noexcept = default;
        SimpleModel& operator=(const SimpleModel&) = default;
        virtual void draw(const UnifiedRender::OpenGL::Program& shader) const;
        void upload();

        const Material* material = nullptr;
    };

    // A complex object being composed by many simple objects
    class Model {
    public:
        Model(void);
        ~Model(void);
        Model(const Model&) = default;
        Model(Model&&) noexcept = default;
        Model& operator=(const Model&) = default;
        virtual void draw(const UnifiedRender::OpenGL::Program& shader) const;

        std::vector<const SimpleModel*> simple_models;
    };

    struct ModelManager {
    private:
        std::set<std::pair<SimpleModel*, std::string>> simple_models;
        std::set<std::pair<Model*, std::string>> complex_models;

        const UnifiedRender::Model& load_wavefront(const std::string& path);
        const UnifiedRender::Model& load_stl(const std::string& path);
    public:
        const SimpleModel& load_simple(const std::string& path);
        const Model& load(const std::string& path);
    };
}