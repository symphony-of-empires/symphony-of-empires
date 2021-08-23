#ifndef COMMAND_H
#define COMMAND_H

#include "actions.hpp"
#include "world.hpp"

class Command {
   public:
    Action action;
    virtual ~Command(){};
    // Change world state and send command to server
    virtual void run_command(World& ws) = 0;
};

#endif