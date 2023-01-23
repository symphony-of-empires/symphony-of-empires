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
//      mesh.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#   include <GL/gl.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl3.h>
#elif defined E3D_BACKEND_RGX
#   include <gccore.h>
#endif

#include "eng3d/mesh.hpp"
#include "eng3d/string.hpp"

Eng3D::OpenGL::VAO::VAO(unsigned int _id)
    : id{ _id }
{

}

Eng3D::OpenGL::VAO::VAO() {
    glGenVertexArrays(1, &id);
}

Eng3D::OpenGL::VAO::~VAO() {
    glDeleteVertexArrays(1, &id);
}

Eng3D::OpenGL::VAO::VAO(Eng3D::OpenGL::VAO&& rhs) noexcept {
    this->id = rhs.id;
    rhs.id = 0;
}

Eng3D::OpenGL::VAO& Eng3D::OpenGL::VAO::operator=(Eng3D::OpenGL::VAO& rhs) {
    this->id = rhs.id;
    rhs.id = 0;
    return *this;
}

void Eng3D::OpenGL::VAO::bind() const {
    glBindVertexArray(id);
}

unsigned int Eng3D::OpenGL::VAO::get_id() const {
    return id;
}

Eng3D::OpenGL::VBO::VBO(unsigned int _id)
    : id{ _id }
{

}

Eng3D::OpenGL::VBO::VBO() {
    glGenBuffers(1, &id);
}

Eng3D::OpenGL::VBO::~VBO() {
    glDeleteBuffers(1, &id);
}

Eng3D::OpenGL::VBO::VBO(Eng3D::OpenGL::VBO&& rhs) noexcept {
    this->id = rhs.id;
    rhs.id = 0;
}

Eng3D::OpenGL::VBO& Eng3D::OpenGL::VBO::operator=(Eng3D::OpenGL::VBO& rhs) {
    this->id = rhs.id;
    rhs.id = 0;
    return *this;
}

void Eng3D::OpenGL::VBO::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

unsigned int Eng3D::OpenGL::VBO::get_id() const {
    return id;
}

Eng3D::OpenGL::EBO::EBO(unsigned int _id)
    : id{ _id }
{

}

Eng3D::OpenGL::EBO::EBO() {
    glGenBuffers(1, &id);
}

Eng3D::OpenGL::EBO::~EBO() {
    glDeleteBuffers(1, &id);
}

Eng3D::OpenGL::EBO::EBO(Eng3D::OpenGL::EBO&& rhs) noexcept {
    this->id = rhs.id;
    rhs.id = 0;
}

Eng3D::OpenGL::EBO& Eng3D::OpenGL::EBO::operator=(Eng3D::OpenGL::EBO& rhs) {
    this->id = rhs.id;
    rhs.id = 0;
    return *this;
}

void Eng3D::OpenGL::EBO::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

unsigned int Eng3D::OpenGL::EBO::get_id() const {
    return id;
}

void Eng3D::draw(const Eng3D::OpenGL::VAO& vao, MeshMode mode, const void*, size_t n_indices, const void*, size_t n_buffer, int instances) {
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
    int i_mode = 0;
    switch(mode) {
    case Eng3D::MeshMode::TRIANGLE_FAN:
        i_mode = GL_TRIANGLE_FAN;
        break;
    case Eng3D::MeshMode::TRIANGLE_STRIP:
        i_mode = GL_TRIANGLE_STRIP;
        break;
    case Eng3D::MeshMode::TRIANGLES:
        i_mode = GL_TRIANGLES;
        break;
    case Eng3D::MeshMode::LINES:
        i_mode = GL_LINES;
        break;
    case Eng3D::MeshMode::LINE_STRIP:
        i_mode = GL_LINE_STRIP;
        break;
    default:
        CXX_THROW(std::runtime_error, Eng3D::translate_format("Unknown mesh mode %i", static_cast<int>(mode)));
    }

    vao.bind();
    if(instances) {
        // Instanced
        if(n_indices)
            glDrawElementsInstanced(i_mode, n_indices, GL_UNSIGNED_INT, nullptr, instances);
        else if(n_buffer)
            glDrawArraysInstanced(i_mode, 0, n_buffer, instances);
    } else {
        // Single-draw
        if(n_indices)
            glDrawElements(i_mode, n_indices, GL_UNSIGNED_INT, nullptr);
        else if(n_buffer)
            glDrawArrays(i_mode, 0, n_buffer);
    }
#endif
}

void Eng3D::upload(const Eng3D::OpenGL::VAO& vao, const Eng3D::OpenGL::VBO& vbo, const Eng3D::OpenGL::EBO& ebo, const void *buffer, size_t n_buffer, size_t sz_buffer, size_t sz_buffer_vert, size_t sz_buffer_tex, size_t v_len, size_t t_len, size_t c_len, const void *indices, size_t n_indices, size_t sz_indices, bool has_color) {
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
    vao.bind();
    vbo.bind();
    glBufferData(GL_ARRAY_BUFFER, n_buffer * sz_buffer, buffer, GL_STATIC_DRAW);
    ebo.bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indices * sz_indices, indices, GL_STATIC_DRAW);

    // Vertices
    glVertexAttribPointer(0, v_len, GL_FLOAT, GL_FALSE, sz_buffer, nullptr);
    glEnableVertexAttribArray(0);

    // Texcoords
    size_t tex_stride = sz_buffer_vert;
    glVertexAttribPointer(1, t_len, GL_FLOAT, GL_FALSE, sz_buffer, (void*)((uintptr_t)tex_stride));
    glEnableVertexAttribArray(1);

    if(has_color) {
        // Color
        size_t color_stride = tex_stride + sz_buffer_tex;
        glVertexAttribPointer(2, c_len, GL_FLOAT, GL_FALSE, sz_buffer, (void*)((uintptr_t)color_stride));
        glEnableVertexAttribArray(2);
    }
#endif
}

void Eng3D::instancing(const Eng3D::OpenGL::VAO& vao, const Eng3D::OpenGL::VBO& i_vbo, const void *buffer, size_t index_size, int instances) {
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
    if(!instances) return;

    vao.bind();
    i_vbo.bind();
    glBufferData(GL_ARRAY_BUFFER, index_size * instances, buffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glEnableVertexAttribArray(2);
    i_vbo.bind();
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, index_size, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);	
    glVertexAttribDivisor(2, 1);
#endif
}
