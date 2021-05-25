#include "diplomacy.hpp"

namespace Diplomacy {
    bool is_friend(Nation * us, Nation * them) {
		const size_t idx = g_world->get_nation_id(them);
		const NationRelation * relation = &us->relations[idx];

		// A high relation means we are friendly <3
		if(relation->relation >= 50.f) {
			return true;
		} else {
			// Well, but maybe our interest is able to determine our friendliness towards them?
			if(relation->interest >= relation->relation) {
				// We cannot be friendly with negative relations
				if(relation->relation <= 0.f) {
					return false;
				}

				// Well, we need to be interested enough to like them
				if(relation->relation >= relation->interest / relation->relation) {
					return true;
				}
			}
			return false;
		}
	}

    bool is_foe(Nation * us, Nation * them) {
        return !is_friend(us, them);
    }
};