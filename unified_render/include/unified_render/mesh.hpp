// Unified Render - General purpouse game engine
// Copyright (C) 2021, Unified Render contributors
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

#ifdef UR_BACKEND_OPENGL
#   include <GL/glew.h>
#   include <GL/gl.h>
#   include <GL/glu.h>
#endif

#ifdef UR_BACKEND_OPENGL
namespace UnifiedRender::OpenGL {
    class VAO {
        GLuint id;
    public:
        VAO();
        ~VAO();
        VAO(const VAO&) = default;
        VAO(VAO&&) noexcept = default;
        VAO& operator=(const VAO&) = default;

        void bind(void) const;
        GLuint get_id(void) const;
    };

    class VBO {
        GLuint id;
    public:
        VBO();
        ~VBO();
        VBO(const VBO&) = default;
        VBO(VBO&&) noexcept = default;
        VBO& operator=(const VBO&) = default;

        void bind(GLenum target = GL_ARRAY_BUFFER) const;
        GLuint get_id(void) const;
    };
};
#endif

namespace UnifiedRender {
    enum class MeshMode  {
#ifdef UR_BACKEND_OPENGL
        TRIANGLE_FAN = GL_TRIANGLE_FAN,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
        TRIANGLES = GL_TRIANGLES,
        LINES = GL_LINES,
#endif
    };

    template<typename V, typename T>
    class MeshData {
    public:
        MeshData(void) {};
        MeshData(V _vert, T _tex): vert(_vert), tex(_tex) {};
        ~MeshData() {};
        MeshData(const MeshData&) = default;
        MeshData(MeshData&&) noexcept = default;
        MeshData& operator=(const MeshData&) = default;

        V vert;
        T tex;
    };

    // Packed model - packs both vertices and texcoords into the same buffer
    template<typename V, typename T>
    class Mesh {
    public:
        Mesh(enum UnifiedRender::MeshMode _mode) : mode(_mode) {};
        virtual ~Mesh() {};
        Mesh(const Mesh&) = default;
        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(const Mesh&) = default;

#ifdef UR_BACKEND_OPENGL
        virtual void draw(void) const {
            vao.bind();
            glDrawArrays(static_cast<GLenum>(mode), 0, buffer.size());
        };
#endif

#ifdef UR_BACKEND_OPENGL
        virtual void upload(void) const {
            if(buffer.empty()) {
                return;
            }

            vao.bind();
            vbo.bind(GL_ARRAY_BUFFER);
            glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(buffer[0]), &buffer[0], GL_STATIC_DRAW);

            // Vertices
            glVertexAttribPointer(0, V::length(), GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)0);
            glEnableVertexAttribArray(0);

            // Texcoords
            const int tex_stride = ((int)V::length() * (int)sizeof(float));
            glVertexAttribPointer(1, T::length(), GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (void*)((uintptr_t)tex_stride));
            glEnableVertexAttribArray(1);
        };
#endif

        std::vector<UnifiedRender::MeshData<V, T>> buffer;
        enum UnifiedRender::MeshMode mode;
#ifdef UR_BACKEND_OPENGL
        // The initialization should be done in this order, first the VAO
        // then initialize the VBO!
        UnifiedRender::OpenGL::VAO vao;
        UnifiedRender::OpenGL::VBO vbo;
#endif
    };
};
