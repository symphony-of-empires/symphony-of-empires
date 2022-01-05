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
--  	provinces/europe/denmark.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Middle Europe
-- Country: Denmark

local denmark_provinces_list = {
    { ref_name = "syddanmark", name = _("Syddanmark"), color = 0xa3ddff, density = 1.2 },
    { ref_name = "hedensted", name = _("Hedensted"), color = 0xdcb2ff, density = 9.4 },
    { ref_name = "frederiksborg", name = _("Frederiksborg"), color = 0x00d0ff, density = 5.2 },
    { ref_name = "midtjylland", name = _("Midtjylland"), color = 0xc972ff, density = 3.2 },
    { ref_name = "nordjylland", name = _("Nordjylland"), color = 0x7cb7ff, density = 2.0 },
    { ref_name = "odense", name = _("Odense"), color = 0x84eeff, density = 10.0 },

    -- German part
    { ref_name = "province_20154200", name = _("Province_421520"), color = 0x421520, density = 1.0 },
    { ref_name = "province_809fbc00", name = _("Province_bc9f80"), color = 0xbc9f80, density = 1.0 },
    { ref_name = "province_a00fba00", name = _("Province_ba0fa0"), color = 0xba0fa0, density = 1.0 },
    { ref_name = "province_c84900", name = _("Province_49c800"), color = 0x49c800, density = 1.0 },
    { ref_name = "province_c0ddad00", name = _("Province_adddc0"), color = 0xadddc0, density = 1.0 },
    { ref_name = "province_40790d00", name = _("Province_0d7940"), color = 0x0d7940, density = 1.0 },
    { ref_name = "province_60531100", name = _("Province_115360"), color = 0x115360, density = 1.0 },
    { ref_name = "province_807fd900", name = _("Province_d97f80"), color = 0xd97f80, density = 1.0 },
    { ref_name = "province_e022da00", name = _("Province_da22e0"), color = 0xda22e0, density = 1.0 },
    { ref_name = "province_4058bb00", name = _("Province_bb5840"), color = 0xbb5840, density = 1.0 },
    { ref_name = "province_ffc9a300", name = _("Province_a3c9ff"), color = 0xa3c9ff, density = 1.0 },
}
for k, v in pairs(denmark_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(wheat_farm, denmark)
    province:add_pop(artisan, danish, protestant, 5000 * v.density, 0.9)
    province:add_pop(farmer, danish, protestant, 40000 * v.density, 0.8)
    province:add_pop(soldier, danish, protestant, 8000 * v.density, 0.8)
    province:add_pop(craftsmen, danish, protestant, 5000 * v.density, 0.9)
    province:add_pop(bureaucrat, danish, protestant, 5000 * v.density, 0.9)
    province:add_pop(burgeoise, danish, protestant, 800 * v.density, 0.9)
    province:add_pop(clergymen, danish, protestant, 5000 * v.density, 0.9)
    province:add_pop(laborer, danish, protestant, 50000 * v.density, 0.8)
    province:add_nucleus(denmark)
    province:add_nucleus(scandinavia)
    province:give_to(denmark)
end
