// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include "eng3d/texture.hpp"
#include "eng3d/glsl_trans.hpp"
#include "eng3d/utils.hpp"

namespace Eng3D {
    class ShaderException : public std::exception {
        std::string buffer;
    public:
        ShaderException(const std::string_view _buffer)
            : buffer{ _buffer }
        {

        }
        
        virtual const char* what() const noexcept {
            return buffer.data();
        }
    };

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
            void compile(unsigned int type);
            std::string buffer;
            unsigned int id;
            std::vector<int> line_numbers;
        public:
            Shader(const std::string_view _buffer, unsigned int type, bool use_transpiler = true, std::vector<Eng3D::GLSL::Define> defintions = {});
            ~Shader();
            unsigned int get_id() const;
        };

        class VertexShader: public Shader {
        public:
            VertexShader(const std::string_view _buffer);
            ~VertexShader() = default;
        };

        class FragmentShader: public Shader {
        public:
            FragmentShader(const std::string_view _buffer, bool use_transpiler = true, std::vector<Eng3D::GLSL::Define> defintions = {});
            ~FragmentShader() = default;
        };

#if !defined E3D_BACKEND_GLES
        class GeometryShader: public Shader {
        public:
            GeometryShader(const std::string_view _buffer);
            ~GeometryShader() = default;
        };

        class TessControlShader: public Shader {
        public:
            TessControlShader(const std::string_view _buffer);
            ~TessControlShader() = default;
        };

        class TessEvalShader: public Shader {
        public:
            TessEvalShader(const std::string_view _buffer);
            ~TessEvalShader() = default;
        };
#endif

        class Program {
            unsigned int id;
        public:
            Program();
            ~Program();
            void link();
            void attach_shader(const Eng3D::OpenGL::Shader& shader);
            void use() const;
            void set_PVM(glm::mat4 projection, glm::mat4 view, glm::mat4 model) const;
            void set_uniform(const std::string_view name, glm::mat4 uniform) const;
            void set_uniform(const std::string_view name, float value1, float value2) const;
            void set_uniform(const std::string_view name, float value1, float value2, float value3) const;
            void set_uniform(const std::string_view name, glm::vec2 uniform) const;
            void set_uniform(const std::string_view name, glm::vec3 uniform) const;
            void set_uniform(const std::string_view name, glm::vec4 uniform) const;
            void set_uniform(const std::string_view name, float value1, float value2, float value3, float value4) const;
            void set_uniform(const std::string_view name, float value) const;
            void set_uniform(const std::string_view name, int value) const;
            void set_texture(int value, const std::string_view name, const Eng3D::Texture& texture) const;
            void set_texture(int value, const std::string_view name, const Eng3D::TextureArray& texture) const;
            unsigned int get_id() const;
        };
    }
}
