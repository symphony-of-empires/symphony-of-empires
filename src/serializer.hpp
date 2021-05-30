#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <cstddef>
#include <cstdint>
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
	static inline void serialize(std::byte *& output, T const& obj);
	static inline void deserialize(std::byte const *& input, T& obj);
};

#include <cstring>
/**
 * A serializer optimized to memcpy directly the element into the byte stream
 * use only when the object can be copied without modification (i.e a class full of ints)
 */
template<typename T>
class SerializerMemcpy {
public:
	static inline void serialize(std::byte *& output, T const& obj) {
		memcpy(output, &obj, sizeof(T));
		output += sizeof(T);
	}
	static inline void deserialize(std::byte const *& input, T& obj) {
		memcpy(&obj, input, sizeof(T));
		input += sizeof(T);
	}
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
	static inline void serialize(std::byte *& output, std::string const& obj) {
		const uint32_t len = obj.length();

		// Put length for later deserialization (since UTF-8/UTF-16 exists)
		memcpy(output, &len, sizeof(len));
		output += sizeof(len);

		// Copy the string into the output
		memcpy(output, obj.c_str(), len);
		output += len;
	}
	static inline void deserialize(std::byte const *& input, std::string& obj) {
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
 * Only use on contigous STL containers, otherwise the optimization will be useless
 * and will cause a segfault
 */
template<typename T, typename C>
class SerializerContainerMemcpy {
public:
	static inline void serialize(std::byte *& output, C const& obj_group) {
		for(const auto& obj: obj_group) {
			memcpy(output, &obj, sizeof(T));
			output += sizeof(T);
		}
	}
	static inline void deserialize(std::byte const *& input, C& obj_group) {
		for(auto& obj: obj_group) {
			memcpy(&obj, input, sizeof(T));
			input += sizeof(T);
		}
	}
};

/**
 * Contigous container serializers
 */
#include <vector>
#include <queue>
#include <deque>

template<typename T>
class Serializer<std::vector<T>> : public SerializerContainerMemcpy<T, std::vector<T>> {};
template<typename T>
class Serializer<std::queue<T>> : public SerializerContainerMemcpy<T, std::queue<T>> {};
template<typename T>
class Serializer<std::deque<T>> : public SerializerContainerMemcpy<T, std::deque<T>> {};

#endif