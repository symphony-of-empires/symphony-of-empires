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
jutland_sinks_evhdl = Event:new{
	ref_name = "jutland_sinks_evhdl",
	conditions_fn = function()
		if get_year() >= 1825 and get_month() >= 2 and math.random(0, 100) == 0 then

        end
		return EVENT_CONDITIONS_MET
	end,
	event_fn = function(ref_name)
		decision = Decision:new{
			ref_name = "jutland_sinks_decision_0",
			name = "That is very unfortunate",
			decision_fn = function(ref_name)
                -- None
			end,
			effects = "None"
		}
		jutland_sinks_evhdl:add_decision(decision)
		return EVENT_DO_ONE_TIME
	end,
	title = "Jutland sinks",
	text = "Vendsyssel-Thy is no longer connected to the Jutland peninsula due to a flood that separated it."
}
jutland_sinks_evhdl:register()
jutland_sinks_evhdl:add_receivers(Nation:get("denmark"))
