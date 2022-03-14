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
clipper = BoatType:new{ ref_name = "clipper", defense = 10.0, attack = 10.0, health = 100.0, speed = 0.5, capacity = 4000 }
clipper.name = _("Clipper")
clipper:register()
clipper:requires_good(arms, 800)
clipper:requires_good(sail, 20)
clipper:requires_good(ammunition, 200)

barque = BoatType:new{ ref_name = "barque", defense = 15.0, attack = 10.0, health = 200.0, speed = 0.5, capacity = 400 }
barque.name = _("Barque")
barque:register()
barque:requires_good(arms, 800)
barque:requires_good(sail, 20)
barque:requires_good(ammunition, 200)

sloop_of_war = BoatType:new{ ref_name = "sloop_of_war", defense = 40.0, attack = 20.0, health = 400.0, speed = 0.5, capacity = 400 }
sloop_of_war.name = _("Sloop of war")
sloop_of_war:register()
sloop_of_war:requires_good(timber, 1000)
sloop_of_war:requires_good(sail, 20)
sloop_of_war:requires_good(ammunition, 400)

man_of_war = BoatType:new{ ref_name = "man_of_war", defense = 20.0, attack = 20.0, health = 500.0, speed = 0.5, capacity = 500 }
man_of_war.name = _("Man of war")
man_of_war:register()
man_of_war:requires_good(arms, 1500)
man_of_war:requires_good(sail, 30)
man_of_war:requires_good(ammunition, 500)