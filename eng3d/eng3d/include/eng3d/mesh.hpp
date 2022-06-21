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
//      mesh.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#   include <GL/gl.h>
#   include <GL/glu.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl3.h>
#elif defined E3D_BACKEND_RGX
#   include <gccore.h>
#endif

/// @todo We aren't deleting the OpenGL objects!!!!
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
namespace Eng3D::OpenGL {
    class VAO {
        GLuint id;
    public:
        VAO(GLuint _id)
            : id{ _id }
        {

        }

        VAO() {
            glGenVertexArrays(1, &id);
        }

        ~VAO() {
            glDeleteVertexArrays(1, &id);
        }

        VAO(const VAO&) = delete;

        VAO(VAO&& rhs) noexcept {
            this->id = rhs.id;
            rhs.id = 0;
        }

        VAO& operator=(VAO& rhs) {
            this->id = rhs.id;
            rhs.id = 0;
            return *this;
        }

        inline void bind() const {
            glBindVertexArray(id);
        }

        inline GLuint get_id() const {
            return id;
        }
    };

    class VBO {
        GLuint id;
    public:
        VBO(GLuint _id)
            : id{ _id }
        {

        }

        VBO() {
            glGenBuffers(1, &id);
        }

        ~VBO() {
            glDeleteBuffers(1, &id);
        }

        VBO(const VBO&) = delete;

        VBO(VBO&& rhs) noexcept {
            this->id = rhs.id;
            rhs.id = 0;
        }

        VBO& operator=(VBO& rhs) {
            this->id = rhs.id;
            rhs.id = 0;
            return *this;
        }

        inline void bind() const {
            glBindBuffer(GL_ARRAY_BUFFER, id);
        }

        inline GLuint get_id() const {
            return id;
        }
    };

    class EBO {
        GLuint id;
    public:
        EBO(GLuint _id)
            : id{ _id }
        {

        }

        EBO() {
            glGenBuffers(1, &id);
        }

        ~EBO() {
            glDeleteBuffers(1, &id);
        }

        EBO(const EBO&) = delete;

        EBO(EBO&& rhs) noexcept {
            this->id = rhs.id;
            rhs.id = 0;
        }

        EBO& operator=(EBO& rhs) {
            this->id = rhs.id;
            rhs.id = 0;
            return *this;
        }

        inline void bind() const {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        }

        inline GLuint get_id() const {
            return id;
        }
    };

    class Program;
};
#endif

namespace Eng3D {
    enum class MeshMode {
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
        TRIANGLE_FAN = GL_TRIANGLE_FAN,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
        TRIANGLES = GL_TRIANGLES,
        LINES = GL_LINES,
#elif defined E3D_BACKEND_RGX
        TRIANGLE_FAN = 1,
        TRIANGLE_STRIP = 2,
        TRIANGLES = 3,
        LINES = 4,
#endif
    };

    template<typename V = glm::vec3, typename T = glm::vec2>
    class MeshData {
    public:
        MeshData() = default;
        MeshData(V _vert, T _tex): vert(_vert), tex(_tex) {};
        ~MeshData() = default;
        MeshData(const MeshData&) = delete;
        MeshData(MeshData&&) noexcept = default;
        MeshData& operator=(const MeshData&) = default;

        V vert;
        T tex;
    };

    // Packed model - packs both vertices and texcoords into the same buffer
    template<typename V = glm::vec3, typename T = glm::vec2>
    class Mesh {
    public:
        Mesh(enum Eng3D::MeshMode _mode)
            : mode(_mode),
            vao(),
            vbo(),
            ebo()
        {

        }
        virtual ~Mesh() = default;
        Mesh(const Mesh&) = delete;
        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(const Mesh&) = delete;

#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
        virtual void draw() const {
            vao.bind();
            if(!indices.empty()) {
                glDrawElements(static_cast<GLenum>(mode), indices.size(), GL_UNSIGNED_INT, 0);
            } else if(!buffer.empty()) {
                glDrawArrays(static_cast<GLenum>(mode), 0, buffer.size());
            }
        }

        virtual void draw(const Eng3D::OpenGL::Program& program) const {
            this->draw();
        }
#else
#   error not implemented
#endif

#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
        virtual void upload() const {
            vao.bind();
            vbo.bind();
            glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(buffer[0]), &buffer[0], GL_STATIC_DRAW);
            ebo.bind();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

            // Vertices
            glVertexAttribPointer(0, V::length(), GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)0);
            glEnableVertexAttribArray(0);

            // Texcoords
            constexpr int tex_stride = sizeof(buffer[0].vert);
            glVertexAttribPointer(1, T::length(), GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)((uintptr_t)tex_stride));
            glEnableVertexAttribArray(1);
        };
#else
#   error not implemented
#endif

        std::vector<Eng3D::MeshData<V, T>> buffer;
        std::vector<unsigned int> indices;
        enum Eng3D::MeshMode mode;

#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
        // The initialization should be done in this order, first the VAO
        // then initialize the VBO!
        Eng3D::OpenGL::VAO vao;
        Eng3D::OpenGL::VBO vbo;
        Eng3D::OpenGL::EBO ebo;
#else
#   error not implemented
#endif
    };
};
