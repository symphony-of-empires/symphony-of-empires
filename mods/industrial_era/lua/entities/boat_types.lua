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
--  	boat_types.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Naval unit types
local v = {}
v=BoatType:new{ref_name="clipper",name=_("Clipper"),defense=10.000000,attack=10.000000,health=100.000000,speed=0.500000}
v:register()
v:requires_good(Good:get("arms"), 800.000000)
v:requires_good(Good:get("sail"), 20.000000)
v:requires_good(Good:get("ammunition"), 200.000000)
v=BoatType:new{ref_name="barque",name=_("Barque"),defense=15.000000,attack=10.000000,health=200.000000,speed=0.500000}
v:register()
v:requires_good(Good:get("arms"), 800.000000)
v:requires_good(Good:get("sail"), 20.000000)
v:requires_good(Good:get("ammunition"), 200.000000)
v=BoatType:new{ref_name="sloop_of_war",name=_("Sloop of war"),defense=40.000000,attack=20.000000,health=400.000000,speed=0.500000}
v:register()
v:requires_good(Good:get("timber"), 1000.000000)
v:requires_good(Good:get("sail"), 20.000000)
v:requires_good(Good:get("ammunition"), 400.000000)
v=BoatType:new{ref_name="man_of_war",name=_("Man of war"),defense=20.000000,attack=20.000000,health=500.000000,speed=0.500000}
v:register()
v:requires_good(Good:get("arms"), 1500.000000)
v:requires_good(Good:get("sail"), 30.000000)
v:requires_good(Good:get("ammunition"), 500.000000)
