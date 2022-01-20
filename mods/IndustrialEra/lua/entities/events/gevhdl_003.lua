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
--  	lua/events/gevhdl_003.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

function gevhdl003_test()
    if math.random(0, 100) == 0 then
        return EVENT_CONDITIONS_MET
    end
	return EVENT_CONDITIONS_UNMET
end
function gevhdl003_event(ref_name)
	descision = Descision:new{
		ref_name = "gevhdl003_descision_0",
		name = "Leave them be",
		descision_fn = "gevhdl003_descision_0",
		effects = "+0.50 con and +0.50 militancy nationwide"
	}
	gevhdl003:add_descision(descision)
    descision = Descision:new{
		ref_name = "gevhdl003_descision_1",
		name = "Take them down",
		descision_fn = "gevhdl003_descision_1",
		effects = "+0.75 militancy nationwide"
	}
	gevhdl003:add_descision(descision)
	return EVENT_DO_MANY_TIMES
end
function gevhdl003_descision_0(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
			pop.con = pop.con + 0.50
			pop.militancy = pop.militancy + 0.50
			province:update_pop(pop)
		end
		province:update_pops()
	end
end
function gevhdl003_descision_1(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
			pop.militancy = pop.militancy + 0.75
			province:update_pop(pop)
		end
		province:update_pops()
	end
end
gevhdl003 = Event:new{
	ref_name = "gevhdl003",
	conditions_fn = "gevhdl003_test",
	event_fn = "gevhdl003_event",
    title = "Stop the activists",
	text = "Some activists are trying to undermine us with false statments about the efficiency of our goverment - we can always crack down on those liars but we may upset free-speech supporters"
}
gevhdl003:register()
gevhdl003:add_receivers(table.unpack(Nation:get_all()))
