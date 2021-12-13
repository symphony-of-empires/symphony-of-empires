#pragma once

#include <set>

// Generic manager for any resource type, the manager will call "load" if an element
// with the same ident already exists
template<typename T, typename I, typename L = std::set<std::pair<T*, I>>>
class Manager {
private:
    L elems;
public:
    // Global manager object
    static Manager<T, I>* singleton;

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
        for(const auto& o : elems) {
            if(o.second == ident) {
                return *(o.first);
            }
        }

        load(ident);

        for(const auto& o : elems) {
            if(o.second == ident) {
                return *(o.first);
            }
        }
        throw std::runtime_error("Can't load " + ident);
    };
};