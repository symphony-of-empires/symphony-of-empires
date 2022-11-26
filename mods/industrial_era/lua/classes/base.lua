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
--      All the base classes/wrappers for interacting and initializing the
--      game data.
-- ----------------------------------------------------------------------------

Technology = {
	id = 0,
	ref_name = "",
	name = "",
	description = "",
	cost = 1.0,
	type = TECH_STRATEGIC
}
function Technology:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Technology:register()
	self.id = add_technology(self.ref_name, self.name, self.description, self.cost, self.type)
end
function Technology:get(o, ref_name)
	local o = Technology:new()
	o.id, o.name, o.description, o.type = get_technology(ref_name)
	o.ref_name = ref_name
	return o
end
function Technology:requires_technology(o)
	add_req_tech_to_tech(self.id, o.id)
end


Ideology = {
	id = 0,
	ref_name = "",
	name = "",
	color = 0x00000000
}
function Ideology:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Ideology:get(ref_name)
	local o = Ideology:new()
	o.id, o.name, o.color = get_ideology(ref_name)
	o.ref_name = ref_name
	return o
end
function Ideology:get_by_id(id)
	local o = Ideology:new()
	o.ref_name, o.name, o.color = get_ideology_by_id(id)
	o.id = id
	return o
end
function Ideology:register()
	self.id = add_ideology(self.ref_name, self.name, self.color)
end

Subideology = {
	ref_name = "",
	name = "",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = 0.0,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = 0.0,
}
function Subideology:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Ideology:add_subideology(s)
	add_ideology_subideology(self.id, s.ref_name, s.name, s.distributism, s.mercantilist, s.capitalism, s.individualism, s.state_power, s.equalitarianism, s.secular, s.pluralism)
end

Commodity = {
	id = 0,
	name = "",
	ref_name = ""
}
function Commodity:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Commodity:register()
	self.id = add_good(self.ref_name, self.name)
end
function Commodity:get(ref_name)
	local o = Commodity:new()
	o.id, o.name = get_good(ref_name)
	o.ref_name = ref_name
	return o
end

Province = {
	id = 0,
	name = "",
	ref_name = "",
	color = 0,
	terrain = {},
	rgo_size = {}
}
function Province:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Province:register()
	self.id = add_province(self.ref_name, self.color, self.name, self.terrain.id, self.rgo_size)
end
function Province:get(ref_name)
	local o = Province:new()
	local terrain_id = 0
	o.id, o.name, o.color, terrain_id, o.rgo_size = get_province(ref_name)
	o.terrain = TerrainType:get_by_id(terrain_id)
	o.ref_name = ref_name
	return o
end
function Province:get_by_id(id)
	local o = Province:new()
	local terrain_id = 0
	o.ref_name, o.name, o.color, terrain_id, o.rgo_size = get_province_by_id(id)
	o.terrain = TerrainType:get_by_id(terrain_id)
	o.id = id
	return o
end
function Province:add_unit(unit_type, size)
	province_add_unit(self.id, unit_type.id, size)
end
function Province:update_building(building_type, level)
	update_province_building(self.id, building_type.id, level)
end
function Province:give_to(nation)
	give_province_to(self.id, nation.id)
end
function Province:give_and_relinquish_to(nation)
	give_hard_province_to(self.id, nation.id)
end
function Province:get_owner()
	return Nation:get(get_province_owner(self.id))
end
function Province:get_controller()
	return Nation:get(get_province_controller(self.id))
end
function Province:get_neighbours()
	local table = get_province_neighbours(self.id)
	local new_table = {}
	for k, v in pairs(table) do
		new_table[k] = Province:get_by_id(v)
	end
	return new_table
end
function Province:get_nuclei()
	local table = get_province_nuclei(self.id)
	local new_table = {}
	for k, v in pairs(table) do
		new_table[k] = Nation:get_by_id(v)
	end
	return new_table
end
function Province:__get_pop_size()
 	return get_province_pops_size(self.id)
end
function Province:get_pops()
	local n_pops = get_province_pops_size(self.id) - 1
	local new_table = {}
	for i = 0, n_pops do
		local tb = Pop:new()
		tb.size, tb.budget, tb.literacy, tb.life_needs_met, tb.everday_needs_met, tb.luxury_needs_met, tb.type_id, tb.ideology_id, tb.militancy = get_province_pop(self.id, i)
		tb.id = i
		tb.province_id = self.id
		new_table[i] = tb
	end
	return new_table
end
function Province:get_total_population()
	local pops = self:get_pops()
	local sum = 0
	for k, v in pairs(pops) do
		sum = sum + v.size
	end
	return sum
end
function Province:update_pop(pop)
	set_province_pop(self.id, pop.id, pop.size, pop.budget, pop.literacy, pop.life_needs_met, pop.everday_needs_met, pop.luxury_needs_met, pop.type_id, pop.militancy)
end
function Province:update_pops(pop)
	-- TODO: Do important stuff
end
function Province:get_buildings()
	local n_buildings = get_province_buildings_size(self.id) - 1
	local new_table = {}
	for i = 0, n_buildings do
		local tb = Building:new()
		tb.level, tb.production_scale, tb.workers = get_province_building(self.id, i)
		tb.id = i
		tb.province_id = self.id
		new_table[i] = tb
	end
	return new_table
end
function Province:update_building(building)
	set_province_building(self.id, building.id, building.level, building.production_scale, building.workers)
end

-- ============================================================================
-- DEPRECATED do not use!!!!
-- ============================================================================

-- Increments militancy for all POPs
function Province:multiply_militancy(factor)
	local pops = self:get_pops()
	for k, pop in pairs(pops) do
		pop.militancy = pop.militancy * factor
		self:update_pop(pop)
	end
	self:update_pops()
end
function Province:multiply_militancy_by_language(language, factor)
	local pops = self:get_pops()
	for k, pop in pairs(pops) do
		-- if pop.language_id == language.id then
			pop.militancy = pop.militancy * factor
			self:update_pop(pop)
		-- end
	end
	self:update_pops()
end
function Province:multiply_militancy_by_religion(religion, factor)
	local pops = self:get_pops()
	for k, pop in pairs(pops) do
		-- if pop.religion_id == religion.id then
		 	pop.militancy = pop.militancy * factor
		 	self:update_pop(pop)
		-- end
	end
	self:update_pops()
end
-- ============================================================================

-- Adds a POP to the province
function Province:add_pop(pop_type, size, literacy)
	add_province_pop(self.id, pop_type.id, size, literacy)
end
-- Rename a province
function Province:rename(new_name)
	rename_province(self.id, new_name)
end
-- Adds a country to the nucleus list of a province
function Province:add_nucleus(nation)
	add_province_nucleus(self.id, nation.id)
end
function Province:set_language(language, pc)
	set_province_language(self.id, language.id, pc)
end
function Province:set_religion(religion, pc)
	set_province_religion(self.id, religion.id, pc)
end

function Province:is_owned_by(...)
	local nations = table.pack(...)
	print("TODO: Implement is_owned_by")
end
function Province:rename_to(new_name)
	print("TODO: Implement rename_to")
end


Nation = {
	id = 0,
	name = "",
	ref_name = "",

	adjective = "",
	noun = "",
	combo_form = ""
}
function Nation:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Nation:register()
	self.id = add_nation(self.ref_name, self.name)
end
function Nation:get(ref_name)
	local o = Nation:new()
	o.id, o.name = get_nation(ref_name)
	o.ref_name = ref_name
	return o
end
function Nation:get_by_id(id)
	local o = Nation:new()
	o.name, o.ref_name = get_nation_by_id(id)
	o.id = id
	return o
end
function Nation:set_capital(province)
	set_nation_capital(self.id, province.id)
end
function Nation:add_accepted_language(language)
	add_nation_accepted_language(self.id, language.id)
end
function Nation:add_accepted_religion(religion)
	add_nation_accepted_religion(self.id, religion.id)
end
function Nation:set_ideology(ideology, subidName)
	set_nation_ideology(self.id, ideology.id, subidName)
end
function Nation:add_client_hint(ideology, name, color)
	add_nation_client_hint(self.id, ideology.id, name, color)
end
function Nation:set_flag(name, value)
	if value == true then
		value = 1.0
	elseif value == false then
		value = 0.0
	end
	set_nation_flag(self.id, name, value)
end
function Nation:get_flag(name)
	return get_nation_flag(self.id, name)
end

function Nation:get_all()
	local table = get_all_nations()
	local new_table = {}
	for k, v in pairs(table) do
		new_table[k] = Nation:get_by_id(v)
	end
	return new_table
end

--- Switches the player of nation A to nation B
function Nation:switch_soul(target)
	switch_nation_soul(self.id, target.id)
end

--- Declares a war to another nation without a cb
--- @param other Nation to declare war to
function Nation:declare_no_cb(other)
	nation_declare_war_no_cb(self.id, other.id)
end

function Nation:get_owned_provinces()
	local table = get_provinces_owned_by_nation(self.id)
	local new_table = {}
	for k, v in pairs(table) do
		new_table[k] = Province:get_by_id(v)
	end
	return new_table
end
function Nation:get_nuclei_provinces()
	local table = get_provinces_with_nucleus_by_nation(self.id)
	local new_table = {}
	for k, v in pairs(table) do
		new_table[k] = Province:get_by_id(v)
	end
	return new_table
end
function Nation:is_owns_nuclei_from(other)
	local owned_table = Nation:get_owned_provinces(self)
	local nuclei_table = Nation:get_nuclei_provinces(other)

	-- Expected size from total counting
	local nuclei_cont = 0
	for _ in pairs(nuclei_table) do nuclei_cont = nuclei_cont + 1 end
	
	local total_eq_cont = 0
	for k1, v1 in pairs(owned_table) do
		for k2, v2 in pairs(nuclei_table) do
			if v1.ref_name == v2.ref_name then
				total_eq_cont = total_eq_cont + 1
			end
		end
	end
	return (total_eq_cont == nuclei_cont)
end
function Nation:relative_policy_stance(tp, v)
	return relative_nation_policy_stance(self.id, tp, v)
end

NationRelation = {
	relation = 0.0,
    has_war = false
}
function NationRelation:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Nation:get_relation(other)
	rel = NationRelation:new{} or {}
	rel.relation, rel.has_war = get_nation_relation(self.id, other.id)
	return rel
end
function Nation:set_relation(other, rel)
	get_nation_relation(self.id, other.id, rel.relation)
end
function Nation:make_puppet(other)
	nation_make_puppet(self.id, other.id)
end
function Nation:make_customs_union(other)
	nation_make_customs_union(self.id, other.id)
end
function Nation:declare_unjustified_war(other)
	nation_declare_unjustified_war(self.id, other.id)
end

function Nation:any_of_language(language)
	print("TODO: Implement any_of_language")
end

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

Building = {
    id = 0,
    province_id = 0,
    level = 0,
    production_scale = 0,
    workers = 0,
}
function Building:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

Event = {
	ref_name = "",
	conditions_fn = "",
	event_fn = "",
	title = "",
	text = "",
	checked = false,

	last_decision_id = 0,
}
function Event:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Event:register()
	self.id = add_event(self.ref_name, self.conditions_fn, self.event_fn, self.title, self.text, self.checked)
end
function Event:update()
	update_event(self.id, self.ref_name, self.conditions_fn, self.event_fn, self.title, self.text, self.checked)
end
function Event:get(ref_name)
	local o = Event:new()
	o.id, o.conditions_fn, o.event_fn, o.title, o.text, o.checked = get_event(ref_name)
	o.ref_name = ref_name
	return o
end
function Event:add_receivers(...)
	local args = table.pack(...)
	for i = 1, args.n do
		args[i] = args[i].id
	end
	add_event_receivers(self.id, args.n, table.unpack(args))
end
function Event:add_decision(decision)
	local decision_ref_name = self.ref_name .. self.last_decision_id
	self.last_decision_id = self.last_decision_id + 1.0

	add_decision(self.id, decision_ref_name, decision.name, decision.decision_fn, decision.effects)
end

Decision = {
	ref_name = "",
	decision_fn = "",
	name = "",
	effects = ""
}
function Decision:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

PopType = {
    id = 0,
    ref_name = "",
    name = "",
    social_value = 1.0,
    is_burgeoise = false,
    is_slave = false,
    is_laborer = false,
    is_soldier = false,
    is_artisan = false,
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
    o.id, o.name, o.social_value, o.is_burgeoise, o.is_slave, o.is_laborer, o.is_soldier, o.is_artisan, o.basic_needs, o.luxury_needs = get_pop_type(ref_name)
    o.ref_name = ref_name
    return o
end
function PopType:get_by_id(id)
    local o = PopType:new()
    o.ref_name, o.name, o.social_value, o.is_burgeoise, o.is_slave, o.is_laborer, o.is_soldier, o.is_artisan, o.basic_needs, o.luxury_needs = get_pop_type_by_id(id)
    o.id = id
    return o
end
function PopType:register()
    self.id = add_pop_type(self.ref_name, self.name, self.social_value, self.is_burgeoise, self.is_slave, self.is_laborer, self.is_soldier, self.is_artisan, self.basic_needs, self.luxury_needs)
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

BuildingType = {
	id = 0,
	name = "",
	ref_name = "",
	is_naval = false,
	is_build_land_units = false,
	is_build_naval_units = false,
	defense_bonus = 1.0,
	is_factory = false,
}
function BuildingType:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function BuildingType:register()
	self.id = add_building_type(self.ref_name, self.name, self.is_naval, self.is_build_land_units, self.is_build_naval_units, self.defense_bonus)
end
function BuildingType:get(ref_name)
	local o = BuildingType:new()
	o.id, o.name, o.is_naval, o.is_build_land_units, o.is_build_naval_units, o.defense_bonus = get_building_type(ref_name)
	o.ref_name = ref_name
	return o
end
function BuildingType:add_input(commodity)
	add_input_to_industry_type(self.id, commodity.id)
end
function BuildingType:add_output(commodity)
	add_output_to_industry_type(self.id, commodity.id)
end
function BuildingType:requires_good(commodity, amount)
	add_req_good_to_industry_type(self.id, commodity.id, amount)
end
function BuildingType:requires_tech(tech, amount)
	add_req_technology_to_industry_type(self.id, tech.id)
end

IndustryType = BuildingType
function IndustryType:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

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


UnitType = {
    id = 0,
    ref_name = "",
    name = "",
    health = 100.0,
    defense = 1.0,
    attack = 1.0,
    max_defensive_ticks = 25,
    position_defense = 0.1,
    is_ground = false,
    is_naval = false,
    speed = 1.0
}
function UnitType:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end
function UnitType:get(ref_name)
    local o = UnitType:new()
    o.id, o.name, o.attack, o.defense, o.health, o.is_ground, o.is_naval, o.speed = get_unit_type(ref_name)
    o.ref_name = ref_name
    return o
end
function UnitType:register()
    self.id = add_unit_type(self.ref_name, self.name, self.attack, self.defense, self.health, self.is_ground, self.is_naval, self.speed)
end
function UnitType:requires_good(commodity, amount)
    add_req_good_unit_type(self.id, commodity.id, amount)
end

BoatType = UnitType
BoatType.is_ground = false
BoatType.is_naval = true
function BoatType:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

AirplaneType = UnitType
AirplaneType.is_ground = true
AirplaneType.is_naval = true
function AirplaneType:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

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

Locale = {}
function Locale:is_vowel(c)
    return (c == 'a' or c == 'e' or c == 'i' or c == 'o' or c == 'u')
end

function Locale:get_noun(s)
    if type(s) == "string" then return s end
    return s.noun
end

function Locale:conjugate_related_and(...)
    local args = table.pack(...)

    local str = ""
    for i = 1, args.n do
        -- CF not available, fallback to an adjective
        if(is_empty(args[i].combo_form)) then
            str = str .. args[i].adjective
        -- Combining form available (only possible if it's first)
        elseif i == 1 then
            str = str .. args[i].combo_form
        end

        -- Makes somewhat correct phrases like Franco-Prussian war, Russo-Chinesse-Roman war
        if i < args.n then
            str = str .. "-"
        end
    end
    return str
end

-- Conjugates phrases so that they're presented as a list:
-- a, b, c, and d (w. Oxford comma)
function Locale:conjugate_comma_and(...)
    local args = table.pack(...)
    local str = ""
    for i = 1, args.n do
        str = str .. Locale:get_noun(args[i])
        if i < (args.n - 1) then
            str = str .. ", "
        elseif i < args.n then
            str = str .. ", and "
        end
    end
    return str
end

print(Locale:conjugate_comma_and("among", "us"))

-- [determiner (a/an)] [adjective (or noun as fallback)]
function Locale:conjugate_indefinite_article(o)
    local str = ""

    if(is_empty(o.adjective)) then
        str = str .. Locale:get_noun(o)
    else
        str = str .. o.adjective
    end

    if(Locale:is_vowel(str[0])) then
        str = "an" .. str
    else
        str = "a" .. str
    end
end

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

function table.remove_if(t, f)
    for k, v in pairs(t) do
        if f(v) == true then
            table.remove(t, k)
        end
    end
    return t
end

function table.find(t, _v)
    for k, v in pairs(t) do
        if v == _v then
            return k
        end
    end
    return nil
end

function table.find_if(t, f)
    for k, v in pairs(t) do
        if f(v) == true then
            return k
        end
    end
    return nil
end

function translate(str)
    -- TODO Translate
    return str
end

print("loaded api.lua")
