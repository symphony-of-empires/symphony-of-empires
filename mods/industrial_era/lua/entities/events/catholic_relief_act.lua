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
--  	lua/events/catholic_relief_act.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Catholic Relief Act of 1829
-- https://en.wikipedia.org/wiki/Roman_Catholic_Relief_Act_1829
catholic_relief_act_of_1829 = Event:new{
	ref_name = "catholic_relief_act_of_1829",
	conditions_fn = function()
		local year = get_year()
		if year == 1829 and math.random(0, 100) == 0 then
			return EVENT_CONDITIONS_MET
		end
		return EVENT_CONDITIONS_UNMET
	end,
	event_fn = function(ref_name)
		catholic_relief_act_of_1829:add_decision(Decision:new{
			ref_name = "catholic_relief_act_of_1829_decision_0",
			name = "Yes, allow catholics on the parliament",
			decision_fn = function(ref_name)
				united_kingdom:add_accepted_religion(Religion:get("christianity"))
			end,
			effects = "None"
		})
		catholic_relief_act_of_1829:add_decision(Decision:new{
			ref_name = "catholic_relief_act_of_1829_decision_1",
			name = "No, the church and the state must remain separate",
			decision_fn = function(ref_name)
				local prov = Nation:get_owned_provinces()
				for k, province in pairs(prov) do
					local pops = province:get_pops()
					province:multiply_militancy_by_religion(Religion:get("christianity"), 1.5)
					province:update_pops()
				end
			end,
			effects = "+0.1 militancy for catholics"
		})
		return EVENT_DO_ONE_TIME
	end,
    title = "Catholic Relief Act of 1829",
	text = "Daniel O'Connell threatens insurrection in Ireland if the catholic relief act is not passed"
}
catholic_relief_act_of_1829:register()