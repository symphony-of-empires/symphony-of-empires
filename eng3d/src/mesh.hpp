// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      mesh.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#   include <GL/gl.h>
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

    template<typename V = glm::vec3, typename T = glm::vec2, typename C = void>
    struct MeshData {
        constexpr static bool has_color = true;

        constexpr MeshData() = default;
        constexpr MeshData(V _vert, T _tex)
            : vert{ _vert },
            tex{ _tex }
        {

        }
        constexpr MeshData(V _vert, T _tex, C _color)
            : vert{ _vert },
            tex{ _tex },
            color{ _color }
        {

        }
        ~MeshData() = default;
        MeshData(const MeshData&) = delete;
        MeshData(MeshData&&) noexcept = default;
        MeshData& operator=(const MeshData&) = default;

        V vert;
        T tex;
        C color;
    };

    template<typename V, typename T>
    struct MeshData<V, T, void> {
        constexpr static bool has_color = false;

        constexpr MeshData() = default;
        constexpr MeshData(V _vert, T _tex)
            : vert{ _vert },
            tex{ _tex }
        {

        }
        ~MeshData() = default;
        MeshData(const MeshData&) = delete;
        MeshData(MeshData&&) noexcept = default;
        MeshData& operator=(const MeshData&) = default;

        V vert;
        T tex;
    };

    // Packed model - packs both vertices and texcoords into the same buffer
    template<typename V = glm::vec3, typename T = glm::vec2, typename C = void>
    class Mesh {
    public:
        using DataType = Eng3D::MeshData<V, T, C>;

        Mesh(enum Eng3D::MeshMode _mode)
            : mode(_mode),
            vao(),
            vbo(),
            ebo(),
            i_vbo()
        {

        }
        virtual ~Mesh() = default;
        Mesh(const Mesh&) = delete;
        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(const Mesh&) = delete;

#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
        virtual void draw(int instances = 0) const {
            vao.bind();
            if(instances) {
                // Instanced
                if(!indices.empty())
                    glDrawElementsInstanced(static_cast<GLenum>(mode), indices.size(), GL_UNSIGNED_INT, nullptr, instances);
                else if(!buffer.empty())
                    glDrawArraysInstanced(static_cast<GLenum>(mode), 0, buffer.size(), instances);
            } else {
                // Single-draw
                if(!indices.empty())
                    glDrawElements(static_cast<GLenum>(mode), indices.size(), GL_UNSIGNED_INT, nullptr);
                else if(!buffer.empty())
                    glDrawArrays(static_cast<GLenum>(mode), 0, buffer.size());
            }
        }

        virtual void draw(const Eng3D::OpenGL::Program&, int instances = 0) const {
            this->draw(instances);
        }
#else
#   error not implemented
#endif

        virtual void upload() const {
#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
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

            if constexpr(DataType::has_color) {
                // Color
                constexpr int color_stride = tex_stride + sizeof(buffer[0].tex);
                glVertexAttribPointer(2, C::length(), GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)((uintptr_t)color_stride));
                glEnableVertexAttribArray(2);
            }
#else
#   error not implemented
#endif
        }

        /// @brief Enables instances on this simple mesh
        /// @param instances Number of instances to configure for
        template<typename I = glm::vec2>
        void instancing(I& buffer, int instances = 0) {
            if(!instances) return;

            vao.bind();
            i_vbo.bind();
            glBufferData(GL_ARRAY_BUFFER, sizeof(I) * instances, &buffer, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0); 

            glEnableVertexAttribArray(2);
            i_vbo.bind();
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(I), (void*)0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);	
            glVertexAttribDivisor(2, 1);  
        }

        std::vector<DataType> buffer;
        std::vector<unsigned int> indices;
        enum Eng3D::MeshMode mode;

#if defined E3D_BACKEND_OPENGL || defined E3D_BACKEND_GLES
        // The initialization should be done in this order, first the VAO
        // then initialize the VBO!
        Eng3D::OpenGL::VAO vao;
        Eng3D::OpenGL::VBO vbo;
        Eng3D::OpenGL::EBO ebo;
        Eng3D::OpenGL::VBO i_vbo; // Instanced VBO, optional
#else
#   error not implemented
#endif
    };
}
