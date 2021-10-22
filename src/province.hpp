#ifndef PROVINCE_H
#define PROVINCE_H
#include <cstdint>
#include <vector>
#include <set>
#include <string>
#include "pop.hpp"

#include "entity.hpp"

class World;
class Nation;
class Industry;
class Product;
// A single province, which is used to simulate economy in a "bulk-tiles" way
// instead of doing economical operations on every single tile
class Province : public RefnameEntity<uint16_t> {
public:
    Nation& get_occupation_controller(void) const;
    size_t total_pops(void) const;
    std::vector<Product*> get_products(void) const;
    float get_attractive(const Pop& pop) const;

    // Color of the province, used for mapping the province's shape from the map_div.png file
    uint32_t color;

    // Budget of the province, money stored from taxes, to later be sent into
    // the nation's main wealth ark (or in case of multiple owners - a % of wealth)
    float budget;

    // Number of tiles this province takes
    size_t n_tiles = 0;

    // Rectangle coordinates (x,y - x,y) for "area" scanning a province when needed
    // (for example, when changing owners)
    size_t min_x = 65532, min_y = 65532, max_x = 0, max_y = 0;

    // The (military) supply limit of the province, the max number of supplies there can be per tick
    float supply_limit;

    // The number of remaining (military) supplies in the province
    float supply_rem;

    // Number of workers available to work on a domestic factory
    size_t worker_pool;

    // The owner of this province
    Nation* owner = nullptr;

    // List containing all nations who have a nucleus in this province
    std::set<Nation*> nucleuses;

    // List of all neighbouring provinces (*should* be used for pathfinding)
    std::set<Province*> neighbours;

    // Each element of this list represents the availability of a product, taking as reference the
    // product list of the world. Indexes are 1:1 with the product list.
    std::vector<size_t> stockpile;

    // List of products (produced here by factories) in this industry
    std::vector<Product*> products;

    // List of pops in this province
    std::vector<Pop> pops;

    // Attractiveness of province
    float base_attractive = 0.f;
};

#endif
