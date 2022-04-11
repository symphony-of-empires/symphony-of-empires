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

hinduism = Religion:new{ ref_name = "hinduism" }
hinduism.name = _("Hinduism")
hinduism.color = 0x1c9fc7
hinduism:register()

taoism = Religion:new{ ref_name = "taoism" }
taoism.name = _("Taoism")
taoism.color = 0x1c9fc7
taoism:register()

atheist = Religion:new{ ref_name = "atheist" }
atheist.name = _("Atheist")
atheist.color = 0xd2a5d4
atheist:register()

other = Religion:new{ ref_name = "other" }
other.name = _("Other")
other.color = 0x616161
other:register()