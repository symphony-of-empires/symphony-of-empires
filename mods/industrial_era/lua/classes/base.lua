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
--  	base.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

require('classes/technology')
require('classes/ideology')

Policies = {
    treatment = 0,
    migration = 0,
    immigration = 0,
    censorship = 0,
    build_infrastructure = 0,
    build_factories = 0,
    private_property = true,
    companies_allowed = true,
    public_education = true,
    secular_education = true,
    public_healthcare = true,
    social_security = true,
    slavery = true,
    legislative_parliament = true,
    executive_parliament = true,
    constitutional = true,
    foreign_trade = true,
    import_tax = 0.0,
    export_tax = 0.0,
    domestic_import_tax = 0.0,
    domestic_export_tax = 0.0,
    poor_flat_tax = 0.0,
    med_flat_tax = 0.0,
    rich_flat_tax = 0.0,
    industry_tax = 0.0,
    military_spending = 0.0,
    free_supplies = true,
    min_wage = 1.0,
    min_sv_for_parliament = 0.0
}
function Policies:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

require('classes/good')
require('classes/province')
require('classes/nation')

Pop = {
    id = 0,
    province_id = 0,
    size = 0,
    budget = 0,
    literacy = 0,
    life_needs_met = 0,
    everday_needs_met = 0,
    luxury_needs_met = 0,
    type = {},
    ideology = {},
    ideology_id = 0,
    militancy = 0,
    militancy = 0,
}
function Pop:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end
function Pop:get_ideology_approval(ideology)
    return get_province_pop_ideology_approval(self.province_id, self.id, ideology.id)
end
function Pop:set_ideology_approval(ideology, approval)
    set_province_pop_ideology_approval(self.province_id, self.id, ideology.id, approval)
end

require('classes/event')

PopType = {
    id = 0,
    ref_name = "",
    name = "",
    social_value = 1.0,
    is_burgeoise = false,
    is_slave = false,
    is_farmer = false,
    is_laborer = false,
    basic_needs = {},
    luxury_needs = {}
}
function PopType:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end
function PopType:get(ref_name)
    local o = PopType:new()
    o.id, o.name, o.social_value, o.is_burgeoise, o.is_slave, o.is_farmer, o.is_laborer, o.basic_needs, o.luxury_needs = get_pop_type(ref_name)
    o.ref_name = ref_name
    return o
end
function PopType:get_by_id(id)
    local o = PopType:new()
    o.ref_name, o.name, o.social_value, o.is_burgeoise, o.is_slave, o.is_farmer, o.is_laborer, o.basic_needs, o.luxury_needs = get_pop_type_by_id(id)
    o.id = id
    return o
end
function PopType:register()
    self.id = add_pop_type(self.ref_name, self.name, self.social_value, self.is_burgeoise, self.is_slave, self.is_farmer, self.is_laborer, self.basic_needs, self.luxury_needs)
end

Language = {
    id = 0,
    ref_name = "",
    name = "",
    adjective = "",
    noun = "",
    combo_form = "",
    color = 0x00000000
}
function Language:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end
function Language:get(ref_name)
    local o = Language:new()
    o.id, o.name, o.color, o.adjective, o.noun, o.combo_form = get_language(ref_name)
    o.ref_name = ref_name
    return o
end
function Language:get_by_id(id)
    local o = Language:new()
    o.ref_name, o.name, o.color, o.adjective, o.noun, o.combo_form = get_language_by_id(id)
    o.id = id
    return o
end
function Language:register()
    self.id = add_language(self.ref_name, self.name, self.color, self.adjective, self.noun, self.combo_form)
end

Religion = {
    id = 0,
    ref_name = "",
    name = "",
    color = 0x00000000
}
function Religion:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end
function Religion:get(ref_name)
    local o = Religion:new()
    o.id, o.name, o.color = get_religion(ref_name)
    o.ref_name = ref_name
    return o
end
function Religion:get_by_id(id)
    local o = Religion:new()
    o.ref_name, o.name, o.color = get_religion_by_id(id)
    o.id = id
    return o
end
function Religion:register()
    self.id = add_religion(self.ref_name, self.name, self.color)
end

require('classes/industry_type')

TerrainType = {
    id = 0,
    name = "",
    ref_name = "",
    color = 0x000000,
    penalty = 1.0,
    is_water_body = false
}
function TerrainType:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end
function TerrainType:register()
    self.id = add_terrain_type(self.ref_name, self.name, self.color, self.penalty, self.is_water_body)
end
function TerrainType:get(ref_name)
    local o = TerrainType:new()
    o.id, o.name, o.color, o.penalty, o.is_water_body = get_terrain_type(ref_name)
    o.ref_name = ref_name
    return o
end
function TerrainType:get_by_id(id)
    local o = TerrainType:new()
    o.ref_name, o.name, o.color, o.penalty, o.is_water_body = get_terrain_type_by_id(id)
    o.id = id
    return o
end

require('classes/unit_type')

-- For sanity
function rgb(r, g, b)
    local color = 0
    color = color | (r << 16)
    color = color | (g << 8)
    color = color | (b << 0)
    return color
end

function is_empty(s)
    return (s == nil or s == '')
end

-- Language = {}

-- function Language:is_vowel(c)
--     return (c == 'a' or c == 'e' or c == 'i' or c == 'o' or c == 'u')
-- end

-- function Language:conjugate_related_and(...)
--     local args = table.pack(...)

--     local str = ""
--     for i = 1, args.n do
--         -- CF not available, fallback to an adjective
--         if(is_empty(args[i].combo_form)) then
--             str = str .. args[i].adjective
--         -- Combining form available (only possible if it's first)
--         elseif i == 1 then
--             str = str .. args[i].combo_form
--         end

--         -- Makes somewhat correct phrases like Franco-Prussian war, Russo-Chinesse-Roman war
--         if i < (args.n - 1) then
--             str = str .. "-"
--         end
--     end
-- end

-- -- [determiner (a/an)] [adjective (or noun as fallback)]
-- function Language:conjugate_indefinite_article(o)
--     local str = ""

--     if(is_empty(o.adjective)) then
--         str = str .. o.noun
--     else
--         str = str .. o.adjective
--     end

--     if(Language:is_vowel(str[0])) then
--         str = "an" .. str
--     else
--         str = "a" .. str
--     end
-- end

function table.clone(org)
    return {table.unpack(org)}
end

-- Obtains the size of a table
function table.size(table)
    local n_items = 0
    for _ in pairs(table) do n_items = n_items + 1 end
    return n_items
end

-- Recursively copy a table
function table.deepcopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for k, v in next, orig, nil do
            copy[table.deepcopy(k)] = table.deepcopy(v)
        end
        setmetatable(copy, table.deepcopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end

print("loaded api.lua")
