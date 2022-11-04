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
--  	lua/events/gevhdl_002.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

gevhdl002_evhdl = Event:new{
	ref_name = "gevhdl002",
	conditions_fn = function()
		if math.random(0, 100) == 0 then
			return EVENT_CONDITIONS_MET
		end
		return EVENT_CONDITIONS_UNMET
	end,
	event_fn = function(ref_name)
		Nation:get(ref_name):set_flag("ideological_revolution_id", math.random(0, 6))

		local ideology = Ideology:get_by_id(Nation:get(ref_name):get_flag("ideological_revolution_id"))
		gevhdl002_evhdl.title = ideology.name .. " outrage!"
		gevhdl002_evhdl.text = "A lot of " .. ideology.name .. "s have started speaking false information about us; what's best for the " .. Nation:get(ref_name).adjective .. " people?"
		gevhdl002_evhdl:update()
		gevhdl002_evhdl:add_decision(Decision:new{
			ref_name = "gevhdl002_decision_0",
			name = "Shut them down",
			effects = "Every POP that supports " .. ideology.name .. " gets 0.1 militancy",
			decision_fn = function(ref_name)
				local prov_list = Nation:get(ref_name):get_owned_provinces()
				local ideology = Ideology:get_by_id(Nation:get(ref_name):get_flag("ideological_revolution_id"))
				for k, province in pairs(prov_list) do
					local pops = province:get_pops()
					for k, pop in pairs(pops) do
						if pop.ideology.ref_name == ideology.ref_name then
							goto continue
						end
						pop.militancy = pop.militancy + 0.1
						province:update_pop(pop)
						::continue::
					end
					province:update_pops()
				end
			end
		})
		gevhdl002_evhdl:add_decision(Decision:new{
			ref_name = "gevhdl002_decision_1",
			name = "Let them be",
			effects = "Every POP that supports " .. ideology.name .. " gets 0.05 militancy",
			decision_fn = function(ref_name)
				local prov_list = Nation:get(ref_name):get_owned_provinces()
				local ideology = Ideology:get_by_id(Nation:get(ref_name):get_flag("ideological_revolution_id"))
				for k, province in pairs(prov_list) do
					local pops = province:get_pops()
					for k, pop in pairs(pops) do
						if pop.ideology.ref_name == ideology.ref_name then
							goto continue
						end
						pop.militancy = pop.militancy + 0.05
						province:update_pop(pop)
						::continue::
					end
					province:update_pops()
				end
			end,
		})
		return EVENT_DO_MANY_TIMES
	end,
}
gevhdl002_evhdl:register()
gevhdl002_evhdl:add_receivers(table.unpack(Nation:get_all()))
