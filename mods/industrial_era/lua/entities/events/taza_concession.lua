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
--  	lua/events/taza_concession.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

function taza_concession_test()
    if math.random(0, 100) == 0 then
        return EVENT_CONDITIONS_MET
    end
	return EVENT_CONDITIONS_UNMET
end
function taza_concession_event(ref_name)
	decision = Decision:new{
		ref_name = "taza_concession_decision_0",
		name = "Taza is ours, by any means!",
		decision_fn = "taza_concession_decision_0",
		effects = "Annex morroco"
	}
	taza_concession:add_decision(decision)

    decision = Decision:new{
		ref_name = "taza_concession_decision_1",
		name = "Taza is rightful Spanish land!",
		decision_fn = "taza_concession_decision_1",
		effects = "War with morrocco"
	}
	taza_concession:add_decision(decision)

    decision = Decision:new{
		ref_name = "taza_concession_decision_2",
		name = "Maybe we can sort this out diplomatically?",
		decision_fn = "taza_concession_decision_2",
		effects = "+50 relations with Morrocco and France"
	}
	taza_concession:add_decision(decision)
	return EVENT_DO_ONE_TIME
end
function taza_concession_decision_0(ref_name)
	local nation = Nation:get("morrocco"):get_nuclei_provinces()
	for k, v in pairs(prov) do
		print(v.ref_name)
        v:give_and_relinquish_to(Nation:get("spain"))
	end
end
function taza_concession_decision_1(ref_name)
    Nation:get("spain"):declare_unjustified_war(Nation:get("morrocco"))
end
function taza_concession_decision_2(ref_name)
    local rel = {}
    
    rel = Nation:get("spain"):get_relation(Nation:get("morrocco"))
    rel.relation = rel.relation + 50.0
    Nation:get("spain"):set_relation(Nation:get("morrocco"), rel)

    rel = Nation:get("spain"):get_relation(Nation:get("france"))
    rel.relation = rel.relation + 50.0
    Nation:get("spain"):set_relation(Nation:get("france"), rel)
end
taza_concession = Event:new{
	ref_name = "taza_concession",
	conditions_fn = "taza_concession_test",
	event_fn = "taza_concession_event",
    title = "Taza concession",
	text = "We must retake the Morrocan lands to give spain a colonial hold over Africa!"
}
taza_concession:register()
taza_concession:add_receivers(Nation:get("spain"))
