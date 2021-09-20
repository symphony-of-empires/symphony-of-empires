#include "world.hpp"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <set>
#ifndef _MSC_VER
#	include <sys/cdefs.h>
#endif

#include "province.hpp"
#include "binary_image.hpp"
#include "path.hpp"
#include "print.hpp"
#include "serializer.hpp"
#include "server/server_network.hpp"
#include "diplomacy.hpp"
#include "policy.hpp"

World* g_world;

World& World::get_instance(void) {
    return *g_world;
}

// Obtains a tile from the world safely, and makes sure that it is in bounds
Tile& World::get_tile(size_t x, size_t y) const {
    std::lock_guard<std::recursive_mutex> lock(tiles_mutex);
    if(x >= width || y >= height)
        throw std::runtime_error("Tile out of bounds");
    return tiles[x + y * width];
}

Tile& World::get_tile(size_t idx) const {
    std::lock_guard<std::recursive_mutex> lock(tiles_mutex);
    if(idx >= width * height)
        throw std::runtime_error("Tile index exceeds boundaries");
    return tiles[idx];
}
