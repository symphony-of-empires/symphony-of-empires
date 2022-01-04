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
--  	building_types.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Outpost types
barracks = BuildingType:new{ ref_name = "barracks", name = _("Barracks"), is_build_land_units = true, defense_bonus = 1.05 }
barracks:register()

fort = BuildingType:new{ ref_name = "fort", name = _("Fort"), defense_bonus = 1.45 }
fort:register()

seaport = BuildingType:new{ ref_name = "seaport", name = _("Seaport"), is_build_naval_units = true, defense_bonus = 1.05 }
seaport:register()

coast_cannons = BuildingType:new{ ref_name = "coast_cannons", name = _("Coast cannons"), defense_bonus = 1.01 }
coast_cannons:register()