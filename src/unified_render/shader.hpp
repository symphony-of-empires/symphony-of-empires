#pragma once

#include <string>
#include <exception>

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"

namespace UnifiedRender::OpenGl {
    class ShaderException : public std::exception {
        std::string buffer;
    public:
        ShaderException(const std::string& _buffer) : buffer(_buffer) {};
        virtual const char* what(void) const noexcept {
            return buffer.c_str();
        }
    };

    class Shader {
    private:        
        void compile(GLuint type);
        std::string buffer;
        GLuint id;
    public:
        Shader(const std::string& path, GLuint type);
        ~Shader();

        GLuint get_id(void) const;
    };

    class VertexShader : public Shader {
    public:
        VertexShader(const std::string& path);
    };

    class FragmentShader : public Shader {
    public:
        FragmentShader(const std::string& path);
    };

    class GeometryShader : public Shader {
    public:
        GeometryShader(const std::string& path);
    };

    class TessControlShader : public Shader {
    public:
        TessControlShader(const std::string& path);
    };

    class TessEvalShader : public Shader {
    public:
        TessEvalShader(const std::string& path);
    };

    class Program {
        GLuint id;
    public:
        Program(const VertexShader* vertex, const FragmentShader* fragment, const GeometryShader* geometry = nullptr, const TessControlShader* tctrl = nullptr, const TessEvalShader* tee = nullptr);
        static Program* create(const std::string& vs_path, const std::string& fs_path, const std::string& gs_path = "");

        void attach_shader(const Shader* shader);
        void link(void);

        void use(void) const;
        void set_uniform(const std::string& name, glm::mat4 uniform) const;
        void set_uniform(const std::string& name, float value1, float value2) const;
        void set_uniform(const std::string& name, float value1, float value2, float value3) const;
        void set_uniform(const std::string& name, float value1, float value2, float value3, float value4) const;
        void set_uniform(const std::string& name, float uniform) const;
        void set_uniform(const std::string& name, int uniform) const;

        void set_texture(int value, const std::string& name, const UnifiedRender::Texture& texture) const;
        void set_texture(int value, const std::string& name, const UnifiedRender::TextureArray& texture) const;
        
        GLuint get_id(void) const;
    };
};