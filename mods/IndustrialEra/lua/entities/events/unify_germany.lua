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
--  	lua/events/unify_germany.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--
-- Unify germany "magic" event
--

function unify_germany_test()
	local year = get_year()
    if year >= 1840 and math.random(0, 100) == 0 then
        return EVENT_CONDITIONS_MET
    end
	return EVENT_CONDITIONS_UNMET
end
function unify_germany_event()
	descision = Descision:new{
		ref_name = "unify_germany_descision_0",
		name = "Yes",
		descision_fn = "unify_germany_descision_0",
		effects = "Prussia acquires all provinces that are a nuclei of Germany"
	}
	unify_germany_evhdl:add_descision(descision)
	
	descision = Descision:new{
		ref_name = "unify_germany_descision_1",
		name = "No",
		descision_fn = "unify_germany_descision_1",
		effects = "Nothing happens"
	}
	unify_germany_evhdl:add_descision(descision)
	return EVENT_DO_ONE_TIME
end
function unify_germany_descision_0(ref_name)
	print('Unifying germany...')
	Nation:get("germany"):set_ideology(Ideology:get("monarchy"))
	local prov = Nation:get("germany"):get_nuclei_provinces()
	for k, v in pairs(prov) do
		print(v.ref_name)
		v:give_and_relinquish_to(Nation:get("germany"))
	end

	Nation:get(ref_name):switch_player_to(Nation:get("germany"))
end
function unify_germany_descision_1()
	print('Germany isn\'t going to be a thing :<')
end
unify_germany_evhdl = Event:new{
	ref_name = "unify_germany", conditions_fn = "unify_germany_test", event_fn = "unify_germany_event",
	title = "Unify germany",
	text = "Make germany with magic!"
}
unify_germany_evhdl:register()
unify_germany_evhdl:add_receivers(Nation:get("prussia"))
