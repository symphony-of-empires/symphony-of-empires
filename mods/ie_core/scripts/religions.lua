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
--  	religions.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Religion
protestant = Religion:new{ ref_name = "protestant" }
protestant.name = _("Protestant")
protestant.color = 0x610b96
protestant:register()

catholic = Religion:new{ ref_name = "catholic" }
catholic.name = _("Catholic")
catholic.color = 0xf5db1b
catholic:register()

christian = Religion:new{ ref_name = "christian" }
christian.name = _("Christian")
christian.color = 0xe8de97
christian:register()

orthodox = Religion:new{ ref_name = "orthodox" }
orthodox.name = _("Orthodox")
orthodox.color = 0xf5db1b
orthodox:register()

islamic = Religion:new{ ref_name = "islamic" }
islamic.name = _("Islamic")
islamic.color = 0x487a4b
islamic:register()

buddhist = Religion:new{ ref_name = "buddhist" }
buddhist.name = _("Buddhist")
buddhist.color = 0xf5db1b
buddhist:register()

hunduism = Religion:new{ ref_name = "hunduism" }
hunduism.name = _("Hunduism")
hunduism.color = 0x1c9fc7
hunduism:register()

animist = Religion:new{ ref_name = "animist" }
animist.name = _("Animist")
animist.color = 0xa1b0c2
animist:register()

atheist = Religion:new{ ref_name = "atheist" }
atheist.name = _("Atheist")
atheist.color = 0x808080
atheist:register()

mapuche = Religion:new{ ref_name = "mapuche" }
mapuche.name = _("Mapuche")
mapuche.color = 0x8c9ebd
mapuche:register()

other = Religion:new{ ref_name = "other" }
other.name = _("Other")
other.color = 0x616161
other:register()