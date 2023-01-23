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
#include <array>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

/// @todo We aren't deleting the OpenGL objects!!!!
namespace Eng3D::OpenGL {
    class VAO {
        unsigned int id;
    public:
        VAO(unsigned int _id);
        VAO();
        ~VAO();
        VAO(const VAO&) = delete;
        VAO(VAO&& rhs) noexcept;
        VAO& operator=(VAO& rhs);
        void bind() const;
        unsigned int get_id() const;
    };

    class VBO {
        unsigned int id;
    public:
        VBO(unsigned int _id);
        VBO();
        ~VBO();
        VBO(const VBO&) = delete;
        VBO(VBO&& rhs) noexcept;
        VBO& operator=(VBO& rhs);
        void bind() const;
        unsigned int get_id() const;
    };

    class EBO {
        unsigned int id;
    public:
        EBO(unsigned int _id);
        EBO();
        ~EBO();
        EBO(const EBO&) = delete;
        EBO(EBO&& rhs) noexcept;
        EBO& operator=(EBO& rhs);
        void bind() const;
        unsigned int get_id() const;
    };

    class Program;
}

namespace Eng3D {
    enum class MeshMode {
        TRIANGLE_FAN,
        TRIANGLE_STRIP,
        TRIANGLES,
        LINES,
        LINE_STRIP,
    };

    void draw(const Eng3D::OpenGL::VAO& vao, MeshMode mode, const void* indices, size_t n_indices, const void* buffer, size_t n_buffer, int instances);
    void upload(const Eng3D::OpenGL::VAO& vao, const Eng3D::OpenGL::VBO& vbo, const Eng3D::OpenGL::EBO& ebo, const void *buffer, size_t n_buffer, size_t sz_buffer, size_t sz_buffer_vert, size_t sz_buffer_tex, size_t v_len, size_t t_len, size_t c_len, const void *indices, size_t n_indices, size_t sz_indices, bool has_color);
    void instancing(const Eng3D::OpenGL::VAO& vao, const Eng3D::OpenGL::VBO& i_vbo, const void* buffer, size_t index_size, int instances);

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

    /// @brief Packed model - packs both vertices and texcoords into the same buffer
    template<typename V = glm::vec3, typename T = glm::vec2, typename C = void>
    struct Mesh {
        using DataType = Eng3D::MeshData<V, T, C>;

        Mesh(enum Eng3D::MeshMode _mode)
            : mode{ _mode }
        {

        }
        virtual ~Mesh() = default;
        Mesh(const Mesh&) = delete;
        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(const Mesh&) = delete;

        void draw(int instances = 0) const {
            Eng3D::draw(vao, mode, indices.data(), indices.size(), buffer.data(), buffer.size(), instances);
        }

        virtual void upload() const {
            size_t c_len = 0;
            if constexpr(DataType::has_color) c_len += C::length();
            Eng3D::upload(vao, vbo, ebo, buffer.data(), buffer.size(), sizeof(buffer[0]), sizeof(buffer[0].vert), sizeof(buffer[0].tex), V::length(), T::length(), c_len, indices.data(), indices.size(), sizeof(indices), DataType::has_color);
        }

        /// @brief Enables instances on this simple mesh
        /// @param instances_buffer Contigous buffer with instances data
        /// @param instances Number of instances to configure for
        template<typename I = glm::vec2>
        void instancing(I& instances_buffer, int instances = 0) {
            Eng3D::instancing(vao, i_vbo, &instances_buffer, sizeof(I), instances);
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

    /// @brief A static mesh with fixed number of elements
    template<std::size_t vtc_size = 0, std::size_t i_size = 0, typename V = glm::vec3, typename T = glm::vec2, typename C = void>
    struct MeshStatic {
        using DataType = Eng3D::MeshData<V, T, C>;

        MeshStatic(enum Eng3D::MeshMode _mode)
            : mode{ _mode }
        {

        }
        virtual ~MeshStatic() = default;
        MeshStatic(const MeshStatic&) = delete;
        MeshStatic(MeshStatic&&) noexcept = default;
        MeshStatic& operator=(const MeshStatic&) = delete;

        void draw(int instances = 0) const {
            Eng3D::draw(vao, mode, indices.data(), indices.size(), buffer.data(), buffer.size(), instances);
        }

        virtual void upload() const {
            size_t c_len = 0;
            if constexpr(DataType::has_color) c_len += C::length();
            Eng3D::upload(vao, vbo, ebo, buffer.data(), buffer.size(), sizeof(buffer[0]), sizeof(buffer[0].vert), sizeof(buffer[0].tex), V::length(), T::length(), c_len, indices.data(), indices.size(), sizeof(indices), DataType::has_color);
        }

        /// @brief Enables instances on this simple mesh
        /// @param instances_buffer Contigous buffer with instances data
        /// @param instances Number of instances to configure for
        template<typename I = glm::vec2>
        void instancing(I& instances_buffer, int instances = 0) {
            Eng3D::instancing(vao, i_vbo, &instances_buffer, sizeof(I), instances);
        }

        std::array<DataType, vtc_size> buffer;
        std::array<unsigned int, i_size> indices;
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
