#pragma once

class World;

// Functions that do a economy simulation in various steps, divided to reduce
// overhead, they are distributed accross 48 ticks
namespace Economy {
    void do_tick(World& world);
}