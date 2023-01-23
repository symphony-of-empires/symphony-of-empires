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
//      shader.cpp
//
// Abstract:
//      Provides functions to use shaders/shader pipeline functions with OpenGL.
// ----------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <sstream>

#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#   include <GL/gl.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl3.h>
#   include <GLES3/gl31.h>
#endif

#include "eng3d/shader.hpp"
#include "eng3d/glsl_trans.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/log.hpp"
#include "eng3d/string.hpp"

#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
/// @brief Construct a shader by opening the provided path and creating a temporal ifstream, reading
/// from that stream in text mode and then compiling the shader
Eng3D::OpenGL::Shader::Shader(const std::string& _buffer, GLuint type, bool use_transpiler, std::vector<Eng3D::GLSL::Define> defintions)
    : buffer(_buffer)
{
    if(use_transpiler) try {
        Eng3D::GLSL::Context ctx(buffer);
        ctx.defines = defintions;
        ctx.lexer();
        ctx.parser();
        buffer = ctx.to_text();
        line_numbers = ctx.line_numbers;
    } catch(Eng3D::GLSL::Exception& e) {
        Eng3D::Log::error("shder", e.it->data + " -> " + e.what());
    }

    id = glCreateShader(type);
    if(!id)
        CXX_THROW(Eng3D::ShaderException, "Can't create shader");
    const char* c_code = buffer.c_str();
    glShaderSource(id, 1, &c_code, NULL);
    compile(type);
}

void Eng3D::OpenGL::Shader::compile(GLuint) {
    glCompileShader(id);

    // Check for errors of the shader
    GLint r = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &r);
    if(!r) {
        GLint infoLen = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLen);
        std::string shader_error_info;
        shader_error_info.resize(infoLen);
        glGetShaderInfoLog(id, infoLen, NULL, &shader_error_info[0]);

        std::ostringstream output_error;
        output_error << "Shader did not compile:\n";
        std::istringstream error_lines(shader_error_info);
        std::string error;
        getline(error_lines, error);
        while(!error_lines.eof()) {
            std::istringstream sline(error);
            // Nvidia's style of errors
            int slot, col;
            size_t row;
            char ch;

            // nvidia styles errors the following way:
            // slot:row(column)
            // They also like to fuck me over, so they use
            // slot(row) <-- fuck this
            sline >> slot >> ch;
            if(ch == ':') {
                sline >> row >> ch;
                if(ch == '(')
                    sline >> col;
            } else if(ch == '(') {
                sline >> row >> ch;
            }

            size_t read_lines = 0;
            std::stringstream code(buffer);
            std::string error_code;
            while(read_lines < row && !code.eof()) {
                getline(code, error_code);
                read_lines++;
            }

            getline(sline, error);
            if(row > 0 && row - 1 < line_numbers.size())
                row = line_numbers[row - 1];

            output_error << "(" << row << ")" << ":" << error_code;
            output_error << "\n^^^: " << error << "\n";
            getline(error_lines, error);
        }
        Eng3D::Log::error("opengl", output_error.str());
        CXX_THROW(Eng3D::ShaderException, output_error.str());
    }
    Eng3D::Log::debug("shader", "Status: Sucess");
}

/// @brief Destroy the Shader object, dditionaly we have to delete the
/// shader from the OpenGL driver by calling glDeleteShader
Eng3D::OpenGL::Shader::~Shader() {
    glDeleteShader(this->get_id());
}

unsigned int Eng3D::OpenGL::Shader::get_id() const {
    return this->id;
}

//
// Vertex shader
//
Eng3D::OpenGL::VertexShader::VertexShader(const std::string& _buffer)
    : Eng3D::OpenGL::Shader(_buffer, GL_VERTEX_SHADER)
{

}

//
// Fragment shader
//
Eng3D::OpenGL::FragmentShader::FragmentShader(const std::string& _buffer, bool use_transpiler, std::vector<Eng3D::GLSL::Define> defintions)
    : Eng3D::OpenGL::Shader(_buffer, GL_FRAGMENT_SHADER, use_transpiler, defintions)
{

}

#if !defined E3D_BACKEND_GLES
//
// Geometry shader
//
Eng3D::OpenGL::GeometryShader::GeometryShader(const std::string& _buffer)
    : Eng3D::OpenGL::Shader(_buffer, GL_GEOMETRY_SHADER)
{

}

//
// Tesseleation control shader
//
Eng3D::OpenGL::TessControlShader::TessControlShader(const std::string& _buffer)
    : Eng3D::OpenGL::Shader(_buffer, GL_TESS_CONTROL_SHADER)
{

}

//
// Tesselation evaluation shader
//
Eng3D::OpenGL::TessEvalShader::TessEvalShader(const std::string& _buffer)
    : Eng3D::OpenGL::Shader(_buffer, GL_TESS_EVALUATION_SHADER)
{

}
#endif

//
// Program shader
//
Eng3D::OpenGL::Program::Program() {
    id = glCreateProgram();
    if(!id)
        CXX_THROW(Eng3D::ShaderException, "Can't create new shader program");
    glBindAttribLocation(id, 0, "m_pos");
    glBindAttribLocation(id, 1, "m_texcoord");
}

Eng3D::OpenGL::Program::~Program() {
    glDeleteProgram(id);
}

/// @brief Links the whole program into itself, all attached shaders that were
/// previously given to this program will be linked (hopefully) by OpenGL.
void Eng3D::OpenGL::Program::link() {
    assert(id != 0); // Program has no Id
    glLinkProgram(id);

    // Check for errors of the shader
    GLint r;
    glGetProgramiv(id, GL_LINK_STATUS, &r);
    if(r != GL_TRUE) {
        GLint infoLen = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLen);
        std::string shader_error_info;
        shader_error_info.resize(infoLen);
        glGetShaderInfoLog(id, infoLen, NULL, &shader_error_info[0]);
        Eng3D::Log::error("shader", Eng3D::translate_format("Shader program error %s", shader_error_info.c_str()));
    }
}

/// @brief Attaches a shader to the program - this will make it so when the program
/// is compiled the shader will then be linked onto it and usable for the GPU.
/// @param shader Shader to attach
void Eng3D::OpenGL::Program::attach_shader(const Eng3D::OpenGL::Shader& shader) {
    glAttachShader(id, shader.get_id());
}

void Eng3D::OpenGL::Program::use() const {
    glUseProgram(id);
}

// Uniform overloads
// It allows the game engine to call these functions without worrying about type specifications
void Eng3D::OpenGL::Program::set_PVM(glm::mat4 projection, glm::mat4 view, glm::mat4 model) const {
    set_uniform("projection", projection);
    set_uniform("view", view);
    set_uniform("model", model);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, glm::mat4 uniform) const {
    glProgramUniformMatrix4fv(id, glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(uniform));
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, float value1, float value2) const {
    glProgramUniform2f(id, glGetUniformLocation(id, name.c_str()), value1, value2);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, float value1, float value2, float value3) const {
    glProgramUniform3f(id, glGetUniformLocation(id, name.c_str()), value1, value2, value3);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, glm::vec2 uniform) const {
    set_uniform(name, uniform.x, uniform.y);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, glm::vec3 uniform) const {
    set_uniform(name, uniform.x, uniform.y, uniform.z);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, glm::vec4 uniform) const {
    set_uniform(name, uniform.x, uniform.y, uniform.z, uniform.w);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, float value1, float value2, float value3, float value4) const {
    glProgramUniform4f(id, glGetUniformLocation(id, name.c_str()), value1, value2, value3, value4);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, float value) const {
    glProgramUniform1f(id, glGetUniformLocation(id, name.c_str()), value);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, int value) const {
    glProgramUniform1i(id, glGetUniformLocation(id, name.c_str()), value);
}

/// @brief Sets the texture (sampler2D) into the shader
void Eng3D::OpenGL::Program::set_texture(int value, const std::string& name, const Eng3D::Texture& texture) const {
    glActiveTexture(GL_TEXTURE0 + value);
    set_uniform(name, value);
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

void Eng3D::OpenGL::Program::set_texture(int value, const std::string& name, const Eng3D::TextureArray& texture) const {
    glActiveTexture(GL_TEXTURE0 + value);
    set_uniform(name, value);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture.id);
}

unsigned int Eng3D::OpenGL::Program::get_id() const {
    return this->id;
}
#endif
