#ifndef ECONOMY_H
#define ECONOMY_H

#include "../world.hpp"

// Functions that do a economy simulation in various steps, divided to reduce
// overhead, they are distributed accross 48 ticks
namespace Economy {
    void do_phase_1(World& world);
    void do_phase_2(World& world);
    void do_phase_3(World& world);
    void do_phase_4(World& world);
}

#endif
