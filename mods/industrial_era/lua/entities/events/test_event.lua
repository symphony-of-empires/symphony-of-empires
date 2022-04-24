-- Industrial_Era_mod - Base game files for Symphony of Empires
-- Copyright (C) 2021, Symphony of Empires contributors
-- 
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
-- 
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License along
-- with this program; if not, write to the Free Software Foundation, Inc.,
-- 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
--
-- ----------------------------------------------------------------------------
--	Name:
--  	lua/events/test_event.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--
-- This is just a test event
-- 

function cake_test()
	-- Requirements
	return EVENT_CONDITIONS_MET
end
function cake_event(ref_name)
	decision = Decision:new{
		ref_name = "cake_decision_0",
		name = "I think i will eat it",
		decision_fn = "cake_decision_0",
		effects = "You will consume cake"
	}
	cake_evhdl:add_decision(decision)
	
	decision = Decision:new{
		ref_name = "cake_decision_1",
		name = "Fuck you",
		decision_fn = "cake_decision_1",
		effects = "You will consume cake anyways"
	}
	cake_evhdl:add_decision(decision)

	decision = Decision:new{
		ref_name = "cake_decision_2",
		name = "Cake!?",
		decision_fn = "cake_decision_2",
		effects = "Your compulsory desire for cake has began to grow"
	}
	cake_evhdl:add_decision(decision)

	decision = Decision:new{
		ref_name = "cake_decision_3",
		name = "Britain, we have a problem",
		decision_fn = "cake_decision_3",
		effects = "What the fuck is britain gonna do, invade us?"
	}
	cake_evhdl:add_decision(decision)
	return EVENT_DO_ONE_TIME
end
function cake_decision_0()
	print('The cake tasted like a cake, what a surprise! and you find a bunch of peseants outside your palace... oh no')
end
function cake_decision_1()
	print('Why no eat my poisoned cake? i put so much poison in it :(')
end
function cake_decision_2()
	print('Bruh, where the fuck am i?')
end
function cake_decision_3()
	print('Britain converted the cake into tea')
end
cake_evhdl = Event:new{
	ref_name = "cake_evhdl",
	conditions_fn = "cake_test",
	event_fn = "cake_event",
	title = "Cake",
	text = "Sir, a cake has just magically appeared in your desk, what would you like to do?"
}
cake_evhdl:register()
cake_evhdl:add_receivers(Nation:get("hawai_i"))
