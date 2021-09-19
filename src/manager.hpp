#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <vector>

template<typename T, typename I>
class Manager {
private:
    std::vector<std::pair<T *, I>> elems;
public:
    // Global manager object
    static Manager<T, I> *singleton;

    // Singleton pattern ;)
    static Manager<T, I>& get_instance(void) {
        return *singleton;
    };

    // Load an element, this is the function that must be defined by the inheritor
    virtual void load(const I& ident) {};

    // Obtain an element or construct a new one from a provided
    // construct which accepts ident
    virtual const T& get(const I& ident) {
        // TODO: How do we remove this duplicate code? :/
        for(const auto& o: elems) {
            if(o.second == ident) {
                return *(o.first);
            }
        }

        load(ident);
        
        for(const auto& o: elems) {
            if(o.second == ident) {
                return *(o.first);
            }
        }
        return *o;
    };
};

#endif