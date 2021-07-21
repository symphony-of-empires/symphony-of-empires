#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <numeric>
#include <string>

/**
 * The purpouse of the serializer is to serialize objects onto a byte stream
 * that can be transfered onto the disk or over the network.
 *
 * Should the object have any pointers - they would need to be converted to
 * indexes accordingly for proper transmission.
 */

class SerializerException{
    std::string buffer;
public:
    SerializerException(std::string msg) {
        buffer = msg;
    };
    virtual const char* what(void) const noexcept {
        return buffer.c_str();
    };
};

/**
 * Base class that serves as archiver, stores (in memory) the data required for
 * serialization/deserialization
 */
#include <vector>
#include <cstdio>
#include <string>
class Archive {
public:
    std::vector<uint8_t> buffer;
    size_t ptr = 0;

    Archive() {};
    ~Archive() {};
    
    void copy_to(void* ptr, size_t size);
    void copy_from(const void* ptr, size_t size);
    void expand(size_t amount);
    void end_stream(void);
    void rewind(void);
    void to_file(std::string path);
    void from_file(std::string path);
    void* get_buffer(void);
    void set_buffer(void* buf, size_t size);
    size_t size(void);
};

/**
* A serializer (base class) which can be used to serialize objects
* and create per-object optimized classes
 */
template<typename T>
class Serializer {
public:
    static constexpr bool is_const_size = false;
    static inline void serialize(Archive& ar, const T* obj);
    static inline void deserialize(Archive& ar, T* obj);
    static inline size_t size(const T* obj);
};

/**
* A serializer specialized in strings
* The serializer stores the lenght of the string and the string itself
* this is done so no errors can happen due to null stuff. (UTF-8 especially)
 */
#include <string>
template<>
class Serializer<std::string> {
public:
    static constexpr bool is_const_size = false;
    static inline void serialize(Archive& ar, const std::string* obj) {
        uint16_t len = obj->length();

        // Truncate lenght
        if(len >= 1024) {
            len = 1024;
        }

        // Put length for later deserialization (since UTF-8/UTF-16 exists)
        ar.expand(sizeof(len));
        ar.copy_from(&len, sizeof(len));

        // Copy the string into the output
        ar.expand(len);
        ar.copy_from(obj->c_str(), len);
    }
    static inline void deserialize(Archive& ar, std::string* obj) {
        uint16_t len;

        // Obtain the lenght of the string to be read
        ar.copy_to(&len, sizeof(len));

        if(len >= 1024)
            throw SerializerException("String is too lenghty");

        // Obtain the string itself
        char* string = new char[len + 1];
        
        ar.copy_to(string, len);
        string[len] = '\0';
        
        *obj = string;
        delete[] string;
    }
    static inline size_t size(const std::string* obj) {
        return sizeof(uint32_t) + (obj->length()* sizeof(char));
    }
};

/**
* A serializer optimized to memcpy directly the element into the byte stream
* use only when the object can be copied without modification (i.e a class full of ints)
* The elements must have a fixed size for this to work.
 */
template<typename T>
class SerializerMemcpy {
public:
    static constexpr bool is_const_size = true;
    static inline void serialize(Archive& ar, const T* obj) {
        ar.expand(size(obj));
        ar.copy_from(obj, sizeof(T));
    }
    static inline void deserialize(Archive& ar, T* obj) {
        ar.copy_to(obj, sizeof(T));
    }
    static constexpr size_t size(const T*) {
        return sizeof(T);
    }
};

// Serializers for primitives only require memcpy
template<>
class Serializer<uint64_t> : public SerializerMemcpy<uint64_t> {};
template<>
class Serializer<uint32_t> : public SerializerMemcpy<uint32_t> {};
template<>
class Serializer<uint16_t> : public SerializerMemcpy<uint16_t> {};
template<>
class Serializer<uint8_t> : public SerializerMemcpy<uint8_t> {};

template<>
class Serializer<int64_t> : public SerializerMemcpy<int64_t> {};
template<>
class Serializer<int32_t> : public SerializerMemcpy<int32_t> {};
template<>
class Serializer<int16_t> : public SerializerMemcpy<int16_t> {};
template<>
class Serializer<int8_t> : public SerializerMemcpy<int8_t> {};

template<>
class Serializer<long double> : public SerializerMemcpy<long double> {};
template<>
class Serializer<double> : public SerializerMemcpy<double> {};
template<>
class Serializer<float> : public SerializerMemcpy<float> {};

template<>
class Serializer<bool> : public SerializerMemcpy<bool> {};

/**
* Non-contigous serializer for STL containers
 */
template<typename T, typename C>
class SerializerContainer {
public:
    static constexpr bool is_const_size = false;
    static inline void serialize(Archive& ar, const C* obj_group) {
        uint32_t len = obj_group->size();
        ar.expand(sizeof(len));
        ar.copy_from(&len, sizeof(len));

        for(auto& obj: *obj_group) {
            Serializer<T>::serialize(ar, &obj);
        }
    }
    static inline void deserialize(Archive& ar, C* obj_group) {
        uint32_t len;
        ar.copy_to(&len, sizeof(len));

        for(size_t i = 0; i < len; i++) {
            T obj;
            Serializer<T>::deserialize(ar, &obj);
            obj_group->insert(obj);
        }
    }
    static constexpr size_t size(const C* obj_group) {
        return sizeof(uint32_t) + (obj_group->size()* sizeof(T));
    }
};

/**
* Pair serializers
 */
template<typename T, typename U>
class Serializer<std::pair<T, U>> {
public:
    static constexpr bool is_const_size = true;
    static inline void serialize(Archive& ar, const std::pair<T, U>* obj) {
        Serializer<T>::serialize(ar, &obj->first);
        Serializer<U>::serialize(ar, &obj->second);
    }
    static inline void deserialize(Archive& ar, std::pair<T, U>* obj) {
        Serializer<T>::deserialize(ar, &obj->first);
        Serializer<U>::deserialize(ar, &obj->second);
    }
    static constexpr size_t size(const std::pair<T, U>* obj) {
        return Serializer<T>::size(obj->first) + Serializer<T>::size(obj->second);
    }
};

/**
* Contigous container serializers implementations
 */
#include <vector>
template<typename T>
class Serializer<std::vector<T>> : public SerializerContainer<T, std::vector<T>> {
public:
    static constexpr bool is_const_size = false;
    static inline void serialize(Archive& ar, const std::vector<T>* obj_group) {
        uint32_t len = obj_group->size();
        ar.expand(sizeof(len));
        ar.copy_from(&len, sizeof(len));
        for(auto& obj: *obj_group) {
            Serializer<T>::serialize(ar, &obj);
        }
    }
    static inline void deserialize(Archive& ar, std::vector<T>* obj_group) {
        uint32_t len;
        ar.copy_to(&len, sizeof(len));
        for(size_t i = 0; i < len; i++) {
            T obj;
            Serializer<T>::deserialize(ar, &obj);
            obj_group->push_back(obj);
        }
    }
};

#include <deque>
template<typename T>
class Serializer<std::deque<T>> : public SerializerContainer<T, std::deque<T>> {
public:
    static constexpr bool is_const_size = false;
    static inline void serialize(Archive& ar, const std::deque<T>* obj_group) {
        uint32_t len = obj_group->size();
        ar.expand(sizeof(len));
        ar.copy_from(&len, sizeof(len));

        for(auto& obj: *obj_group) {
            Serializer<T>::serialize(ar, &obj);
        }
    }
    static inline void deserialize(Archive& ar, std::deque<T>* obj_group) {
        uint32_t len;
        ar.copy_to(&len, sizeof(len));

        for(size_t i = 0; i < len; i++) {
            T obj;
            Serializer<T>::deserialize(ar, &obj);
            obj_group->push_back(obj);
        }
    }
};

#include <queue>
template<typename T>
class Serializer<std::queue<T>> : public SerializerContainer<T, std::queue<T>> {};

#include <set>
template<typename T>
class Serializer<std::set<T>> : public SerializerContainer<T, std::set<T>> {};

template<typename T>
inline void serialize(Archive& ar, const T* obj) {
    Serializer<T>::serialize(ar, obj);
}

template<typename T>
inline void deserialize(Archive& ar, T* obj) {
    Serializer<T>::deserialize(ar, obj);
}

template<typename T>
constexpr size_t serialized_size(const T* obj) {
    return Serializer<T>::size(obj);
}

#endif
