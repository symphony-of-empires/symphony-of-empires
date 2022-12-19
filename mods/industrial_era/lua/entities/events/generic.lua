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
--  	lua/events/generic.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

generic_infection_evhdl = Event:new{
    ref_name = "generic_infection",
    conditions_fn = function()
        if math.random(0, 350) == 0 then
            local prov_list = Nation:get(ref_name):get_owned_provinces()
            if table.size(prov_list) > 0 then
                return EVENT_CONDITIONS_MET
            end
        end
        return EVENT_CONDITIONS_UNMET
    end,
    event_fn = function(ref_name)
        local prov_list = Nation:get(ref_name):get_owned_provinces()
        local random_province = prov_list[math.random(1, table.size(prov_list))]
        local disease_names = {
            "Disentery",
            "Cholera",
            "Tuberculosis",
        }
        local disease_names_idx = math.random(1, table.size(disease_names))
        local disease_name = disease_names[disease_names_idx]
        local texts = {
            {
                title = "Large " .. disease_name .. " Spread in " .. random_province.name,
                text = "Due to largely failed attempts in " .. random_province.name .. " to control " .. disease_name .. ", many people have simply been exposed to the disease, whether in the sewers"
            }, {
                title = "Mass Spreading Event in " .. random_province.name,
                text = "A large party in ".. random_province.name .. " is believed to be one of the causes of a major spread of " .. disease_name .. ". The disease had a large semi-exposed sewer which could've helped spread the disease"
            }, {
                title = disease_name .. " dominates in ".. random_province.name,
                text = "A large " .. disease_name .. " outbreak in " .. random_province.name .. " has occured. Many have been infected"
            }, { 
                title = disease_name .. " ravages ".. random_province.name,
                text = "A strain of " .. disease_name .. " is ravaging the small villages of " .. random_province.name .." . This is proving to be a disaster as government officials in that area prepare a rescue mission"
            },
        }
        local texts_idx = math.random(1, table.size(texts))
        generic_infection_evhdl.title = texts[texts_idx].title
        generic_infection_evhdl.text = texts[texts_idx].title
        generic_infection_evhdl:update()
        generic_infection_evhdl:add_decision(Decision:new{
            name = "We have to deal with this outbreak.",
            decision_fn = function(ref_name)
                local prov_list = Nation:get(ref_name):get_owned_provinces()
                for k, province in pairs(prov_list) do
                    local pops = province:get_pops()
                    for k, pop in pairs(pops) do
                        pop.militancy = pop.militancy + 1.0
                        province:update_pop(pop)
                    end
                    province:update_pops()
                end
            end,
            effects = "+1 militancy"
        })
        generic_infection_evhdl:add_decision(Decision:new{
            name = "Send a small amount of aid to the area",
            decision_fn = function(ref_name)
                local prov_list = Nation:get(ref_name):get_owned_provinces()
                for k, province in pairs(prov_list) do
                    local pops = province:get_pops()
                    for k, pop in pairs(pops) do
                        pop.militancy = pop.militancy + 0.5
                        province:update_pop(pop)
                    end
                    province:update_pops()
                end
            end,
            effects = "+0.5 militancy"
        })
        generic_infection_evhdl:add_decision(Decision:new{
            name = "They can deal with it themselves",
            decision_fn = function(ref_name)
                local prov_list = Nation:get(ref_name):get_owned_provinces()
                for k, province in pairs(prov_list) do
                    local pops = province:get_pops()
                    for k, pop in pairs(pops) do
                        pop.militancy = pop.militancy + 1.0
                        province:update_pop(pop)
                    end
                    province:update_pops()
                end
            end,
            effects = "+1 militancy"
        })
        return EVENT_DO_MANY_TIMES
    end,
}
generic_infection_evhdl:register()
generic_infection_evhdl:add_receivers(table.unpack(Nation:get_all()))
