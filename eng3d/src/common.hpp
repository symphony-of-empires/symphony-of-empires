#pragma once

namespace Eng3D {
    // Does the same as std::erase but doesn't keep the order
    template <typename C, typename T>
    inline void fast_erase(C& c, T value) noexcept {
        for(auto i = c.size(); i-- > 0; ) {
            if(c[i] == value) {
                c[i] = c.back();
                c.pop_back();
                return;
            }
        }
    }

    // Does the same as std::erase_all but doesn't keep the order
    template <typename C, typename T>
    inline void fast_erase_all(C& c, T value) noexcept {
        for(auto i = c.size(); i-- > 0; ) {
            if(c[i] == value) {
                c[i] = c.back();
                c.pop_back();
            }
        }
    }
}