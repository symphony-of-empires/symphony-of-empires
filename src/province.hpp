#ifndef PROVINCE_H
#define PROVINCE_H

#include <cstdint>
#include <set>
#include <string>
#include "pop.hpp"

typedef uint16_t ProvinceId;

class World;
class Nation;
class Industry;
class Product;

/**
 * A single province, which is used to simulate economy in a "bulk-tiles" way
 * instead of doing economical operations on every single tile
 */
class Province {
public:
	~Province();

	ProvinceId get_id(const World& world);
	void add_industry(World* world, Industry* industry);
	void remove_industry(World* world, Industry* industry);

	// Name of the province
	std::string name;

	// Reference name of the province
	std::string ref_name;

	// Color of the province, used for mapping the province's shape from the map_div.png file
	uint32_t color;

	// Budget of the province, money stored from taxes, to later be sent into
	// the nation's main wealth ark (or in case of multiple owners - a % of wealth)
	float budget;

	// Number of tiles this province takes
	size_t n_tiles = 0;

	// Rectangle coordinates (x,y - x,y) for "area" scanning a province when needed
	// (for example, when changing owners)
	size_t min_x = 65532;
	size_t min_y = 65532;
	size_t max_x = 0;
	size_t max_y = 0;
	
	// The (military) supply limit of the province, the max number of supplies there can be per tick
	float supply_limit;

	// The number of remaining (military) supplies in the province
	float supply_rem;
	
	// Number of workers available to work on a domestic factory
	size_t worker_pool;

	// The owner of this province
	Nation * owner = nullptr;

	// List containing all nations who have a nucleus in this province
	std::set<Nation *> nucleuses;

	// List of all neighbouring provinces (*should* be used for pathfinding)
	std::set<Province *> neighbours;
	
	// Each element of this list represents the availability of a product, taking as reference the
	// product list of the world. Indexes are 1:1 with the product list.
	std::vector<size_t> stockpile;
	
	// List of industries in the province
	std::vector<Industry *> industries;

	// List of products (produced here by factories) in this industry
	std::vector<Product *> products;

	// List of pops in this province
    std::vector<Pop> pops;

	// Calculates the total number of POPs in this province (total population)
	size_t total_pops(void) const;
};
template<>
class Serializer<Province> {
public:
	static constexpr bool is_const_size = false;
	static inline void serialize(Archive& output, const Province& obj) {
		::serialize(output, obj.name);
		::serialize(output, obj.ref_name);

		::serialize(output, obj.color);
		::serialize(output, obj.budget);
		::serialize(output, obj.n_tiles);

		::serialize(output, obj.min_x);
		::serialize(output, obj.min_y);
		::serialize(output, obj.max_x);
		::serialize(output, obj.max_y);

		::serialize(output, obj.supply_limit);
		::serialize(output, obj.supply_rem);
		::serialize(output, obj.worker_pool);
		::serialize(output, obj.owner);

		::serialize(output, obj.nucleuses);
		::serialize(output, obj.neighbours);
		::serialize(output, obj.stockpile);
		::serialize(output, obj.industries);
		::serialize(output, obj.products);
		::serialize(output, obj.pops);
	}
	static inline void deserialize(Archive& input, Province& obj) {
		::deserialize(input, obj.name);
		::deserialize(input, obj.ref_name);

		::deserialize(input, obj.color);
		::deserialize(input, obj.budget);
		::deserialize(input, obj.n_tiles);

		::deserialize(input, obj.min_x);
		::deserialize(input, obj.min_y);
		::deserialize(input, obj.max_x);
		::deserialize(input, obj.max_y);

		::deserialize(input, obj.supply_limit);
		::deserialize(input, obj.supply_rem);
		::deserialize(input, obj.worker_pool);
		::deserialize(input, obj.owner);

		::deserialize(input, obj.nucleuses);
		::deserialize(input, obj.neighbours);
		::deserialize(input, obj.stockpile);
		::deserialize(input, obj.industries);
		::deserialize(input, obj.products);
		::deserialize(input, obj.pops);
	}
	static inline size_t size(const Province& obj) {
		return serialized_size(obj.name)
			+ serialized_size(obj.ref_name)
			+ serialized_size(obj.color)
			+ serialized_size(obj.budget)
			+ serialized_size(obj.n_tiles)
			+ serialized_size(obj.min_x)
			+ serialized_size(obj.min_y)
			+ serialized_size(obj.max_x)
			+ serialized_size(obj.max_y)
			+ serialized_size(obj.supply_limit)
			+ serialized_size(obj.supply_rem)
			+ serialized_size(obj.worker_pool)
			+ serialized_size(obj.owner)
			+ serialized_size(obj.nucleuses)
			+ serialized_size(obj.neighbours)
			+ serialized_size(obj.stockpile)
			+ serialized_size(obj.industries)
			+ serialized_size(obj.products)
			+ serialized_size(obj.pops)
		;
	}
};

#endif
