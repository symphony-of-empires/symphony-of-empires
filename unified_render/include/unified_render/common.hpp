#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <ctime>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <deque>
#include <atomic>
#include <thread>
#include <mutex>
#include <utility>
#include <numeric>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <random>

#ifdef _MSC_VER
typedef signed int ssize_t;
#endif

template <typename C, typename T>
void fast_erase(C& c, T value) noexcept {
    for(auto i = c.size(); i-- > 0; ) {
        if(c[i] == value) {
            c[i] = v.back();
            c.pop_back();
            return;
        }
    }
} 

template <typename C, typename T>
void fast_erase_all(C& c, T value) noexcept {
    for(auto i = c.size(); i-- > 0; ) {
        if(c[i] == value) {
            c[i] = v.back();
            c.pop_back();
        }
    }
} 


template<typename T>
constexpr inline T clamp(T n, T min, T max) {
    return std::min<T>(max, std::max<T>(n, min));
}

//source: Bob Jenkins
class prng {
private:
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
public:
    using result_type = uint32_t;

    prng() {}
    prng(uint32_t seed) {
        a = 0xf1ea5eed;
        b = c = d = seed;

        for(size_t i = 0; i < 20; ++i)
            (*this)();
    }
    prng(prng const& o) noexcept: a(o.a), b(o.b), c(o.c), d(o.d) {}
    prng(prng&& o) noexcept: a(o.a), b(o.b), c(o.c), d(o.d) {}
    constexpr static uint32_t max() { return std::numeric_limits<uint32_t>::max(); }
    constexpr static uint32_t min() { return std::numeric_limits<uint32_t>::min(); }
#undef rot32
#define rot32(x,k) (((x)<<(k))|((x)>>(32-(k))))
    uint32_t operator()() {
        uint32_t e = a - rot32(b, 27);
        a = b ^ rot32(c, 17);
        b = c + d;
        c = d + e;
        d = e + a;
        return d;
    }
#undef rot32


    template<int32_t n>
    void advance_n() {
        for(int32_t i = n; i--; )
            this->operator()();
    }
    prng& operator=(prng const&) noexcept = default;
    prng& operator=(prng&&) noexcept = default;
};

prng& get_local_generator() {
    static thread_local prng local_generator(std::random_device{}());
    return local_generator;
}
