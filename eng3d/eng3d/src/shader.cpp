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
        line_numbers = ctx.line_numbers;
    }

    id = glCreateShader(type);
    if(!id)
        CXX_THROW(Eng3D::ShaderException, "Can't create shader");
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
                sline >> row >> ch;
            }

            int read_lines = 0;
            std::stringstream code(buffer);
            std::string error_code;
            while(read_lines < row && !code.eof()) {
                getline(code, error_code);
                read_lines++;
            }

            getline(sline, error);
            if(row > 0 && row - 1 < line_numbers.size()) {
                row = line_numbers[row - 1];
            }

            output_error << "(" << row << ")" << ":" << error_code;
            output_error << "\n^^^: " << error << "\n";
            getline(error_lines, error);
        }
        Eng3D::Log::error("opengl", output_error.str());
        CXX_THROW(Eng3D::ShaderException, output_error.str());
    }
    Eng3D::Log::debug("shader", "Status: Sucess");
}

//
// Fragment shader
//
Eng3D::OpenGL::FragmentShader::FragmentShader(const std::string& _buffer, bool use_transpiler, std::vector<Eng3D::OpenGL::GLSL_Define> defintions)
    : Shader(_buffer, GL_FRAGMENT_SHADER, use_transpiler, defintions)
{

}

/**
 * @brief Links the whole program into itself, all attached shaders that were
 * previously given to this program will be linked (hopefully) by OpenGL.
 *
 */
void Eng3D::OpenGL::Program::link() {
    assert(id != 0); // Program has no Id
    glLinkProgram(id);

    // Check for errors of the shader
    GLint r = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &r);
    if(!r) {
        std::unique_ptr<GLchar[]> error_info(new GLchar[GL_INFO_LOG_LENGTH]);
        glGetShaderInfoLog(id, GL_INFO_LOG_LENGTH, NULL, error_info.get());
        Eng3D::Log::error("shader", std::string() + "Program error " + error_info.get());
        CXX_THROW(Eng3D::ShaderException, error_info.get());
    }
}

#endif
