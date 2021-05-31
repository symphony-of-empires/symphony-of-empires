#include "nation.hpp"
#include "world.hpp"

inline void Nation::increase_relation(const World& world, Nation * target) {
	const NationId t1_idx = world.get_id<NationId>(target, world.nations);
	this->relations[t1_idx].relation += 5.f;
	const NationId t2_idx = world.get_id<NationId>(this, world.nations);
	target->relations[t2_idx].relation += 5.f;
}

inline void Nation::decrease_relation(const World& world, Nation * target) {
	const NationId t1_idx = world.get_id<NationId>(target, world.nations);
	this->relations[t1_idx].relation += 5.f;
	const NationId t2_idx = world.get_id<NationId>(this, world.nations);
	target->relations[t2_idx].relation += 5.f;
}