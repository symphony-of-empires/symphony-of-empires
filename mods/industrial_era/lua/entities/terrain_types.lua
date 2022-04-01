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
deepsea = TerrainType:new{ ref_name = "deepsea", name = _("Deep sea"), is_water_body = true }
deepsea:register()

sea = TerrainType:new{ ref_name = "sea", name = _("Sea"), color = 0x0000ff, is_water_body = true }
sea:register()

plains = TerrainType:new{ ref_name = "plains", name = _("Plains"), color = 0x10db32 }
plains:register()

forest = TerrainType:new{ ref_name = "forest", name = _("Forest"), color = 0x3c5e35 }
forest:register()

jungle = TerrainType:new{ ref_name = "jungle", name = _("Jungle"), color = 0x00ff00 }
jungle:register()

desert = TerrainType:new{ ref_name = "desert", name = _("Desert"), color = 0xfcff0f }
desert:register()

savanna = TerrainType:new{ ref_name = "savanna", name = _("Savanna"), color = 0xff970f }
savanna:register()

taiga = TerrainType:new{ ref_name = "taiga", name = _("Taiga"), color = 0x47bf17 }
taiga:register()

tundra = TerrainType:new{ ref_name = "tundra", name = _("Tundra"), color = 0x00ffff }
tundra:register()

artic = TerrainType:new{ ref_name = "artic", name = _("Artic"), color = 0xffffff }
artic:register()

mountain = TerrainType:new{ ref_name = "mountain", name = _("Mountain"), color = 0xdbdbdb }
mountain:register()
