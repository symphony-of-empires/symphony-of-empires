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
//      serializer.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <numeric>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <type_traits>
#include <bitset>
#include <limits>
#include <concepts>
#include <glm/glm.hpp>
#include "eng3d/utils.hpp"
#include "eng3d/string.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/freelist.hpp"

namespace Eng3D::Deser {
    /// @brief The purpouse of the serializer is to serialize objects onto a byte stream
    /// that can be transfered onto the disk or over the network. Should the object have
    /// any pointers - they would need to be converted to indexes accordingly for
    /// proper transmission.
    class Exception : public std::exception {
        ::std::string buffer;
    public:
        Exception(const std::string_view msg) {
            buffer = msg;
        }
        virtual const char* what() const noexcept {
            return buffer.data();
        }
    };

    /// @brief Base class that serves as archiver, stores (in memory) the data required for
    /// serialization/deserialization
    struct Archive {
        Archive() = default;
        ~Archive() = default;
        void to_file(const std::string_view path);
        void from_file(const std::string_view path);
        void copy_to(void* ptr, size_t size);
        void copy_from(const void* ptr, size_t size);

        inline void expand(size_t amount) {
            buffer.resize(buffer.size() + amount);
        }

        inline void end_stream() {
            buffer.shrink_to_fit();
        }

        inline void rewind() {
            ptr = 0;
        }

        inline const void* get_buffer() {
            return static_cast<const void*>(&buffer[0]);
        }
        
        inline void set_buffer(const void* buf, size_t size) {
            buffer.resize(size);
            ::std::memcpy(buffer.data(), buf, size);
        }

        inline size_t size() {
            return buffer.size();
        }

        ::std::vector<uint8_t> buffer;
        size_t ptr = 0;
    };

    template<bool is_const, typename T>
    struct CondConstType;
    template<typename T>
    struct CondConstType<true, T> { using type = const ::std::remove_reference_t<T>; };
    template<typename T>
    struct CondConstType<false, T> { using type = ::std::remove_reference_t<T>; };

    /// @brief A serializer (base class) which can be used to serialize objects
    /// and create per-object optimized classes
    template<typename T>
    struct Serializer {
        template<bool is_const>
        using type = typename CondConstType<is_const, T>::type;
#ifdef DEBUG_SERIALIZER
        template<bool is_serialize = true>
        static inline void deser_dynamic(Eng3D::Deser::Archive&, T&&) {
            CXX_THROW(Eng3D::Deser::Exception, "Implement your serializer function!");
        }

        template<bool is_serialize = false>
        static inline void deser_dynamic(Eng3D::Deser::Archive&, const T&&) {
            CXX_THROW(Eng3D::Deser::Exception, "Implement your deserializer function!");
        }
#endif
    };

    /// @brief Template generic (de)-serializer
    /// @tparam is_serialize if true perform serialize mode, otherwise deserialize
    /// @tparam T the type to (de)-serialize
    template<bool is_serialize, typename T>
    void deser_dynamic(Eng3D::Deser::Archive& ar, T& obj);

    template<bool is_serialize = true, typename T>
    void deser_dynamic(Eng3D::Deser::Archive& ar, const T& obj) {
        Serializer<::std::remove_reference_t<T>>::template deser_dynamic<is_serialize>(ar, obj);
    }

    template<bool is_serialize = false, typename T>
    void deser_dynamic(Eng3D::Deser::Archive& ar, T& obj) {
        Serializer<::std::remove_reference_t<T>>::template deser_dynamic<is_serialize>(ar, obj);
    }

    template<typename T>
    inline void serialize(Eng3D::Deser::Archive& ar, const T& obj) {
        Serializer<::std::remove_reference_t<T>>::template deser_dynamic<true>(ar, obj);
    }

    template<typename T>
    inline void serialize(Eng3D::Deser::Archive& ar, T& obj) {
        Serializer<::std::remove_reference_t<T>>::template deser_dynamic<true>(ar, obj);
    }

    template<typename T>
    inline void deserialize(Eng3D::Deser::Archive& ar, T& obj) {
        Serializer<::std::remove_reference_t<T>>::template deser_dynamic<false>(ar, obj);
    }

    /// @brief A serializer optimized to memcpy directly the element into the byte stream
    /// use only when the object can be copied without modification (i.e a class full of ints)
    /// The elements must have a fixed size for this to work.
    template<typename T>
    struct SerializerMemcpy {
        template<bool is_const>
        using type = typename CondConstType<is_const, T>::type;

        template<bool is_serialize>
        static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
            if constexpr(is_serialize) ar.copy_from(&obj, sizeof(T));
            else ar.copy_to(&obj, sizeof(T));
        }
    };

    template<typename T>
    concept SerializerScalar = std::is_integral_v<T> || std::is_floating_point_v<T>;
    template<SerializerScalar T>
    class Serializer<T> {
        constexpr static auto scaling = 1000.f;
    public:
        template<bool is_const>
        using type = typename CondConstType<is_const, T>::type;

        template<bool is_serialize>
        static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
            if constexpr(std::is_floating_point_v<T>) {
                auto tmp = static_cast<int32_t>(obj * scaling);
                Eng3D::Deser::deser_dynamic<is_serialize>(ar, tmp);
                if constexpr(!is_serialize)
                    obj = static_cast<T>(tmp) * (1.f / scaling);
            } else {
                if constexpr(is_serialize && ::std::endian::native == ::std::endian::big)
                    obj = ::std::byteswap<T>(obj);
                SerializerMemcpy<T>::template deser_dynamic<is_serialize>(ar, obj);
                if constexpr(!is_serialize && ::std::endian::native == ::std::endian::big)
                    obj = ::std::byteswap<T>(obj);
            }
        }
    };

    template<typename T>
    concept SerializerContainer = requires(T a, T b) {
#if defined(__GNUC__) && !defined(__clang__) && !defined(__llvm__)
        requires std::destructible<typename T::value_type>;
        requires std::forward_iterator<typename T::iterator>;
#endif
        { a.begin() } -> std::same_as<typename T::iterator>;
        { a.end() } -> std::same_as<typename T::iterator>;
    };
    /// @brief Non-contigous serializer for STL containers
    /// This serializer class works primarly with containers whose memory is contiguous
    template<SerializerContainer T>
    struct Serializer<T> {
        static constexpr auto max_elements = 163550 * 32;

        template<bool is_const>
        using type = typename CondConstType<is_const, T>::type;

        template<bool is_serialize>
        static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj_group) {
            uint32_t len = obj_group.size();
            Eng3D::Deser::deser_dynamic<is_serialize>(ar, len);
            if(!len) return; // Early exit iff nothing to do

            if(len >= max_elements)
                CXX_THROW(Eng3D::Deser::Exception, "Exceeded max element count");

            if constexpr(is_serialize) {
                for(auto& obj : obj_group)
                    Eng3D::Deser::deser_dynamic<true>(ar, obj);
            } else {
                if constexpr(requires(T a) { a.clear(); })
                    obj_group.clear();
                
                // No insert means this is a static array of some sort, ::std::array perhaps?
                constexpr bool has_insert = requires(T a, typename T::value_type tp) { a.insert(tp); };
                constexpr bool has_resize = requires(T a, size_t n) { a.resize(n); };
                if constexpr(!has_insert && !has_resize) {
                    for(decltype(len) i = 0; i < len; i++)
                        Eng3D::Deser::deser_dynamic<false>(ar, obj_group[i]);
                } else {
                    if constexpr(has_resize) {
                        obj_group.resize(len);
                        for(decltype(len) i = 0; i < len; i++)
                            Eng3D::Deser::deser_dynamic<false>(ar, obj_group[i]);
                    } else { // non-len, no resize
                        for(decltype(len) i = 0; i < len; i++) {
                            typename T::value_type obj{}; // Initialized but then overwritten by the deserializer
                            Eng3D::Deser::deser_dynamic<false>(ar, obj);
                            constexpr bool has_push_back = requires(T a, typename T::value_type tp) { a.push_back(tp); };
                            if constexpr(has_push_back)
                                obj_group.push_back(obj);
                            else if constexpr(has_insert)
                                obj_group.insert(obj);
                        }
                    }
                }
            }
        }
    };

    /// @todo On some compilers a boolean can be something not a uint8_t, we should
    // explicitly recast this boolean into a uint8_t to avoid problems
    template<>
    struct Serializer<bool> {
        template<bool is_const>
        using type = typename CondConstType<is_const, bool>::type;

        template<bool is_serialize>
        static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
            auto tmp = static_cast<char>(obj);
            Eng3D::Deser::deser_dynamic<is_serialize>(ar, tmp);
            if constexpr(!is_serialize) {
                obj = tmp;
            }
        }
    };

    /// @brief Pair serializers
    template<typename T, typename U>
    struct Serializer<::std::pair<T, U>> {
        template<bool is_const>
        using type = typename CondConstType<is_const, ::std::pair<T, U>>::type;

        template<bool is_serialize>
        static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
            using tp_1 = ::std::remove_const_t<T>;
            using tp_2 = ::std::remove_const_t<U>;
            Eng3D::Deser::deser_dynamic<is_serialize>(ar, const_cast<tp_1&>(obj.first));
            Eng3D::Deser::deser_dynamic<is_serialize>(ar, const_cast<tp_2&>(obj.second));
        }
    };

    template<typename T, int N>
    struct SerializerBitset {
        template<bool is_const>
        using type = typename CondConstType<is_const, T>::type;

        template<bool is_serialize>
        static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj_group) {
            unsigned long num = obj_group.to_ulong();
            if constexpr(is_serialize) {
                Eng3D::Deser::deser_dynamic<is_serialize>(ar, num);
            } else {
                Eng3D::Deser::deser_dynamic<is_serialize>(ar, num);
                obj_group = T(num);
            }
        }
    };
    template<size_t bits>
    struct Serializer<::std::bitset<bits>> : SerializerBitset<::std::bitset<bits>, bits> {};

    template<>
    struct Serializer<Eng3D::StringRef> {
        template<bool is_const>
        using type = typename CondConstType<is_const, Eng3D::StringRef>::type;

        template<bool is_serialize>
        static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
            Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.id);
        }
    };

    template<>
    struct Serializer<Eng3D::Rectangle> {
        template<bool is_const>
        using type = typename CondConstType<is_const, Eng3D::Rectangle>::type;

        template<bool is_serialize>
        static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
            Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.left);
            Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.right);
            Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.top);
            Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.bottom);
        }
    };

    template<typename T>
    struct Serializer<std::optional<T>> {
        template<bool is_const>
        using type = typename CondConstType<is_const, std::optional<T>>::type;

        template<bool is_serialize>
        static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
            if constexpr(is_serialize) {
                auto has_value = obj.has_value();
                Eng3D::Deser::deser_dynamic<is_serialize>(ar, has_value);
                if(has_value)
                    Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.value());
            } else {
                bool has_value = false;
                Eng3D::Deser::deser_dynamic<is_serialize>(ar, has_value);
                if(has_value) {
                    obj.emplace();
                    Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.value());
                }
            }
        }
    };

    template<typename T>
    struct Serializer<Eng3D::Freelist<T>> {
        template<bool is_const>
        using type = typename CondConstType<is_const, Eng3D::Freelist<T>>::type;

        template<bool is_serialize>
        static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
            Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.data);
            Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.slots);
        }
    };

    /// @todo Template for entities
}
