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
function cholera_evhdl_event(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
    local n_prov_list = 0
    for _ in pairs(prov_list) do
        n_prov_list = n_prov_list + 1
    end
    local random_province = prov_list[math.random(0, n_prov_list)]

	local disease_names = {
		"Disentery",
		"Cholera",
		"Tuberculosis",
	}
	local disease_names_idx = math.random(1, 3)
	local texts = {
		{
			title = "Large " .. disease_names[disease_names_idx] .. " Spread in " .. random_province.name,
			text = "Due to largely failed attempts in " .. random_province.name .. " to control " .. disease_names[disease_names_idx] .. ", many people have simply been exposed to the disease, whether in the sewers"
		}, {
			title = "Mass Spreading Event in " .. random_province.name,
			text = "A large party in ".. random_province.name .. " is believed to be one of the causes of a major spread of " .. disease_names[disease_names_idx] .. ". The disease had a large semi-exposed sewer which could've helped spread the disease"
		}, {
			title = disease_names[disease_names_idx] .. "Dominates in ".. random_province.name,
			text = "A large " .. disease_names[disease_names_idx] .. " outbreak in " .. random_province.name .. " has occured. Many have been infected"
		}, { 
			title = disease_names[disease_names_idx] .. " Ravages ".. random_province.name,
			text = "A strain of " .. disease_names[disease_names_idx] .. " is ravaging the small villages of " .. random_province.name .." . This is proving to be a disaster as government officials in that area prepare a rescue mission"
		},
	}
	local texts_idx = math.random(1, 4)

	cholera_evhdl.title = texts[text_idx].title
	cholera_evhdl.text = texts[text_idx].title
	cholera_evhdl:update()
    
    decision = Decision:new{
        ref_name = "cholera_decision_0",
        name = "We have to deal with this outbreak.",
        decision_fn = "cholera_decision_0",
        effects = "+1 con"
    }
	cholera_evhdl:add_decision(decision)
    decision = Decision:new{
        ref_name = "cholera_decision_1",
        name = "Send a small amount of aid to the area",
        decision_fn = "cholera_decision_1",
        effects = "+0.5 con"
    }
	cholera_evhdl:add_decision(decision)
    decision = Decision:new{
        ref_name = "cholera_decision_2",
        name = "Eh, They can deal with it themselves",
        decision_fn = "cholera_decision_2",
        effects = "+1 militancy"
    }
    cholera_evhdl:add_decision(decision)
    return EVENT_DO_MANY_TIMES
end
function cholera_decision_0(ref_name)
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
function cholera_decision_1(ref_name)
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
function cholera_decision_2(ref_name)
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
cholera_evhdl = Event:new{
	ref_name = "cholera_evhdl",
	conditions_fn = "cholera_test",
	event_fn = "cholera_event",
    title = "",
	text = ""
}
cholera_evhdl:register()
cholera_evhdl:add_receivers(table.unpack(Nation:get_all()))

function cholera_2_test()
	if math.random(0, 100) == 0 then
        return EVENT_CONDITIONS_MET
    end
    return EVENT_CONDITIONS_UNMET
end
function cholera_2_event(ref_name)
    local prov_list = Nation:get(ref_name):get_owned_provinces()
    local n_prov_list = 0
    for _ in pairs(prov_list) do
        n_prov_list = n_prov_list + 1
    end
    local random_province = prov_list[math.random(0, n_prov_list)]

	local disease_names = {
		"Disentery",
		"Cholera",
		"Tuberculosis",
	}
	local disease_names_idx = math.random(1, 3)

	local texts = {
		{
			title = "Politicians in " .. random_province.name .." are believed to have " .. disease_names[disease_names_idx],
			text = "This is believed to have happened after a speaker at a town hall spreaded it to almost everybody there"
		}, {
			title = "Politicians denounce measures taken to limit spread in " .. random_province.name,
			text = "Local Politicians in the province have said that the measures are unlawful and should be taken away. They have allowed citizens of their province to leave their villages freely, enabling a larger spread."
		}, {
			title = "Politicians demand that measures to limit the spread of " .. disease_names[disease_names_idx] .. " be taken down in " .. random_province.name,
			text = "Local Politicians in the province have made their case to the countries' highest political levels, and it's seems that some that are in high places believe the local politicians are right. This could enable a massive spread of " .. disease_names[disease_names_idx] .. "."
		},
	}
	local texts_idx = math.random(1, 3)

	cholera_evhdl_2.title = texts[text_idx].title
	cholera_evhdl_2.text = texts[text_idx].title
	cholera_evhdl_2:update()
		
	decision = Decision:new{
		ref_name = "cholera_2_decision_0",
		name = "Fire Them! They have no place in our government",
		decision_fn = "cholera_2_decision_0",
		effects = "+1 militancy, +1 con"
	}
	cholera_evhdl_2:add_decision(decision)
	decision = Decision:new{
		ref_name = "cholera_2_decision_1",
		name = "Ignorance is Bliss, Gentlemen",
		decision_fn = "cholera_2_decision_1",
		effects = "+2 con"
	}
    cholera_evhdl_2:add_decision(decision)
    return EVENT_DO_MANY_TIMES
end
function cholera_2_decision_0(ref_name)
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
function cholera_2_decision_1(ref_name)
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
cholera_evhdl_2 = Event:new{
	ref_name = "cholera_2",
	conditions_fn = "cholera_2_test",
	event_fn = "cholera_2_event",
    title = "",
	text = ""
}
cholera_evhdl_2:register()
cholera_evhdl_2:add_receivers(table.unpack(Nation:get_all()))

function cholera_3_test()
	if math.random(0, 100) == 0 then
        return EVENT_CONDITIONS_MET
    end
    return EVENT_CONDITIONS_UNMET
end
function cholera_3_event(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
    local n_prov_list = 0
    for _ in pairs(prov_list) do
        n_prov_list = n_prov_list + 1
    end
    local random_province = prov_list[math.random(0, n_prov_list)]

	local disease_names = {
		"Disentery",
		"Cholera",
		"Tuberculosis",
	}
	local disease_names_idx = math.random(1, 3)

	local texts = {
		{
			title = "Populace of " .. random_province.name .. " are angry at the government's poor response to the " .. disease_names[disease_names_idx] .. " epidemic",
			text = "Many people in that province are furious at our failed attempts to combat the epidemic. This may lead to severe repurcussions"
		}, {
			title = "Large Scale Riots in " .. random_province.name,
			text = "Many in that province are rioting in the streets and towns and demanding better healthcare!"
		}
	}
	local texts_idx = math.random(1, 2)

	cholera_evhdl_3.title = texts[text_idx].title
	cholera_evhdl_3.text = texts[text_idx].title
	cholera_evhdl_3:update()
		
	decision = Decision:new{
		ref_name = "cholera_3_decision_0",
		name = "Send in Police to deal with them",
		decision = "cholera_3_decision_0",
		effects = "+1.5 militancy, +2 con"
	}
	cholera_evhdl_3:add_decision(decision)
	decision = Decision:new{
		ref_name = "cholera_3_decision_1",
		name = "Fine, send aid",
		decision = "cholera_3_decision_1",
		effects = "-1 militancy, +3 con"
	}
    cholera_evhdl_3:add_decision(decision)
	return EVENT_DO_MANY_TIMES
end
function cholera_3_decision_0(ref_name)
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
function cholera_3_decision_1(ref_name)
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
cholera_evhdl_3 = Event:new{
	ref_name = "cholera_3",
	conditions_fn = "cholera_3_test",
	event_fn = "cholera_3_event",
    title = "",
	text = ""
}
cholera_evhdl_3:register()
cholera_evhdl_3:add_receivers(table.unpack(Nation:get_all()))
