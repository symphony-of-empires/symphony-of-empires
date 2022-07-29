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
--  	province.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

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
		tb.size, tb.budget, tb.literacy, tb.life_needs_met, tb.everday_needs_met, tb.luxury_needs_met, tb.type_id, tb.culture_id, tb.religion_id, tb.ideology_id, tb.militancy = get_province_pop(self.id, i)
		tb.id = i
		tb.province_id = self.id
		new_table[i] = tb
	end
	return new_table
end
function Province:update_pop(pop)
	set_province_pop(self.id, pop.id, pop.size, pop.budget, pop.literacy, pop.life_needs_met, pop.everday_needs_met, pop.luxury_needs_met, pop.type_id, pop.culture_id, pop.religion_id, pop.militancy)
end
function Province:update_pops(pop)
	-- TODO: Do important stuff
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
function Province:multiply_militancy_by_culture(culture, factor)
	local pops = self:get_pops()
	for k, pop in pairs(pops) do
		if pop.culture_id == culture.id then
			pop.militancy = pop.militancy * factor
			self:update_pop(pop)
		end
	end
	self:update_pops()
end
function Province:multiply_militancy_by_religion(religion, factor)
	local pops = self:get_pops()
	for k, pop in pairs(pops) do
		if pop.religion_id == religion.id then
			pop.militancy = pop.militancy * factor
			self:update_pop(pop)
		end
	end
	self:update_pops()
end
-- ============================================================================

-- Adds a POP to the province
function Province:add_pop(pop_type, culture, religion, size, literacy)
	add_province_pop(self.id, pop_type.id, culture.id, religion.id, size, literacy)
end
-- Rename a province
function Province:rename(new_name)
	rename_province(self.id, new_name)
end
-- Adds a country to the nucleus list of a province
function Province:add_nucleus(nation)
	add_province_nucleus(self.id, nation.id)
end
