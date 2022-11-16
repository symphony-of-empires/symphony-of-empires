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
--  	unit_types.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Unit types
local v = {}
v=UnitType:new{ref_name="garrison",name=_("Garrison"),defense=1.300000,attack=1.100000,health=100.000000,speed=0.500000}
v:register()
v:requires_good(Commodity:get("arms"), 1.000000)
v=UnitType:new{ref_name="infantry",name=_("Infantry"),defense=1.500000,attack=1.200000,health=100.000000,speed=0.500000}
v:register()
v:requires_good(Commodity:get("arms"), 1.000000)
v=UnitType:new{ref_name="cavalry",name=_("Cavalry"),defense=1.200000,attack=1.150000,health=50.000000,speed=0.500000}
v:register()
v:requires_good(Commodity:get("arms"), 2.000000)
v=UnitType:new{ref_name="artillery",name=_("Artillery"),defense=1.200000,attack=1.150000,health=50.000000,speed=0.500000}
v:register()
v:requires_good(Commodity:get("arms"), 4.000000)