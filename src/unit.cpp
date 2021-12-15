#include "unit.hpp"
#include "province.hpp"

std::pair<float, float> Unit::get_pos(void) const {
#if defined TILE_GRANULARITY
    return std::make_pair(x, y);
#else
    return std::make_pair(
        province->min_x + ((province->max_x - province->min_x) / 2.f),
        province->min_y + ((province->max_y - province->min_y) / 2.f)
    );
#endif
};