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
--  	lua/events/province_renames.lua
--
-- 	Abstract:
--      Events for provinces/areas that changed their names historically
--      over the century.
-- ----------------------------------------------------------------------------

-- zapadnaya_alyaska -> Western Alaska
-- vnutrennyaya_alyaska -> Inner Alaska
-- novoarkhangelsk -> Sitka
-- kad_yak -> Kodiak
-- bolshoy_diomida -> Big Diomede
-- nunivok -> Nunivak Island
-- zaliv_norton -> Norton Sound
-- severnyy_chugach -> North Chugach
-- ostrov_nuchek -> Hichinbrook Island
-- poluostrov_alyaska -> Alaska Peninsula
-- krepost_ross -> Fort Ross
-- eastern_prairies -> Saskatchewan (after 1905)
-- bahia_de_guantanamo -> Guantamo Bay
-- sankt_croix -> Saint Croix (France), Saint Cross (English), Santa Cruz (Spanish)
-- hinche -> Hincha (Spanish)
-- sankt_barthélemy -> Saint Barthelemy
-- maroni -> Marowijne (Dutch/Suriname)
rename_events_data = {
    { language = "english", ref_name = "zapadnaya_alyaska", new_name = "Western Alaska" },
    { language = "english", ref_name = "vnutrennyaya_alyaska", new_name = "Inner Alaska" },
    { language = "english", ref_name = "novoarkhangelsk", new_name = "Sitka" },
    { language = "english", ref_name = "kad_yak", new_name = "Kodiak" },
    { language = "english", ref_name = "bolshoy_diomida", new_name = "Big Diomede" },
    { language = "english", ref_name = "novoarkhangelsk", new_name = "Sitka" },
    { language = "english", ref_name = "nunivok", new_name = "Nunivak Island" },
    { language = "english", ref_name = "zaliv_norton", new_name = "Norton Sound" },
    { language = "english", ref_name = "severnyy_chugach", new_name = "North Chugach" },
    { language = "english", ref_name = "ostrov_nuchek", new_name = "Hichinbrook Island" },
    { language = "english", ref_name = "poluostrov_alyaska", new_name = "Alaska Peninsula" },
    { language = "english", ref_name = "krepost_ross", new_name = "Fort Ross" },
    { language = "english", ref_name = "eastern_prairies", new_name = "Saskatchewan" },
    { language = "english", ref_name = "bahia_de_guantanamo", new_name = "Guantamo Bay" },
    { language = "french", ref_name = "sankt_croix", new_name = "Saint Croix" },
    { language = "english", ref_name = "sankt_croix", new_name = "Saint Cross" },
    { language = "spanish", ref_name = "sankt_croix", new_name = "Santa Cruz" },
    { language = "spanish", ref_name = "hinche", new_name = "Hincha" },
    { language = "dutch", ref_name = "maroni", new_name = "Marowijne" },
}

-- Generate all the evhdls
for k, v in pairs(rename_events_data) do
    v.evhdl_ref_name = v.language .. "_" .. v.ref_name .. "_rename_evhdl"
    rename_events_data[k] = v
end
for k, v in pairs(rename_events_data) do
    local evhdl = _G[v.evhdl_ref_name]
    evhdl = Event:new{
        ref_name = "zapadnaya_alyaska_rename",
        conditions_fn = function()
            local year = get_year()
            if Province:is_owned_by(Nation:any_of_language(Language:get("english"))) then
                return EVENT_CONDITIONS_MET
            end
            return EVENT_CONDITIONS_UNMET
        end,
        event_fn = function()
            local evhdl_data = rename_events_data[table.find_if(rename_events_data, function(e)
                return e.evhdl_ref_name == evhdl.evhdl_ref_name
            end)]
            evhdl:add_decision(Decision:new{
                name = "Yes",
                decision_fn = function()
                    Province:get(evhdl_data.ref_name):rename_to(evhdl_data.new_name)
                end,
                effects = "Rename " .. Province:get(evhdl_data):name
            })
            evhdl:add_decision(Decision:new{
                name = "No",
                decision_fn = function() end,
                effects = "None"
            })
            return EVENT_DO_ONE_TIME
        end,
        title = "Zapadnaya Alyaska",
        text = "..."
    }
    evhdl:register()
end
