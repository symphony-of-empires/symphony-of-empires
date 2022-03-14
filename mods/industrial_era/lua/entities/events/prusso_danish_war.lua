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
--  	lua/events/prusso_danish_war.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--
-- Unify germany "magic" event
--

function prusso_danish_war_test()
	local year = get_year()
    if year >= 1820 and math.random(0, 100) == 0 then
        local has_german = false

        -- Only if denmark owns german lands
        local prov = Nation:get("denmark"):get_owned_provinces()
	    for k, province in pairs(prov) do
            local nucleus = province:get_nuclei()
            for k, v in pairs(nucleus) do
                if nucleus.ref_name == "germany" then
                    has_german = true
                    break
                end
            end
        end

        if has_german == true then
            return EVENT_CONDITIONS_MET
        else
            return EVENT_CONDITIONS_UNMET
        end
    end
	return EVENT_CONDITIONS_UNMET
end
function prusso_danish_war_event(ref_name)
	descision = Descision:new{
		ref_name = "prusso_danish_war_descision_0",
		name = "Yes",
		descision_fn = "prusso_danish_war_descision_0",
		effects = "+1.0 militancy on all German POPs, -1.0 militancy on all Danish POPs, +50 relations with Denmark"
	}
	prusso_danish_war_evhdl:add_descision(descision)
	
	descision = Descision:new{
		ref_name = "prusso_danish_war_descision_1",
		name = "No",
		descision_fn = "prusso_danish_war_descision_1",
		effects = "Declares war on Denmark"
	}
	prusso_danish_war_evhdl:add_descision(descision)
	return EVENT_DO_ONE_TIME
end
function prusso_danish_war_descision_0(ref_name)

end
function prusso_danish_war_descision_1(ref_name)
	Nation:get(ref_name):declare_unjustified_war(Nation:get("denmark"))
end

prusso_danish_war_evhdl = Event:new{
	ref_name = "prusso_danish_war", conditions_fn = "prusso_danish_war_test", event_fn = "prusso_danish_war_event",
	title = "The Holstein-Schweilg question",
	text = "Denmark owns lands which are rightfully part of our greater german state; should we let them own these and prevent the unification of all the germanic people?"
}
prusso_danish_war_evhdl:register()
prusso_danish_war_evhdl:add_receivers(Nation:get("prussia"))
