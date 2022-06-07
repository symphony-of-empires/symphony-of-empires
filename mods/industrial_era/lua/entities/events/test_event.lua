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
cake_evhdl = Event:new{
	ref_name = "cake_evhdl",
	conditions_fn = function()
		-- Requirements
		return EVENT_CONDITIONS_MET
	end,
	event_fn = function(ref_name)
		decision = Decision:new{
			ref_name = "cake_decision_0",
			name = "I think I will eat it",
			decision_fn = function(ref_name)
				print('The cake tasted like a cake, what a surprise! and you find a bunch of peseants outside your palace... oh no')
			end,
			effects = "You will consume cake"
		}
		cake_evhdl:add_decision(decision)
		
		decision = Decision:new{
			ref_name = "cake_decision_1",
			name = "Fuck you",
			decision_fn = function(ref_name)
				print('Why no eat my poisoned cake? i put so much poison in it :(')
			end,
			effects = "You will consume cake anyways"
		}
		cake_evhdl:add_decision(decision)
	
		decision = Decision:new{
			ref_name = "cake_decision_2",
			name = "Cake!?",
			decision_fn = function(ref_name)
				print('Where the fuck am i?')
			end,
			effects = "Your compulsory desire for cake has began to grow"
		}
		cake_evhdl:add_decision(decision)
	
		decision = Decision:new{
			ref_name = "cake_decision_3",
			name = "Britain, we have a problem",
			decision_fn = function(ref_name)
				print('Britain converted the cake into tea')
			end,
			effects = "What the fuck is britain gonna do, invade us?"
		}
		cake_evhdl:add_decision(decision)
		return EVENT_DO_ONE_TIME
	end,
	title = "Cake",
	text = "Sir, a cake has just magically appeared in your desk, what would you like to do?"
}
cake_evhdl:register()
cake_evhdl:add_receivers(Nation:get("hawai_i"))
