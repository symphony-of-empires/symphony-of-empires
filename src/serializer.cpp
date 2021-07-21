#include "serializer.hpp"

void Archive::copy_to(void* ptr, size_t size) {
    if(size > buffer.size() - this->ptr)
        throw SerializerException("Buffer too small");
    
    memcpy(ptr, &buffer[this->ptr], size);
    this->ptr += size;
}

void Archive::copy_from(const void* ptr, size_t size) {
    if(size > buffer.size() - this->ptr)
        throw SerializerException("Buffer too small");
    
    memcpy(&buffer[this->ptr], ptr, size);
    this->ptr += size;
}

// Expands the archive to fit a new serialized object
void Archive::expand(size_t amount) {
    buffer.resize(buffer.size() + amount);
}

// Call when serialization has ended and it's ready to be sent to a fstream
void Archive::end_stream(void) {
    buffer.shrink_to_fit();
}

void Archive::rewind(void) {
    ptr = 0;
}

void Archive::to_file(std::string path) {
    FILE* fp = fopen(path.c_str(), "wb");
    fwrite(&buffer[0], 1, buffer.size(), fp);
    fclose(fp);
        
    end_stream();
}

void Archive::from_file(std::string path) {
    FILE* fp = fopen(path.c_str(), "rb");
    
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    ::rewind(fp);
    
    buffer.resize(size);
    fread(&buffer[0], 1, buffer.size(), fp);
    fclose(fp);
    
    end_stream();
}

void* Archive::get_buffer(void) {
    return (void*)&buffer[0];
}
    
void Archive::set_buffer(void* buf, size_t size) {
    buffer.resize(size);
    memcpy(&buffer[0], buf, size);
}
    
size_t Archive::size(void) {
    return buffer.size();
}
