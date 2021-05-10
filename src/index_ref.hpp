#ifndef INDEXREF_H
#define INDEXREF_H

#include <vector>

template<class T>
class IndexRef {
private:
    std::vector<T> * vec;
    size_t index;
public:
    IndexRef() {};
    IndexRef(std::vector<T> * vec, size_t index) {
        this->vec = vec;
        this->index = index;
    }
    T & get() {
        return vec->at(index);
    }
};

#endif