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

#ifdef E3D_BACKEND_OPENGL
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
            Eng3D::Log::error("shder", e.it->data + " -> " + e.what());
        }
        buffer = ctx.to_text();
    }

    id = glCreateShader(type);
    if(!id) {
        CXX_THROW(Eng3D::ShaderException, "Can't create shader");
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
        CXX_THROW(Eng3D::ShaderException, line_buf + "\n" + error_info);
    }
    Eng3D::Log::debug("shader", "Status: Sucess");
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

/**
 * @brief Links the whole program into itself, all attached shaders that were
 * previously given to this program will be linked (hopefully) by OpenGL.
 *
 */
void Eng3D::OpenGL::Program::link(void) {
    assert(id != 0); // Program has no Id
    glLinkProgram(id);

    // Check for errors of the shader
    GLint r = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &r);
    if(!r) {
        std::string error_info;
        glGetProgramInfoLog(id, GL_INFO_LOG_LENGTH, NULL, &error_info[0]);
        Eng3D::Log::error("shader", std::string() + "Program error " + error_info.c_str());
        CXX_THROW(Eng3D::ShaderException, error_info);
    }
}

#endif
