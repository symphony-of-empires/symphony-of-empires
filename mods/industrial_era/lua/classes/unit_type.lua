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
--  	unit_type.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

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
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function UnitType:get( ref_name)
	o = UnitType:new()
	o.id, o.name, o.attack, o.defense, o.health, o.max_defensive_ticks, o.position_defense, o.is_ground, o.is_naval, o.speed = get_unit_type(ref_name)
	o.ref_name = ref_name
	return o
end
function UnitType:register()
	self.id = add_unit_type(self.ref_name, self.name, self.attack, self.defense, self.health, self.max_defensive_ticks, self.position_defense, self.is_ground, self.is_naval, self.speed)
end
function UnitType:requires_good(good, amount)
	add_req_good_unit_type(self.id, good.id, amount)
end

BoatType = UnitType
BoatType.is_ground = false
BoatType.is_naval = true
function BoatType:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

AirplaneType = UnitType
AirplaneType.is_ground = true
AirplaneType.is_naval = true
function AirplaneType:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
