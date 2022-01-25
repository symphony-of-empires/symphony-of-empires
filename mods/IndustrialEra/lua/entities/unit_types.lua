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
--militia = UnitType:new{ ref_name = "militia", defense = 1.5, attack = 0.5, health = 10.0, speed = 0.5 }
--militia.name = _("Militia")
--militia:register()
--militia:requires_good(ammunition, 200)
--militia:requires_good(arms, 100)

garrison = UnitType:new{ ref_name = "garrison", defense = 3.0, attack = 1.0, health = 100.0, speed = 0.5 }
garrison.name = _("Garrison")
garrison:register()
garrison:requires_good(ammunition, 200)
garrison:requires_good(arms, 100)

infantry = UnitType:new{ ref_name = "infantry", defense = 5.0, attack = 2.0, health = 100.0, speed = 0.5 }
infantry.name = _("Infantry")
infantry:register()
infantry:requires_good(ammunition, 200)
infantry:requires_good(arms, 100)

cavalry = UnitType:new{ ref_name = "cavalry", defense = 2.0, attack = 15.0, health = 50.0, speed = 0.5 }
cavalry.name = _("Cavalry")
cavalry:register()
cavalry:requires_good(ammunition, 200)
cavalry:requires_good(arms, 100)

artillery = UnitType:new{ ref_name = "artillery", defense = 2.0, attack = 15.0, health = 50.0, speed = 0.5 }
artillery.name = _("Artillery")
artillery:register()
artillery:requires_good(ammunition, 500)
artillery:requires_good(arms, 500)
