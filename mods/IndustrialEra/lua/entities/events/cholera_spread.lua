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
--  	lua/events/cholera_spread.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

function cholera_test()
    if math.random(0, 100) == 0 then
        return EVENT_CONDITIONS_MET
    end
    return EVENT_CONDITIONS_UNMET
end
function cholera_evhdl(ref_name)
    local prov_list = Nation:get(ref_name):get_owned_provinces()
    local n_prov_list = 0
    for _ in pairs(prov_list) do
        n_prov_list = n_prov_list + 1
    end
    local random_province = prov_list[math.random(0, n_prov_list)]

    local r = math.random(0, 5)
    if r == 0 then
        cholera_event.title = "Large Cholera Spread in " .. random_province.name
        cholera_event.text = "Due to largely failed attempts in " .. random_province.name .. " to control cholera, many people have simply been exposed to the disease, whether in the sewers"
    elseif r == 1 then
        cholera_event.title = "Mass Spreading Event in " .. random_province.name
        cholera_event.text = "A large party in ".. random_province.name .. " is believed to be one of the causes of a major spread of cholera. The disease had a large semi-exposed sewer which could've helped spread the disease"
    elseif r == 3 then
        cholera_event.title = "Cholera Dominates in ".. random_province.name
        cholera_event.text = "A large cholera outbreak in " .. random_province.name .. " has occured. Many have been infected"
    else 
        cholera_event.title = "Cholera Ravages ".. random_province.name
        cholera_event.text = "A strain of Cholera is ravaging the small villages of " .. random_province.name .." . This is proving to be a disaster as government officials in that area prepare a rescue mission"
    end
    
    decision = Decision:new{
        ref_name = "cholera_evhdl_decision_0",
        name = "We have to deal with this outbreak.",
        decision_fn = "cholera_evhdl_decision_0",
        effects = "+1 con"
    }
	cholera_evdhl:add_decision(decision)
    decision = Decision:new{
        ref_name = "cholera_evdhl_decision_1",
        name = "Send a small amount of aid to the area",
        decision_fn = "cholera_evdhl_decision_1",
        effects = "+0.5 con"
    }
	cholera_evdhl:add_decision(decision)
    decision = Decision:new{
        ref_name = "cholera_evdhl_decision_2",
        name = "Eh, They can deal with it themselves",
        decision_fn = "cholera_evdhl_decision_2",
        effects = "+1 militancy"
    }
    cholera_evdhl:add_decision(decision)
    return EVENT_DO_MANY_TIMES
end
function cholera_evhdl_decision_0(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
			pop.con = pop.con + 1.0
			province:update_pop(pop)
		end
		province:update_pops()
	end
end
function cholera_evdhl_decision_1(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
            pop.con = pop.con + 0.5
			province:update_pop(pop)
		end
		province:update_pops()
	end
end
function cholera_evdhl_decision_2(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
            pop.militancy = pop.militancy + 1.0
			province:update_pop(pop)
		end
		province:update_pops()
	end
end
cholera_evdhl = Event:new{
	ref_name = "cholera_evdhl",
	conditions_fn = "cholera_evdhl_test",
	event_fn = "cholera_evdhl_event",
    title = "",
	text = ""
}
cholera_evdhl:register()
cholera_evdhl:add_receivers(table.unpack(Nation:get_all()))

function cholera_test2()
	if math.random(0, 100) == 0 then
        return EVENT_CONDITIONS_MET
    end
    return EVENT_CONDITIONS_UNMET
end
function cholera_evhdl_2(ref_name)
    local prov_list = Nation:get(ref_name):get_owned_provinces()
    local n_prov_list = 0
    for _ in pairs(prov_list) do
        n_prov_list = n_prov_list + 1
    end
    local random_province = prov_list[math.random(0, n_prov_list)]

    local r = math.random(0, 2)
	if r == 0 then
		cholera_event_2.title = "Politicians in " .. random_province.name .." are believed to have cholera"
		cholera_event_2.text = "This is believed to have happened after a speaker at a town hall spreaded it to almost everybody there"
	elseif r == 1 then
		cholera_event_2.title = "Politicians denounce measures taken to limit spread in " .. random_province.name
		cholera_event_2.text = "Local Politicians in the province have said that the measures are unlawful and should be taken away. They have allowed citizens of their province to leave their villages freely, enabling a larger spread."
	elseif r == 2 then
		cholera_event_2.title = "Politicians demand that measures to limit the spread of cholera be taken down in " .. random_province.name
		cholera_event_2.text = "Local Politicians in the province have made their case to the countries' highest political levels, and it's seems that some that are in high places believe the local politicians are right. This could enable a massive spread of cholera."
    end
		
	decision = Decision:new{
		ref_name = "cholera_evhdl_2_decision_0",
		name = "Fire Them! They have no place in our government",
		decision_fn = "cholera_evhdl_2_decision_0",
		effects = "+1 militancy, +1 con"
	}
	cholera_evdhl_2:add_decision(decision)
	decision = Decision:new{
		ref_name = "cholera_evhdl_2_decision_1",
		name = "Ignorance is Bliss, Gentlemen",
		decision_fn = "cholera_evhdl_2_decision_1",
		effects = "+2 con"
	}
    cholera_evdhl_2:add_decision(decision)
    return EVENT_DO_MANY_TIMES
end
function cholera_evhdl_2_decision_0(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
            pop.militancy = pop.militancy + 1.0
			pop.con = pop.con + 1.0
			province:update_pop(pop)
		end
		province:update_pops()
	end
end
function cholera_evdhl_2_decision_1(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
            pop.con = pop.con + 2.0
			province:update_pop(pop)
		end
		province:update_pops()
	end
end
cholera_evdhl_2 = Event:new{
	ref_name = "cholera_evdhl_2",
	conditions_fn = "cholera_evdhl_2_test",
	event_fn = "cholera_evdhl_2_event",
    title = "",
	text = ""
}
cholera_evdhl_2:register()
cholera_evdhl_2:add_receivers(table.unpack(Nation:get_all()))

function cholera_evdhl_3(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
    local n_prov_list = 0
    for _ in pairs(prov_list) do
        n_prov_list = n_prov_list + 1
    end
    local random_province = prov_list[math.random(0, n_prov_list)]

    local r = math.random(0, 2)
	if r == 0 then
		cholera_event_3.title = "Populace of " .. random_province.name .. " are angry at the government's poor response to the cholera epidemic"
		cholera_event_3.text = "Many people in that province are furious at our failed attempts to combat the epidemic. This may lead to severe repurcussions"
	else
		cholera_event_3.title = "Large Scale Riots in " .. random_province.name
		cholera_event_3.text = "Many in that province are rioting in the streets and towns and demanding better healthcare!"
    end
		
	decision = Decision:new{
		ref_name = "cholera_evhdl_3_decision_0",
		name = "Send in Police to deal with them",
		decision = "cholera_evhdl_3_decision_0",
		effects = "+1.5 militancy, +2 con"
	}
	cholera_evdhl_3:add_decision(decision)
	decision = Decision:new{
		ref_name = "cholera_evdhl_3_decision_1",
		name = "Fine, send aid",
		decision = "cholera_evdhl_3_decision_1",
		effects = "-1 militancy, +3 con"
	}
    cholera_evdhl_3:add_decision(decision)
	return EVENT_DO_MANY_TIMES
end
function cholera_evhdl_3_decision_0(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
            pop.militancy = pop.militancy + 1.5
			pop.con = pop.con + 2.0
			province:update_pop(pop)
		end
		province:update_pops()
	end
end
function cholera_evdhl_3_decision_1(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
			pop.militancy = pop.militancy - 1.0
            pop.con = pop.con + 3.0
			province:update_pop(pop)
		end
		province:update_pops()
	end
end
cholera_evdhl_3 = Event:new{
	ref_name = "cholera_evdhl_3",
	conditions_fn = "cholera_evdhl_3_test",
	event_fn = "cholera_evdhl_3_event",
    title = "",
	text = ""
}
cholera_evdhl_3:register()
cholera_evdhl_3:add_receivers(table.unpack(Nation:get_all()))
