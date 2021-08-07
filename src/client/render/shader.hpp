#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include "path.hpp"
#include "print.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

namespace UnifiedRender::OpenGl {
    class Shader {
    protected:
        std::string buffer;
        GLuint id;
    public:
        Shader(const std::string& path);
        ~Shader();

        virtual void compile(void);
        std::string get_status(void);
        GLuint get_id(void) const;
    };

    class VertexShader : public Shader {
        static const GLuint type = GL_VERTEX_SHADER;
    public:
        VertexShader(const std::string& path) : Shader(path) {};
        virtual void compile(void);
    };

    class FragmentShader : public Shader {
        static const GLuint type = GL_FRAGMENT_SHADER;
    public:
        FragmentShader(const std::string& path) : Shader(path) {};
        virtual void compile(void);
    };

    class Program {
        GLuint id;
    public:
        Program(const VertexShader* vertex, const FragmentShader* fragment);
        void use(void) const;
        void set_uniform(const std::string& name, glm::mat4 uniform) const;
        GLuint get_id(void) const;
    };
};

#endif
