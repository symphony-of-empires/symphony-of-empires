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
--  	terrain_types.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Terrain types
TerrainType:new{ref_name="deepsea",name=_("Deep sea"),color=0x0,is_water_body=true}:register()
TerrainType:new{ref_name="sea",name=_("Sea"),color=0xff,is_water_body=true}:register()
TerrainType:new{ref_name="plains",name=_("Plains"),color=0x10db32,is_water_body=false}:register()
TerrainType:new{ref_name="forest",name=_("Forest"),color=0x3c5e35,is_water_body=false}:register()
TerrainType:new{ref_name="jungle",name=_("Jungle"),color=0xff00,is_water_body=false}:register()
TerrainType:new{ref_name="desert",name=_("Desert"),color=0xfcff0f,is_water_body=false}:register()
TerrainType:new{ref_name="savanna",name=_("Savanna"),color=0xff970f,is_water_body=false}:register()
TerrainType:new{ref_name="taiga",name=_("Taiga"),color=0x47bf17,is_water_body=false}:register()
TerrainType:new{ref_name="tundra",name=_("Tundra"),color=0xffff,is_water_body=false}:register()
TerrainType:new{ref_name="artic",name=_("Artic"),color=0xffffff,is_water_body=false}:register()
TerrainType:new{ref_name="mountain",name=_("Mountain"),color=0xdbdbdb,is_water_body=false}:register()
