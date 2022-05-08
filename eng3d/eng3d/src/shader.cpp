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
//      shader.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <sstream>

#include "eng3d/shader.hpp"
#include "eng3d/glsl_trans.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/assert.hpp"
#include "eng3d/log.hpp"

// Construct a shader by opening the provided path and creating a temporal ifstream, reading
// from that stream in text mode and then compiling the shader
Eng3D::OpenGL::Shader::Shader(const std::string& _buffer, GLuint type, bool use_transpiler, std::vector<Eng3D::OpenGL::GLSL_Define> defintions)
    : buffer(_buffer)
{
    if(use_transpiler) {
        Eng3D::OpenGL::GLSL_Context ctx(buffer);
        ctx.defines = defintions;
        ctx.lexer();
        try {
            ctx.parser();
        } catch(Eng3D::OpenGL::GLSL_Exception& e) {
            print_error("%s -> %s", e.it->data.c_str(), e.what());
        }
        buffer = ctx.to_text();
    }

    id = glCreateShader(type);
    if(!id) {
        CXX_THROW(Eng3D::OpenGL::ShaderException, "Can't create shader");
    }
    const char* c_code = buffer.c_str();
    glShaderSource(id, 1, &c_code, NULL);
    compile(type);
}

#include <sstream>
void Eng3D::OpenGL::Shader::compile(GLuint type) {
    glCompileShader(id);

    // Check for errors of the shader
    GLint r = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &r);
    if(!r) {
        GLchar* error_info;
        glGetShaderInfoLog(id, GL_INFO_LOG_LENGTH, NULL, error_info);
        Eng3D::Log::error("opengl", error_info);

        // Nvidia's style of errors
        std::istringstream sline(error_info);
        int slot, row, col;
        char ch;

        // nvidia styles errors the following way:
        // slot:row(column)
        // They also like to fuck me over, so they use
        // slot(row) <-- fuck this
        sline >> slot >> ch;
        if(ch == ':') {
            sline >> row >> ch;
            if(ch == '(') {
                sline >> col;
            }
        } else if(ch == '(') {
            sline >> row;
        }

        // Now we will pray that this actually worked

        // Next we will read over the shader file and find the line
        int row_rem = row;
        std::string::iterator it;
        for(it = buffer.begin(); it != buffer.end() && row_rem; it++) {
            if(*it == '\n') {
                row_rem--;
                if(!row_rem) {
                    break;
                }
            }
        }

        std::string line_buf = "(No line info)";
        if(it != buffer.end()) {
            line_buf = buffer.substr(std::distance(buffer.begin(), it), buffer.find_first_of('\n', std::distance(buffer.begin(), it)));
        }
        CXX_THROW(Eng3D::OpenGL::ShaderException, line_buf + "\n" + error_info);
    }
    // print_info("Status: Sucess");
}

GLuint Eng3D::OpenGL::Shader::get_id(void) const {
    return id;
}

// Deconstructs the shader object and we have to delete the shader from the OpenGL
// driver by calling glDeleteShader
Eng3D::OpenGL::Shader::~Shader() {
    if(id) {
        glDeleteShader(id);
    }
}

//
// Vertex shader
//
Eng3D::OpenGL::VertexShader::VertexShader(const std::string& _buffer)
    : Shader(_buffer, GL_VERTEX_SHADER)
{

}

Eng3D::OpenGL::VertexShader::~VertexShader(void) {

}

//
// Fragment shader
//
Eng3D::OpenGL::FragmentShader::FragmentShader(const std::string& _buffer, bool use_transpiler, std::vector<Eng3D::OpenGL::GLSL_Define> defintions)
    : Shader(_buffer, GL_FRAGMENT_SHADER, use_transpiler, defintions)
{

}

Eng3D::OpenGL::FragmentShader::~FragmentShader(void) {

}

//
// Geometry shader
//
Eng3D::OpenGL::GeometryShader::GeometryShader(const std::string& _buffer)
    : Shader(_buffer, GL_GEOMETRY_SHADER)
{

}

Eng3D::OpenGL::GeometryShader::~GeometryShader(void) {

}

//
// Tessellation control shader
//
Eng3D::OpenGL::TessControlShader::TessControlShader(const std::string& _buffer)
    : Shader(_buffer, GL_TESS_CONTROL_SHADER)
{

}

Eng3D::OpenGL::TessControlShader::~TessControlShader(void) {

}

//
// Tessellation evaluation shader
//
Eng3D::OpenGL::TessEvalShader::TessEvalShader(const std::string& _buffer)
    : Shader(_buffer, GL_TESS_EVALUATION_SHADER)
{

}

Eng3D::OpenGL::TessEvalShader::~TessEvalShader(void) {

}

//
// Program
//
Eng3D::OpenGL::Program::Program(void) {
    id = glCreateProgram();
    if(!id) {
        CXX_THROW(Eng3D::OpenGL::ShaderException, "Can't create new program");
    }
    glBindAttribLocation(id, 0, "m_pos");
    glBindAttribLocation(id, 1, "m_texcoord");
}

Eng3D::OpenGL::Program::~Program(void) {

}

// Attaches a shader to the program - this will make it so when the program is compiled the shader
// will then be linked onto it
void Eng3D::OpenGL::Program::attach_shader(const Eng3D::OpenGL::Shader* shader) {
    glAttachShader(id, shader->get_id());
}

void Eng3D::OpenGL::Program::link(void) {
    debug_assert(id != 0); // Program has no Id
    glLinkProgram(id);

    // Check for errors of the shader
    GLint r = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &r);
    if(!r) {
        std::string error_info;
        glGetProgramInfoLog(id, GL_INFO_LOG_LENGTH, NULL, &error_info[0]);
        print_error("Program error %s", error_info.c_str());
        CXX_THROW(Eng3D::OpenGL::ShaderException, error_info);
    }
}

void Eng3D::OpenGL::Program::use(void) const {
    glUseProgram(id);
}

// Uniform overloads
// It allows the game engine to call these functions without worrying about type specifications
void Eng3D::OpenGL::Program::set_uniform(const std::string& name, glm::mat4 uniform) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(uniform));
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, float value1, float value2) const {
    glUniform2f(glGetUniformLocation(id, name.c_str()), value1, value2);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, float value1, float value2, float value3) const {
    glUniform3f(glGetUniformLocation(id, name.c_str()), value1, value2, value3);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, glm::vec3 uniform) const {
    set_uniform(name, uniform.x, uniform.y, uniform.z);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, glm::vec4 uniform) const {
    set_uniform(name, uniform.x, uniform.y, uniform.z, uniform.w);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, float value1, float value2, float value3, float value4) const {
    glUniform4f(glGetUniformLocation(id, name.c_str()), value1, value2, value3, value4);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Eng3D::OpenGL::Program::set_uniform(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

// Sets the texture (sampler2D) into the shader,
void Eng3D::OpenGL::Program::set_texture(int value, const std::string& name, const Eng3D::Texture& texture) const {
    debug_assert(texture.gl_tex_num != 0); // Texture with invalid Id

    glActiveTexture(GL_TEXTURE0 + value);
    set_uniform(name, value);
    glBindTexture(GL_TEXTURE_2D, texture.gl_tex_num);
}

void Eng3D::OpenGL::Program::set_texture(int value, const std::string& name, const Eng3D::TextureArray& texture) const {
    debug_assert(texture.gl_tex_num != 0); // Texture with invalid Id

    glActiveTexture(GL_TEXTURE0 + value);
    set_uniform(name, value);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture.gl_tex_num);
}

GLuint Eng3D::OpenGL::Program::get_id(void) const {
    return id;
}
