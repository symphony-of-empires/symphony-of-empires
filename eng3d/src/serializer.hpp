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
class Archive {
public:
    Archive() = default;
    ~Archive() = default;
    void to_file(const std::string& path);
    void from_file(const std::string& path);

    inline void copy_to(void* ptr, size_t size) {
        if(size > buffer.size() - this->ptr)
            CXX_THROW(SerializerException, "Buffer too small for write");
        std::memcpy(ptr, &buffer[this->ptr], size);
        this->ptr += size;
    }

    inline void copy_from(const void* ptr, size_t size) {
        if(size > buffer.size() - this->ptr)
            CXX_THROW(SerializerException, "Buffer too small for read");
        std::memcpy(&buffer[this->ptr], ptr, size);
        this->ptr += size;
    }

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
        std::memcpy(&buffer[0], buf, size);
    }

    inline size_t size() {
        return buffer.size();
    }

    std::vector<uint8_t> buffer;
    size_t ptr = 0;
};

/// @brief A serializer (base class) which can be used to serialize objects
/// and create per-object optimized classes
template<typename T>
class Serializer {
public:
#ifdef DEBUG_SERIALIZER
    template<bool is_serialize = true>
    static inline void deser_dynamic(Archive&, T*) {
        CXX_THROW(SerializerException, "Implement your serializer function!");
    }

    template<bool is_serialize = false>
    static inline void deser_dynamic(Archive&, const T*) {
        CXX_THROW(SerializerException, "Implement your deserializer function!");
    }
#endif
};

/// @brief Template generic (de)-serializer
/// @tparam is_serialize if true perform serialize mode, otherwise deserialize
/// @tparam T the type to (de)-serialize
template<bool is_serialize, typename T>
inline void deser_dynamic(Archive& ar, T& obj) {
    Serializer<T>::template deser_dynamic<is_serialize>(ar, obj);
}

template<bool is_serialize, typename T>
inline void deser_dynamic(Archive& ar, const T& obj) {
    Serializer<T>::template deser_dynamic<is_serialize>(ar, const_cast<T&>(obj));
}

template<typename T>
inline void serialize(Archive& ar, const T& obj) {
    Serializer<T>::template deser_dynamic<true>(ar, const_cast<T&>(obj));
}

template<typename T>
inline void deserialize(Archive& ar, const T& obj) {
    Serializer<T>::template deser_dynamic<false>(ar, const_cast<T&>(obj));
}

/// @brief A serializer optimized to memcpy directly the element into the byte stream
/// use only when the object can be copied without modification (i.e a class full of ints)
/// The elements must have a fixed size for this to work.
template<typename T>
class SerializerMemcpy {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, T& obj) {
        if constexpr(is_serialize) {
            ar.expand(sizeof(T));
            ar.copy_from(&obj, sizeof(T));
        } else {
            ar.copy_to(&obj, sizeof(T));
        }
    }
};

/// @todo On some compilers a boolean can be something not a uint8_t, we should
// explicitly recast this boolean into a uint8_t to avoid problems
template<>
class Serializer<bool> : public SerializerMemcpy<bool> {};

// A class more focused on numbers :)
template<typename T>
class SerializerNumber {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, T& obj) {
        /// @todo fix big endian
        if constexpr(std::endian::native == std::endian::big) {
            if constexpr(is_serialize) {
                ar.expand(sizeof(T));
                ar.copy_from(&obj, sizeof(T));
                obj = std::byteswap<T>(obj);
            } else {
                T tmp = std::byteswap<T>(obj);
                ar.copy_to(&tmp, sizeof(T));
            }
        } else {
            if constexpr(is_serialize) {
                ar.expand(sizeof(T));
                ar.copy_from(&obj, sizeof(T));
            } else {
                ar.copy_to(&obj, sizeof(T));
            }
        }
    }   
};

// Serializers for primitives only require memcpy
template<>
class Serializer<signed char> : public SerializerNumber<signed char> {};
template<>
class Serializer<signed short> : public SerializerNumber<signed short> {};
template<>
class Serializer<signed int> : public SerializerNumber<signed int> {};
template<>
class Serializer<signed long> : public SerializerNumber<signed long> {};
template<>
class Serializer<signed long long> : public SerializerNumber<signed long long> {};

template<>
class Serializer<unsigned char> : public SerializerNumber<unsigned char> {};
template<>
class Serializer<unsigned short> : public SerializerNumber<unsigned short> {};
template<>
class Serializer<unsigned int> : public SerializerNumber<unsigned int> {};
template<>
class Serializer<unsigned long> : public SerializerNumber<unsigned long> {};
template<>
class Serializer<unsigned long long> : public SerializerNumber<unsigned long long> {};

/// @brief Converts a float number into a fixed-point integer scaled by a factor of 1000
template<typename T>
class SerializerFloat {
    static constexpr auto scaling = 1000.f;
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, T& obj) {
        int32_t tmp;
        if constexpr(is_serialize) {
            tmp = obj * scaling;
            ::deser_dynamic<is_serialize>(ar, tmp);
        } else {
            ::deser_dynamic<is_serialize>(ar, tmp);
            obj = static_cast<T>(tmp) / scaling;
        }
    }   
};

template<>
class Serializer<float> : public SerializerFloat<float> {};
template<>
class Serializer<double> : public SerializerFloat<double> {};
template<>
class Serializer<long double> : public SerializerFloat<long double> {};

/// @brief A serializer specialized in strings
/// The serializer stores the lenght of the string and the string itself
/// this is done so no errors can happen due to null stuff. (UTF-8 especially)
template<>
class Serializer<std::string> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, std::string& obj) {
        /// @brief Used to reduce number of uneeded allocations
        char tmpstr_buf[1024];
        if constexpr(is_serialize) {
            // Truncate lenght
            uint16_t len = static_cast<uint16_t>(std::min<size_t>(std::numeric_limits<uint16_t>::max(), obj.length()));
            ::serialize(ar, len); // Put length for later deserialization (since UTF-8/UTF-16 exists)
            if(len) { // Copy the string into the output
                ar.expand(len);
                ar.copy_from(obj.c_str(), len);
            }
        } else {
            uint16_t len = 0; // Obtain the lenght of the string to be read
            ::deserialize(ar, len);
            if(len >= sizeof(tmpstr_buf))
                CXX_THROW(SerializerException, "String is too lenghty");
            if(len) // Obtain the string itself
                ar.copy_to(tmpstr_buf, len);
            tmpstr_buf[len] = '\0';
            obj = tmpstr_buf;
        }
    }
};

/// @brief Non-contigous serializer for STL containers
/// This serializer class works primarly with containers whose memory is contiguous
template<typename T, typename C>
class SerializerContainer {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, C& obj_group) {
        uint32_t len = obj_group.size();
        ::deser_dynamic<is_serialize>(ar, len);
        if constexpr(is_serialize) {
            for(auto& obj : obj_group)
                ::deser_dynamic<is_serialize>(ar, obj);
        } else {
            obj_group.clear();
            for(decltype(len) i = 0; i < len; i++) {
                T obj;
                ::deser_dynamic<is_serialize>(ar, obj);
                obj_group.insert(obj);
            }
        }
    }
};

/// @brief Pair serializers
template<typename T, typename U>
class Serializer<std::pair<T, U>> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, std::pair<T, U>& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.first);
        ::deser_dynamic<is_serialize>(ar, obj.second);
    }
};

#include "eng3d/string.hpp"
template<>
class Serializer<Eng3D::StringRef> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Eng3D::StringRef& obj) {
        if constexpr(is_serialize) {
            ::deser_dynamic<is_serialize>(ar, obj.get_string());
        } else {
            std::string deserialized_string;
            ::deser_dynamic<is_serialize>(ar, deserialized_string);
            obj = Eng3D::StringRef(deserialized_string);
        }
    }
};

/// @brief Contigous container serializers implementations
#include <vector>
template<typename T, typename A>
class Serializer<std::vector<T, A>> : public SerializerContainer<T, std::vector<T, A>> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, std::vector<T, A>& obj_group) {
        uint32_t len = obj_group.size();
        ::deser_dynamic<is_serialize>(ar, len);
        if constexpr(is_serialize) {
            for(auto& obj : obj_group)
                ::deser_dynamic<is_serialize>(ar, obj);
        } else {
            obj_group.resize(len);
            for(decltype(len) i = 0; i < len; i++)
                ::deser_dynamic<is_serialize, T>(ar, obj_group[i]);
        }
    }
};

#include <deque>
template<typename T, typename A>
class Serializer<std::deque<T, A>> : public SerializerContainer<T, std::deque<T, A>> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, std::deque<T, A>& obj_group) {
        uint32_t len = obj_group.size();
        ::deser_dynamic<is_serialize>(ar, len);
        if constexpr(is_serialize) {
            for(auto& obj : obj_group)
                ::deser_dynamic<is_serialize>(ar, obj);
        } else {
            obj_group.resize(len);
            for(decltype(len) i = 0; i < len; i++)
                ::deser_dynamic<is_serialize, T>(ar, obj_group[i]);
        }
    }
};

#include <queue>
template<typename T, typename S>
class Serializer<std::queue<T, S>> : public SerializerContainer<T, std::queue<T, S>> {};

#include <set>
template<typename K, typename C, typename A>
class Serializer<std::set<K, C, A>> : public SerializerContainer<K, std::set<K, C, A>> {};

#include <unordered_set>
template<typename V, typename H, typename P, typename A>
class Serializer<std::unordered_set<V, H, P, A>> : public SerializerContainer<V, std::unordered_set<V, H, P, A>> {};

#include <bitset>
template<typename T, int N>
class SerializerBitset {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, T& obj_group) {
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
class Serializer<std::bitset<bits>> : public SerializerBitset<std::bitset<bits>, bits> {};

/// @brief Used as a template for serializable objects (pointers mostly) which should be
/// treated as a reference instead of the object itself
template<typename W, typename T>
class SerializerReference {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, T*& obj) {
        typename T::Id id = obj == nullptr ? T::invalid() : W::get_instance().get_id(*obj);
        ::deser_dynamic<is_serialize>(ar, id);
        if constexpr(!is_serialize) {
            if(id >= W::get_instance().get_list((T*)nullptr).size()) {
                obj = nullptr;
            } else {
                obj = id != T::invalid() ? W::get_instance().get_list((T*)nullptr)[id] : nullptr;
            }
        }
    }
};

// Non-pointer
template<typename W, typename T>
class SerializerReferenceLocal {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, T*& obj) {
        typename T::Id id = obj == nullptr ? T::invalid() : W::get_instance().get_id(*obj);
        ::deser_dynamic<is_serialize>(ar, id);
        if constexpr(!is_serialize) {
            if(id >= W::get_instance().get_list((T*)nullptr).size()) {
                obj = nullptr;
            } else {
                obj = id != T::invalid() ? &(W::get_instance().get_list((T*)nullptr)[id]) : nullptr;
            }
        }
    }
};

#include "eng3d/rectangle.hpp"
template<>
class Serializer<Eng3D::Rectangle> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Eng3D::Rectangle& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.left);
        ::deser_dynamic<is_serialize>(ar, obj.right);
        ::deser_dynamic<is_serialize>(ar, obj.top);
        ::deser_dynamic<is_serialize>(ar, obj.bottom);
    }
};

/// @todo Template for entities
