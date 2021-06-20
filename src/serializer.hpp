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
 * Base class that serves as archiver, stores (in memory) the data required for
 * serialization/deserialization
 */
#include <vector>
#include <cstdio>
#include <string>
class Archive {
public:
	Archive() {};
	~Archive() {};

	std::vector<uint8_t> buffer;
	size_t ptr = 0;

	// Expands the archive to fit a new serialized object
	inline void expand(size_t amount) {
		buffer.resize(buffer.size() + amount);
	}

	// Call when serialization has ended and it's ready to be sent to a fstream
	inline void end_stream(void) {
		buffer.shrink_to_fit();
	}

	void to_file(std::string path) {
		FILE * fp = fopen(path.c_str(), "wb");
		fwrite(&buffer[0], 1, buffer.size(), fp);
		fclose(fp);
		
		end_stream();
	}
	
	void from_file(std::string path) {
		FILE * fp = fopen(path.c_str(), "rb");
		
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		rewind(fp);
		
		buffer.resize(size);
		fread(&buffer[0], 1, buffer.size(), fp);
		fclose(fp);
		
		end_stream();
	}

	void * get_buffer(void) {
		return (void *)&buffer[0];
	}
	
	void set_buffer(void * buf, size_t size) {
		buffer.resize(size);
		memcpy(&buffer[0], buf, size);
	}
	
	size_t size(void) {
		return buffer.size();
	}
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
		uint32_t len = obj->length();

		// Put length for later deserialization (since UTF-8/UTF-16 exists)
		ar.expand(sizeof(len));
		memcpy(&ar.buffer[ar.ptr], &len, sizeof(len));
		ar.ptr += sizeof(len);

		// Copy the string into the output
		ar.expand(len);
		memcpy(&ar.buffer[ar.ptr], obj->c_str(), len);
		ar.ptr += len;
	}
	static inline void deserialize(Archive& ar, std::string* obj) {
		uint32_t len;

		// Obtain the lenght of the string to be read
		memcpy(&len, &ar.buffer[ar.ptr], sizeof(len));
		ar.ptr += sizeof(len);

		// Obtain the string itself
		char * string = new char[len + 1];
		
		memcpy(string, &ar.buffer[ar.ptr], len);
		string[len] = '\0';

		ar.ptr += len;
		*obj = string;
		delete[] string;
	}
	static inline size_t size(const std::string* obj) {
		return sizeof(uint32_t) + (obj->length() * sizeof(char));
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
		memcpy(&ar.buffer[ar.ptr], obj, sizeof(T));
		ar.ptr += sizeof(T);
	}
	static inline void deserialize(Archive& ar, T* obj) {
		memcpy(obj, &ar.buffer[ar.ptr], sizeof(T));
		ar.ptr += sizeof(T);
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
		memcpy(&ar.buffer[ar.ptr], &len, sizeof(len));
		ar.ptr += sizeof(len);

		for(auto& obj: *obj_group) {
			Serializer<T>::serialize(ar, &obj);
		}
	}
	static inline void deserialize(Archive& ar, C* obj_group) {
		uint32_t len;
		memcpy(&len, &ar.buffer[ar.ptr], sizeof(len));
		ar.ptr += sizeof(len);

		for(size_t i = 0; i < len; i++) {
			T obj;
			Serializer<T>::deserialize(ar, &obj);
			obj_group->insert(obj);
		}
	}
	static constexpr size_t size(const C* obj_group) {
		return sizeof(uint32_t) + (obj_group->size() * sizeof(T));
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
	static constexpr size_t size(const std::pair<T, U>*) {
		return sizeof(T) + sizeof(U);
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
		memcpy(&ar.buffer[ar.ptr], &len, sizeof(len));
		ar.ptr += sizeof(len);

		for(auto& obj: *obj_group) {
			Serializer<T>::serialize(ar, &obj);
		}
	}
	static inline void deserialize(Archive& ar, std::vector<T>* obj_group) {
		uint32_t len;
		memcpy(&len, &ar.buffer[ar.ptr], sizeof(len));
		ar.ptr += sizeof(len);

		for(size_t i = 0; i < len; i++) {
			T obj;
			Serializer<T>::deserialize(ar, &obj);
			obj_group->push_back(obj);
		}
	}
};

#include <deque>
template<typename T>
class Serializer<std::deque<T>> : public SerializerContainer<T, std::deque<T>> {};

#include <queue>
template<typename T>
class Serializer<std::queue<T>> : public SerializerContainer<T, std::queue<T>> {};

#include <set>
template<typename T>
class Serializer<std::set<T>> : public SerializerContainer<T, std::set<T>> {};

template<typename T>
inline void serialize(Archive& ar, const T * obj) {
	Serializer<T>::serialize(ar, obj);
}

template<typename T>
inline void deserialize(Archive& ar, T * obj) {
	Serializer<T>::deserialize(ar, obj);
}

template<typename T>
constexpr size_t serialized_size(const T * obj) {
	return Serializer<T>::size(obj);
}

#endif
