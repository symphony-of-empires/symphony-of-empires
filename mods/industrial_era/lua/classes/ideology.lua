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
--  	ideology.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

Ideology = {
	id = 0,
	ref_name = "",
	name = "",
	color = 0x00000000
}
function Ideology:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Ideology:get(ref_name)
	o = Ideology:new()
	o.id, o.name, o.color = get_ideology(ref_name)
	o.ref_name = ref_name
	return o
end
function Ideology:get_by_id(id)
	o = Ideology:new()
	o.ref_name, o.name, o.color = get_ideology_by_id(id)
	o.id = id
	return o
end
function Ideology:register()
	self.id = add_ideology(self.ref_name, self.name, self.color)
end
