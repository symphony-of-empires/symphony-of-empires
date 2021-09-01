#ifndef DESCISION_H
#define DESCISION_H

#include "../game_state.hpp"
#include "../ui.hpp"
#include "../../industry.hpp"
#include "../../nation.hpp"
#include "../../world.hpp"
void create_descision(UI::Context* ui_ctx, Event& msg, std::vector<Event>& displayed_events);

#endif