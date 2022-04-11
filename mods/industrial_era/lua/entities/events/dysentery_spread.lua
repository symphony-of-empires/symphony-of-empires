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
--  	lua/events/dysentry_spread.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

function dysentery_test()
    if math.random(0, 100) == 0 then
        return EVENT_CONDITIONS_MET
    end
    return EVENT_CONDITIONS_UNMET
end
function dysentery_evhdl(ref_name)
    local r = math.random(0, 5)
    local prov_list = Nation:get(ref_name):get_owned_provinces()
    local n_prov_list = 0
    for _ in pairs(prov_list) do count = count + 1 end
    local random_province = prov_list[math.random(0, n_prov_list)]

    if r == 0 then
        dysentery_event.title = "Large dysentery Spread in " .. random_province.name
        dysentery_event.text = "Due to largely failed attempts in " .. random_province.name .. " to control dysentery, many people have simply been exposed to the disease, whether in the sewers"
    elseif r == 1 then
        dysentery_event.title = "Mass Spreading Event in " .. random_province.name
        dysentery_event.text = "A large party in ".. random_province.name .. " is believed to be one of the causes of a major spread of dysentery. The disease had a large semi-exposed sewer which could've helped spread the disease"
    elseif r == 3 then
        dysentery_event.title = "dysentery Dominates in ".. random_province.name
        dysentery_event.text = "A large dysentery outbreak in " .. random_province.name .. " has occured. Many have been infected"
    else 
        dysentery_event.title = "dysentery Ravages ".. random_province.name
        dysentery_event.text = "A strain of dysentery is ravaging the small villages of " .. random_province.name .." . This is proving to be a disaster as government officials in that area prepare a rescue mission"
    end
    
    decision = Decision:new{
        ref_name = "dysentery_evhdl_decision_0",
        name = "We have to deal with this outbreak.",
        decision_fn = "dysentery_evhdl_decision_0",
        effects = "+1 con"
    }
    decision = Decision:new{
        ref_name = "dysentery_evdhl_decision_1",
        name = "Send a small amount of aid to the area",
        decision_fn = "dysentery_evdhl_decision_1",
        effects = "+0.5 con"
    }
    decision = Decision:new{
        ref_name = "dysentery_evdhl_decision_2",
        name = "Eh, They can deal with it themselves",
        decision_fn = "dysentery_evdhl_decision_2",
        effects = "+1 militancy"
    }
    
    dysentery_evdhl:add_decision(decision)
    return EVENT_DO_MANY_TIMES
end

function dysentery_test2()
	if math.random(0, 100) == 0 then
        return EVENT_CONDITIONS_MET
    end
    return EVENT_CONDITIONS_UNMET
end
function dysentery_evhdl_2(ref_name)
    local r = math.random(0, 2)
    local prov_list = Nation:get(ref_name):get_owned_provinces()
    local n_prov_list = 0
    for _ in pairs(prov_list) do count = count + 1 end
    local random_province = prov_list[math.random(0, n_prov_list)]
	
	if r == 0 then
		dysentery_event_2.title = "Politicians in " .. random_province.name .." are believed to have dysentery"
		dysentery_event_2.text = "This is believed to have happened after a speaker at a town hall spreaded it to almost everybody there"
	elseif r == 1 then
		dysentery_event_2.title = "Politicians denounce measures taken to limit spread in " .. random_province.name
		dysentery_event_2.text = "Local Politicians in the province have said that the measures are unlawful and should be taken away. They have allowed citizens of their province to leave their villages freely, enabling a larger spread."
	elseif r == 2 then
		dysentery_event_2.title = "Politicians demand that measures to limit the spread of dysentery be taken down in " .. random_province.name
		dysentery_event_2.text = "Local Politicians in the province have made their case to the countries' highest political levels, and it's seems that some that are in high places believe the local politicians are right. This could enable a massive spread of dysentery."
    end
		
	decision = Decision:new{
		ref_name = "dysentery_evhdl_2_decision_0",
		name = "Fire Them! They have no place in our government",
		decision_fn = "dysentery_evhdl_2_decision_0",
		effects = "+1 militancy, +1 con"
	}
	decision = Decision:new{
		ref_name = "dysentery_evhdl_2_decision_1",
		name = "Ignorance is Bliss, Gentlemen",
		decision_fn = "dysentery_evhdl_2_decision_1",
		effects = "+2 con"
	}
dysentery_evdhl_2:add_decision(decision)
    return EVENT_DO_MANY_TIMES
end

function dysentery_evdhl_3(ref_name)
	local r = math.random(0, 2)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
    local n_prov_list = 0
    for _ in pairs(prov_list) do count = count + 1 end
    local random_province = prov_list[math.random(0, n_prov_list)]
	
	if r == 0 then
		dysentery_event_3.title = "Populace of " .. random_province.name .. " are angry at the government's poor response to the dysentery epidemic"
		dysentery_event_3.text = "Many people in that province are furious at our failed attempts to combat the epidemic. This may lead to severe repurcussions"
	else
		dysentery_event_3.title = "Large Scale Riots in " .. random_province.name
		dysentery_event_3.text = "Many in that province are rioting in the streets and towns and demanding better healthcare!"
    end

	decision = Decision:new{
		ref_name = "dysentery_evhdl_3_decision_0",
		name = "Send in Police to deal with them",
		decision = "dysentery_evhdl_3_decision_0",
		effects = "+1.5 militancy, +2 con"
    }
	
	decision = Decision:new{
		ref_name = "dysentery_evdhl_3_decision_1",
		name = "Fine, send aid",
		decision = "dysentery_evdhl_3_decision_1",
		effects = "-1 militancy, +3 con"
	}
    dysentery_evdhl_3:add_decision(decision)
	return EVENT_DO_MANY_TIMES
end