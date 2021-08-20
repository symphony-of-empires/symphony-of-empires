#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "world.hpp"
#include "nation.hpp"

class Command {
public:
	// Change world state and send command to server
	void run_command(World& ws);	
};

// The all encompassing client state
// This is the state we could pass down to all the ui widgets
class GameState {
public:
	// The ui will mostly need to read the world state
	World ws;
	Nation::Id curr_nation;	
	Map map;

	// Ui calls add_command to set world state
	// Commands like set budget and move troops
	void add_command(Command command);
private:
	std::queue<Command> pending_commands;
};

// World tick and pending commands
void main_loop();
#endif