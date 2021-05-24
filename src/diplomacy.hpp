#ifndef DIPLOMACY_H
#define DIPLOMACY_H

#include "world.hpp"
#include "nation.hpp"

namespace Diplomacy {
	bool is_friend(Nation * us, Nation * them);
	bool is_foe(Nation * us, Nation * them);
};

#endif