#pragma once
#include <random>

#undef rot32
#define rot32(x,k) (((x)<<(k))|((x)>>(32-(k))))
namespace UnifiedRender {
    class rand {
    private:
        uint32_t a;
        uint32_t b;
        uint32_t c;
        uint32_t d;
    public:
        using result_type = uint32_t;

        rand() {}
        rand(uint32_t seed) {
            a = 0xf1ea5eed;
            b = c = d = seed;

            for(size_t i = 0; i < 20; ++i)
                (*this)();
        }
        rand(rand const& o) noexcept: a(o.a), b(o.b), c(o.c), d(o.d) {}
        rand(rand&& o) noexcept: a(o.a), b(o.b), c(o.c), d(o.d) {}
        constexpr static uint32_t max() { return std::numeric_limits<uint32_t>::max(); }
        constexpr static uint32_t min() { return std::numeric_limits<uint32_t>::min(); }
        uint32_t operator()() {
            uint32_t e = a - rot32(b, 27);
            a = b ^ rot32(c, 17);
            b = c + d;
            c = d + e;
            d = e + a;
            return d;
        }


        template<int32_t n>
        void advance_n() {
            for(int32_t i = n; i--; )
                this->operator()();
        }
        rand& operator=(rand const&) noexcept = default;
        rand& operator=(rand&&) noexcept = default;
    };

    rand& get_local_generator() {
        static thread_local rand local_generator(std::random_device{}());
        return local_generator;
    }
}
#undef rot32