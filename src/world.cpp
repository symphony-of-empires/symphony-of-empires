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

// TODO: Do a template or something to remove code duplication
Nation::Id World::get_id(const Nation* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(nations_mutex);
    return get_id_ptr<Nation::Id>(ptr, nations);
}

Province::Id World::get_id(const Province* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(provinces_mutex);
    return get_id_ptr<Province::Id>(ptr, provinces);
}

Product::Id World::get_id(const Product* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(products_mutex);
    return get_id_ptr<Product::Id>(ptr, products);
}

Good::Id World::get_id(const Good* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(goods_mutex);
    return get_id_ptr<Good::Id>(ptr, goods);
}

Culture::Id World::get_id(const Culture* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(cultures_mutex);
    return get_id_ptr<Culture::Id>(ptr, cultures);
}

Company::Id World::get_id(const Company* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(companies_mutex);
    return get_id_ptr<Company::Id>(ptr, companies);
}

IndustryType::Id World::get_id(const IndustryType* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(industry_types_mutex);
    return get_id_ptr<IndustryType::Id>(ptr, industry_types);
}

Industry::Id World::get_id(const Province& province, const Industry* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(provinces_mutex);
    return ((ptrdiff_t)ptr - (ptrdiff_t)&province.industries[0]) / sizeof(Industry);
}

Event::Id World::get_id(const Event* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(events_mutex);
    return get_id_ptr<Event::Id>(ptr, events);
}

UnitType::Id World::get_id(const UnitType* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(unit_types_mutex);
    return get_id_ptr<UnitType::Id>(ptr, unit_types);
}

BoatType::Id World::get_id(const BoatType* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(boat_types_mutex);
    return get_id_ptr<BoatType::Id>(ptr, boat_types);
}

UnitTrait::Id World::get_id(const UnitTrait* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(unit_traits_mutex);
    return get_id_ptr<UnitTrait::Id>(ptr, unit_traits);
}

Unit::Id World::get_id(const Unit* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(units_mutex);
    return get_id_ptr<Unit::Id>(ptr, units);
}

Boat::Id World::get_id(const Boat* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(boats_mutex);
    return get_id_ptr<Boat::Id>(ptr, boats);
}

OutpostType::Id World::get_id(const OutpostType* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(outpost_types_mutex);
    return get_id_ptr<OutpostType::Id>(ptr, outpost_types);
}

Outpost::Id World::get_id(const Outpost* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(outposts_mutex);
    return get_id_ptr<Outpost::Id>(ptr, outposts);
}

Treaty::Id World::get_id(const Treaty* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(treaties_mutex);
    return get_id_ptr<Treaty::Id>(ptr, treaties);
}

size_t World::get_id(const Tile* ptr) const {
    std::lock_guard<std::recursive_mutex> lock(tiles_mutex);
    return ((ptrdiff_t)ptr - (ptrdiff_t)tiles) / sizeof(Tile);
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
