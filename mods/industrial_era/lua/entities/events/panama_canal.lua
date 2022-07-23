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
--  	lua/events/panama_canal.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Panama Canal lore

-- Holds the data for the panama canal campaing
CANAL_CAMPAING_DATA = {}

-- The Atlantic-Pacific trade route
-- Wanting to build the canal out of the blue :P
aptr_evhdl = Event:new{
	ref_name = "aptr_evhdl",
	conditions_fn = function()
		-- Requirements
		local year = get_year()
		if year >= 1850 and math.random(0, 100) == 0 then
			return EVENT_CONDITIONS_MET
		end
		return EVENT_CONDITIONS_UNMET
	end,
	event_fn = function(ref_name)
		local nation = Nation:get(ref_name)
		local canal_prov = Province:get("colon")
		-- TODO: Generate names dynamically based on culture (for multicultural states it's just random chance)
		CANAL_CAMPAING_DATA[ref_name] = {
			engineer = "Sr. ENGINEER_NAME",
			ambition = 0.0,
			with_plan = false
		}
		decision = Decision:new{
			ref_name = "dsc0",
			name = "You're right - contact the best engineers we have!",
			decision_fn = function(ref_name)
				CANAL_CAMPAING_DATA[ref_name].ambition = CANAL_CAMPAING_DATA[ref_name].ambition + 2.5
				print('Country ' .. ref_name .. ' called engineer ' .. CANAL_CAMPAING_DATA[ref_name].engineer)
			end,
			effects = "Call " .. CANAL_CAMPAING_DATA[ref_name].engineer .. " for the work"
		}
		aptr_evhdl:add_decision(decision)
		decision = Decision:new{
			ref_name = "dsc1",
			name = "You're right - but we should ask the " .. canal_prov:get_owner().adjective .. "s first",
			decision_fn = function(ref_name)
				CANAL_CAMPAING_DATA[ref_name].ambition = CANAL_CAMPAING_DATA[ref_name].ambition + 5.0
				print('Country ' .. ref_name .. ' asked the ' .. canal_prov:get_owner() .. ' for permission ')
			end,
			effects = "You will consume cake anyways"
		}
		aptr_evhdl:add_decision(decision)
		decision = Decision:new{
			ref_name = "dsc2",
			name = "We can't afford the canal at this moment",
			decision_fn = function(ref_name)
				CANAL_CAMPAING_DATA[ref_name].ambition = CANAL_CAMPAING_DATA[ref_name].ambition + 10.0
				print('Country ' .. ref_name .. ' rejected the canal proposal')
			end,
			effects = "Rejects the canal offer with no modifiers"
		}
		aptr_evhdl:add_decision(decision)
		return EVENT_DO_ONE_TIME
	end,
	title = "The Atlantic-Pacific trade route",
	text = "We are tired of passing thru the artic to reach the Pacific, we need an alternate trade route that can connect both Atlantic and Pacific oceans - we can of course choose any spot, however we need engineers that are skilled enough to build an efficient and cheap canal"
}
aptr_evhdl:register()
aptr_evhdl:add_receivers(table.unpack(Nation:get_all()))

-- Engineer answers to our proposal and approves the panama canal proposal
eapcp_evhdl = Event:new{
	ref_name = "eapcp",
	conditions_fn = function()
		-- TODO: Requirements
		if CANAL_CAMPAING_DATA[ref_name] then
			return EVENT_CONDITIONS_MET
		end
		return EVENT_CONDITIONS_UNMET
	end,
	event_fn = function(ref_name)
		local nation = Nation:get(ref_name)
		local canal_prov = Province:get("colon")
		decision = Decision:new{
			ref_name = "dsc0",
			name = "We're grateful of your ingenous ideas",
			decision_fn = function(ref_name)
				CANAL_CAMPAING_DATA[ref_name].ambition = CANAL_CAMPAING_DATA[ref_name].ambition + 5.0
				CANAL_CAMPAING_DATA[ref_name].with_plan = true
			end,
			effects = "Start the preparations for a canal on Panama"
		}
		eapcp_evhdl:add_decision(decision)
		decision = Decision:new{
			ref_name = "dsc1",
			name = "This plan is too much for us!",
			decision_fn = function(ref_name)
				CANAL_CAMPAING_DATA[ref_name].ambition = CANAL_CAMPAING_DATA[ref_name].ambition + 50.0
				print('Country ' .. ref_name .. ' rejected the canal proposal')
			end,
			effects = "Panama canal idea is aborted"
		}
		eapcp_evhdl:add_decision(decision)
		return EVENT_DO_ONE_TIME
	end,
	title = "Engineer proposes Panama Canal",
	text = "Our engineer contacted us and gave us a plan to terraform a canal in Panama, what should we do?"
}
eapcp_evhdl:register()
eapcp_evhdl:add_receivers(table.unpack(Nation:get_all()))

eapcp_evhdl = Event:new{
	ref_name = "eapcp",
	conditions_fn = function()
		local year = get_year()
		if year >= 1850 and math.random(0, 100) == 0 then
			return EVENT_CONDITIONS_MET
		end
		return EVENT_CONDITIONS_UNMET
	end,
	event_fn = function(ref_name)
		local nation = Nation:get(ref_name)
		local canal_prov = Province:get("colon")
		eapcp_evhdl.title = "The country of " .. ref_name
		eapcp_evhdl.text = "We can decide stuff, what we should do with the country of " .. Nation:get(ref_name).adjective .. " people?"
		eapcp_evhdl:update(eapcp_evhdl)
	
		decision = Decision:new{
			ref_name = "dsc0",
			name = "Something very smart",
			decision_fn = function(ref_name)
				print("Dec 0: " .. ref_name)
			end,
			effects = "Smart stuff"
		}
		eapcp_evhdl:add_decision(decision)
		
		decision = Decision:new{
			ref_name = "dsc1",
			name = "Something very ambitious",
			decision_fn = function(ref_name)
				print("Dec 1: " .. ref_name)
			end,
			effects = "Ambitious stuff"
		}
		eapcp_evhdl:add_decision(decision)
		return EVENT_DO_ONE_TIME
	end
}
eapcp_evhdl:register()
eapcp_evhdl:add_receivers(table.unpack(Nation:get_all()))

-- The [OWNER_NOUN]s  
-- 
