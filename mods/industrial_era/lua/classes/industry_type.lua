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
--  	industry_type.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

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
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function BuildingType:register()
	self.id = add_building_type(self.ref_name, self.name, self.is_naval, self.is_build_land_units, self.is_build_naval_units, self.defense_bonus, self.is_factory)
end
function BuildingType:get(ref_name)
	o = BuildingType:new()
	o.id, o.name, o.is_naval, o.is_build_land_units, o.is_build_naval_units, o.defense_bonus, o.is_factory = get_building_type(ref_name)
	o.ref_name = ref_name
	return o
end
function BuildingType:add_input(good)
	add_input_to_industry_type(self.id, good.id)
end
function BuildingType:add_output(good)
	add_output_to_industry_type(self.id, good.id)
end
function BuildingType:requires_good(good, amount)
	add_req_good_to_industry_type(self.id, good.id, amount)
end
function BuildingType:requires_tech(tech, amount)
	add_req_technology_to_industry_type(self.id, tech.id)
end

IndustryType = BuildingType
IndustryType.is_factory = true
function IndustryType:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
