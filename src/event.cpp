#include "event.hpp"
#include "world.hpp"

#include <algorithm>

void Event::take_descision(Nation* sender, Descision* dec) {
	auto receiver = std::find(receivers.begin(), receivers.end(), sender);
	
	// Confirm that the sender is in receiver's list
	if(receiver == receivers.end()) {
		// Not in receiver's list so it's not allowed
		return;
	}
	
	// Tell the world that we took a descision
	print_info("TODO: Serialize this and send it onto the server\n");
	g_world->taken_descisions.push_back(dec);
	
	// Remove from the receivers list so we don't duplicate descisions
	receivers.erase(receiver);
	
	// Remove from inbox too
	for(auto it = sender->inbox.begin(); it != sender->inbox.end(); it++) {
		if((*it).ref_name == this->ref_name) {
			sender->inbox.erase(it);
			break;
		}
	}
}
