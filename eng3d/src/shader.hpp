// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      shader.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cassert>
#include <string>
#include <exception>

#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#   include <GL/gl.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl3.h>
#endif

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/glsl_trans.hpp"
#include "eng3d/utils.hpp"

namespace Eng3D {
    class ShaderException : public std::exception {
        std::string buffer;
    public:
        ShaderException(const std::string& _buffer): buffer(_buffer) {};
        virtual const char* what() const noexcept {
            return buffer.c_str();
        }
    };

#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
    namespace OpenGL {
        /// @brief Option that is passed to the GLSL transpiler for preprocessing
        /// the shaders and programming them on the fly.
        class Option {
            std::string _option;
        public:
            bool used = false;

            Option() = default;
            Option(std::string option, bool use)
                : _option{ option },
                used{ use }
            {

            }

            std::string get_option() const {
                return _option;
            }
        };

        /// @brief OpenGL shader object
        class Shader {
        private:
            void compile(GLuint type);
            std::string buffer;
            GLuint id;
            std::vector<int> line_numbers;
        public:
            Shader(const std::string& _buffer, GLuint type, bool use_transpiler = true, std::vector<Eng3D::GLSL::Define> defintions = {});
            
            /// @brief Destroy the Shader object, dditionaly we have to delete the
            /// shader from the OpenGL driver by calling glDeleteShader
            inline ~Shader() {
                if(id)
                    glDeleteShader(id);
            }

            inline GLuint get_id() const {
                return id;
            }
        };

        class VertexShader: public Shader {
        public:
            VertexShader(const std::string& _buffer) : Shader(_buffer, GL_VERTEX_SHADER) {};
            ~VertexShader() = default;
        };

        class FragmentShader: public Shader {
        public:
            FragmentShader(const std::string& _buffer, bool use_transpiler = true, std::vector<Eng3D::GLSL::Define> defintions = {});
            ~FragmentShader() = default;
        };

#if !defined E3D_BACKEND_GLES
        class GeometryShader: public Shader {
        public:
            GeometryShader(const std::string& _buffer) : Shader(_buffer, GL_GEOMETRY_SHADER) {};
            ~GeometryShader() = default;
        };

        class TessControlShader: public Shader {
        public:
            TessControlShader(const std::string& _buffer) : Shader(_buffer, GL_TESS_CONTROL_SHADER) {};
            ~TessControlShader() = default;
        };

        class TessEvalShader: public Shader {
        public:
            TessEvalShader(const std::string& _buffer) : Shader(_buffer, GL_TESS_EVALUATION_SHADER) {};
            ~TessEvalShader() = default;
        };
#endif

        class Program {
            GLuint id;
        public:
            Program() {
                id = glCreateProgram();
                if(!id)
                    CXX_THROW(Eng3D::ShaderException, "Can't create new program");
                glBindAttribLocation(id, 0, "m_pos");
                glBindAttribLocation(id, 1, "m_texcoord");
            }

            ~Program() {
                glDeleteProgram(id);
            }

            void link();

            /// @brief Attaches a shader to the program - this will make it so when the program
            /// is compiled the shader will then be linked onto it and usable for the GPU.
            /// @param shader Shader to attach
            inline void attach_shader(const Shader& shader) {
                glAttachShader(id, shader.get_id());
            }

            inline void use() const {
                glUseProgram(id);
            }

            // Uniform overloads
            // It allows the game engine to call these functions without worrying about type specifications
            inline void set_uniform(const std::string& name, glm::mat4 uniform) const {
                glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(uniform));
            }

            inline void set_uniform(const std::string& name, float value1, float value2) const {
                glUniform2f(glGetUniformLocation(id, name.c_str()), value1, value2);
            }

            inline void set_uniform(const std::string& name, float value1, float value2, float value3) const {
                glUniform3f(glGetUniformLocation(id, name.c_str()), value1, value2, value3);
            }

            inline void set_uniform(const std::string& name, glm::vec2 uniform) const {
                set_uniform(name, uniform.x, uniform.y);
            }

            inline void set_uniform(const std::string& name, glm::vec3 uniform) const {
                set_uniform(name, uniform.x, uniform.y, uniform.z);
            }

            inline void set_uniform(const std::string& name, glm::vec4 uniform) const {
                set_uniform(name, uniform.x, uniform.y, uniform.z, uniform.w);
            }

            inline void set_uniform(const std::string& name, float value1, float value2, float value3, float value4) const {
                glUniform4f(glGetUniformLocation(id, name.c_str()), value1, value2, value3, value4);
            }

            inline void set_uniform(const std::string& name, float value) const {
                glUniform1f(glGetUniformLocation(id, name.c_str()), value);
            }

            inline void set_uniform(const std::string& name, int value) const {
                glUniform1i(glGetUniformLocation(id, name.c_str()), value);
            }

            /// @brief Sets the texture (sampler2D) into the shader,
            inline void set_texture(int value, const std::string& name, const Eng3D::Texture& texture) const {
                glActiveTexture(GL_TEXTURE0 + value);
                set_uniform(name, value);
                glBindTexture(GL_TEXTURE_2D, texture.gl_tex_num);
            }

            inline void set_texture(int value, const std::string& name, const Eng3D::TextureArray& texture) const {
                glActiveTexture(GL_TEXTURE0 + value);
                set_uniform(name, value);
                glBindTexture(GL_TEXTURE_2D_ARRAY, texture.gl_tex_num);
            }

            inline GLuint get_id() const {
                return id;
            }
        };
    };
#endif
};
