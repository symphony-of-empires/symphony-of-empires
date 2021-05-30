#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <numeric>

/**
 * The purpouse of the serializer is to serialize objects onto a byte stream
 * that can be transfered onto the disk or over the network.
 *
 * Should the object have any pointers - they would need to be converted to
 * indexes accordingly for proper transmission.
 */

/**
 * A serializer (base class) which can be used to serialize objects
 * and create per-object optimized classes
 */
template<typename T>
class Serializer {
public:
	static constexpr bool is_const_size = true;
	static inline void serialize(uint8_t *& output, T const& obj);
	static inline void deserialize(uint8_t const *& input, T& obj);
	static inline size_t size(const T& obj);
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
	static inline void serialize(uint8_t *& output, std::string const& obj) {
		const uint32_t len = obj.length();

		// Put length for later deserialization (since UTF-8/UTF-16 exists)
		memcpy(output, &len, sizeof(len));
		output += sizeof(len);

		// Copy the string into the output
		memcpy(output, obj.c_str(), len);
		output += len;
	}
	static inline void deserialize(uint8_t const *& input, std::string& obj) {
		uint32_t len;

		// Obtain the lenght of the string to be read
		memcpy(&len, input, sizeof(len));
		input += sizeof(len);

		// Obtain the string itself
		char * string = new char[len + 1];
		memcpy(string, input, len);
		input += len;
		obj = string;
		delete[] string;
	}
	static inline size_t size(const std::string& obj) {
		return sizeof(uint32_t) + (obj.length() * sizeof(char));
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
	static inline void serialize(uint8_t *& output, T const& obj) {
		memcpy(output, &obj, sizeof(T));
		output += sizeof(T);
	}
	static inline void deserialize(uint8_t const *& input, T& obj) {
		memcpy(&obj, input, sizeof(T));
		input += sizeof(T);
	}
	constexpr static inline size_t size(const T& obj) {
		return sizeof(T);
	}
};

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
 * A full STL-container copy, **optimized** to copy many elements from a contigous
 * STL vector
 *
 * Only use on STL containers that are guaranteed to be contigous, otherwise the
 * optimization will be useless and will cause a segfault
 *
 * If the elements of the container are constexpr evaluable (they don't have dynamic sizes)
 * then this will tell the compiler to use a simple memcpy to avoid loops
 */
template<typename T, typename C>
class SerializerContainer {
public:
	static constexpr bool is_const_size = false;
	static inline void serialize(uint8_t *& output, C const& obj_group) {
		const uint32_t len = obj_group.size();
		memcpy(output, &len, sizeof(len));
		output += sizeof(len);

		if constexpr(Serializer<T>::is_const_size) {
			memcpy(output, &obj_group[0], sizeof(T) * len);
			output += sizeof(T) * len;
		} else {
			for(const auto& obj: obj_group) {
				Serializer<T>::serializer(output, obj);
			}
		}
	}
	static inline void deserialize(uint8_t const *& input, C& obj_group) {
		uint32_t len;
		memcpy(&len, input, sizeof(len));
		input += sizeof(len);

		if constexpr(Serializer<T>::is_const_size) {
			obj_group.resize(len);
			memcpy(&obj_group[0], input, sizeof(T) * len);
			input += sizeof(T) * len;
		} else {
			for(size_t i = 0; i < len; i++) {
				T obj;
				Serializer<T>::deserializer(input, obj);
				obj_group.push_back(obj);
			}
		}
	}
	constexpr static inline size_t size(const C& obj_group) {
		return sizeof(uint32_t) + (obj_group.size() * sizeof(T));
	}
};

/**
 * Contigous container serializers implementations
 */
#include <vector>
#include <queue>
#include <deque>
template<typename T>
class Serializer<std::vector<T>> : public SerializerContainer<T, std::vector<T>> {};
template<typename T>
class Serializer<std::queue<T>> : public SerializerContainer<T, std::queue<T>> {};
template<typename T>
class Serializer<std::deque<T>> : public SerializerContainer<T, std::deque<T>> {};

#endif