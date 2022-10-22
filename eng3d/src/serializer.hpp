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
#include <cstdio>
#include <type_traits>
#include <limits>
#include <glm/glm.hpp>
#include "eng3d/utils.hpp"

/// @brief The purpouse of the serializer is to serialize objects onto a byte stream
/// that can be transfered onto the disk or over the network. Should the object have
/// any pointers - they would need to be converted to indexes accordingly for
/// proper transmission.
class SerializerException : public std::exception {
    std::string buffer;
public:
    SerializerException(const std::string& msg) {
        buffer = msg;
    };
    virtual const char* what() const noexcept {
        return buffer.c_str();
    };
};

/// @brief Base class that serves as archiver, stores (in memory) the data required for
/// serialization/deserialization
struct Archive {
    Archive() = default;
    ~Archive() = default;
    void to_file(const std::string& path);
    void from_file(const std::string& path);
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
        std::memcpy(buffer.data(), buf, size);
    }

    inline size_t size() {
        return buffer.size();
    }

    std::vector<uint8_t> buffer;
    size_t ptr = 0;
};

template<bool is_const, typename T>
struct CondConstType;
template<typename T>
struct CondConstType<true, T> { using type = const std::remove_reference_t<T>; };
template<typename T>
struct CondConstType<false, T> { using type = std::remove_reference_t<T>; };

/// @brief A serializer (base class) which can be used to serialize objects
/// and create per-object optimized classes
template<typename T>
struct Serializer {
    template<bool is_const>
    using type = CondConstType<is_const, T>::type;
#ifdef DEBUG_SERIALIZER
    template<bool is_serialize = true>
    static inline void deser_dynamic(Archive&, T&&) {
        CXX_THROW(SerializerException, "Implement your serializer function!");
    }

    template<bool is_serialize = false>
    static inline void deser_dynamic(Archive&, const T&&) {
        CXX_THROW(SerializerException, "Implement your deserializer function!");
    }
#endif
};

/// @brief Template generic (de)-serializer
/// @tparam is_serialize if true perform serialize mode, otherwise deserialize
/// @tparam T the type to (de)-serialize
template<bool is_serialize, typename T>
void deser_dynamic(Archive& ar, T& obj);

template<bool is_serialize = true, typename T>
void deser_dynamic(Archive& ar, const T& obj) {
    Serializer<std::remove_reference_t<T>>::template deser_dynamic<is_serialize>(ar, obj);
}

template<bool is_serialize = false, typename T>
void deser_dynamic(Archive& ar, T& obj) {
    Serializer<std::remove_reference_t<T>>::template deser_dynamic<is_serialize>(ar, obj);
}

template<typename T>
inline void serialize(Archive& ar, const T& obj) {
    Serializer<std::remove_reference_t<T>>::template deser_dynamic<true>(ar, obj);
}

template<typename T>
inline void serialize(Archive& ar, T& obj) {
    Serializer<std::remove_reference_t<T>>::template deser_dynamic<true>(ar, obj);
}

template<typename T>
inline void deserialize(Archive& ar, T& obj) {
    Serializer<std::remove_reference_t<T>>::template deser_dynamic<false>(ar, obj);
}

/// @brief A serializer optimized to memcpy directly the element into the byte stream
/// use only when the object can be copied without modification (i.e a class full of ints)
/// The elements must have a fixed size for this to work.
template<typename T>
struct SerializerMemcpy {
    template<bool is_const>
    using type = CondConstType<is_const, T>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj) {
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
    using type = CondConstType<is_const, T>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj) {
        if constexpr(std::is_floating_point_v<T>) {
            auto tmp = static_cast<int32_t>(obj * scaling);
            ::deser_dynamic<is_serialize>(ar, tmp);
            if constexpr(!is_serialize)
                obj = static_cast<T>(tmp) * (1.f / scaling);
        } else {
            if constexpr(is_serialize && std::endian::native == std::endian::big)
                obj = std::byteswap<T>(obj);
            SerializerMemcpy<T>::template deser_dynamic<is_serialize>(ar, obj);
            if constexpr(!is_serialize && std::endian::native == std::endian::big)
                obj = std::byteswap<T>(obj);
        }
    }
};

template<typename T>
concept SerializerContainer = requires(T a, T b) {
    requires std::destructible<typename T::value_type>;
    requires std::forward_iterator<typename T::iterator>;
    { a.begin() } -> std::same_as<typename T::iterator>;
    { a.end() } -> std::same_as<typename T::iterator>;
};
/// @brief Non-contigous serializer for STL containers
/// This serializer class works primarly with containers whose memory is contiguous
template<SerializerContainer T>
struct Serializer<T> {
    template<bool is_const>
    using type = CondConstType<is_const, T>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj_group) {
        constexpr bool has_data = requires(T a) { a.data(); };
        uint32_t len = obj_group.size();
        ::deser_dynamic<is_serialize>(ar, len);
        if(!len) return; // Early exit iff nothing to do

        if constexpr(is_serialize) {
            if constexpr(has_data && std::is_trivially_copyable<typename T::value_type>::value) {
                ar.copy_from(obj_group.data(), len * sizeof(typename T::value_type));
            } else { // non-trivial
                for(auto& obj : obj_group)
                    ::deser_dynamic<true>(ar, obj);
            }
        } else {
            // No insert means this is a static array of some sort, std::array perhaps?
            constexpr bool has_insert = requires(T a, typename T::value_type tp) { a.insert(tp); };
            constexpr bool has_resize = requires(T a, size_t n) { a.resize(n); };
            if constexpr(!has_insert && !has_resize) {
                for(decltype(len) i = 0; i < len; i++)
                    ::deser_dynamic<false>(ar, obj_group[i]);
                return;
            }

            if constexpr(has_resize) {
                obj_group.resize(len);
                if constexpr(has_data && std::is_trivially_copyable<typename T::value_type>::value) {
                    ar.copy_to(obj_group.data(), len * sizeof(typename T::value_type));
                } else { // non-len
                    for(decltype(len) i = 0; i < len; i++)
                        ::deser_dynamic<false>(ar, obj_group[i]);
                }
            } else { // non-len, no resize
                for(decltype(len) i = 0; i < len; i++) {
                    typename T::value_type obj{}; // Initialized but then overwritten by the deserializer
                    ::deser_dynamic<false>(ar, obj);
                    constexpr bool has_push_back = requires(T a, typename T::value_type tp) { a.push_back(tp); };
                    if constexpr(has_push_back)
                        obj_group.push_back(obj);
                    else if constexpr(has_insert)
                        obj_group.insert(obj);
                }
            }
        }
    }
};

/// @todo On some compilers a boolean can be something not a uint8_t, we should
// explicitly recast this boolean into a uint8_t to avoid problems
template<>
struct Serializer<bool> : SerializerMemcpy<bool> {};

/// @brief Pair serializers
template<typename T, typename U>
struct Serializer<std::pair<T, U>> {
    template<bool is_const>
    using type = CondConstType<is_const, std::pair<T, U>>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.first);
        ::deser_dynamic<is_serialize>(ar, obj.second);
    }
};

#include <bitset>
template<typename T, int N>
struct SerializerBitset {
    template<bool is_const>
    using type = CondConstType<is_const, T>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj_group) {
        unsigned long num = obj_group.to_ulong();
        if constexpr(is_serialize) {
            ::deser_dynamic<is_serialize>(ar, num);
        } else {
            ::deser_dynamic<is_serialize>(ar, num);
            obj_group = T(num);
        }
    }
};
template<size_t bits>
struct Serializer<std::bitset<bits>> : SerializerBitset<std::bitset<bits>, bits> {};

#include "eng3d/string.hpp"
template<>
struct Serializer<Eng3D::StringRef> {
    template<bool is_const>
    using type = CondConstType<is_const, Eng3D::StringRef>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.id);
    }
};

#include "eng3d/rectangle.hpp"
template<>
struct Serializer<Eng3D::Rectangle> {
    template<bool is_const>
    using type = CondConstType<is_const, Eng3D::Rectangle>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.left);
        ::deser_dynamic<is_serialize>(ar, obj.right);
        ::deser_dynamic<is_serialize>(ar, obj.top);
        ::deser_dynamic<is_serialize>(ar, obj.bottom);
    }
};

/// @todo Template for entities
