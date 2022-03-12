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
--  	unit_traits.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Unit traits
-- TODO: Probably generals have them
fanatic = UnitTrait:new{ ref_name = "fanatic", attack_mod = 1.2 }
fanatic:register()

frugal = UnitTrait:new{ ref_name = "frugal", supply_consumption_mod = 0.7 }
frugal:register()

stupid = UnitTrait:new{ ref_name = "stupid", attack_mod = 1.5, defense_mod = 0.8, supply_consumption_mod = 1.4 }
stupid:register()

heroic = UnitTrait:new{ ref_name = "heroic", attack_mod = 0.7, defense_mod = 1.5 }
heroic:register()

iron_teeth = UnitTrait:new{ ref_name = "iron_teeth", supply_consumption_mod = 0.7 }
iron_teeth:register()

chargers = UnitTrait:new{ ref_name = "chargers", attack_mod = 2.0 }
chargers:register()

zealous = UnitTrait:new{ ref_name = "zealous", defense_mod = 2.0 }
zealous:register()

suicidal = UnitTrait:new{ ref_name = "suicidal", attack_mod = 5.0, defense_mod = 0.1, supply_consumption_mod = 5.0 }
suicidal:register()