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
--  	provinces/europe/balkans.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Montenegro
local montenegro_provinces = {
    { ref_name = "province_ffc3bc00", name = _("Province_bcc3ff"), color = 0xbcc3ff },
    { ref_name = "province_ffebb500", name = _("Province_b5ebff"), color = 0xb5ebff },
    { ref_name = "province_fff87a00", name = _("Province_7af8ff"), color = 0x7af8ff },
    { ref_name = "province_ff21f700", name = _("Province_f721ff"), color = 0xf721ff },
}
for k, v in pairs(montenegro_provinces) do
    province = Province:new(v)
    province:register()

    --province:add_industry(wheat_farm, turkey)
    province:add_pop(artisan, serbian, orthodox, 5000, 0.9)
    province:add_pop(farmer, serbian, orthodox, 40000, 0.8)
    province:add_pop(soldier, serbian, orthodox, 8000, 0.8)
    province:add_pop(craftsmen, serbian, orthodox, 5000, 0.9)
    province:add_pop(bureaucrat, serbian, orthodox, 5000, 0.9)
    province:add_pop(burgeoise, serbian, orthodox, 5000, 0.9)
    province:add_pop(clergymen, serbian, orthodox, 5000, 0.9)
    province:add_pop(laborer, serbian, orthodox, 50000, 0.8)
    province:add_pop(burgeoise, serbian, orthodox, 800, 0.9)
    province:add_nucleus(montenegro)
    province:give_to(turkey)
    province:set_terrain(plains)
end

-- Serbia
local serbia_provinces = {
    { ref_name = "province_89f3ff00", name = _("Province_fff389"), color = 0xfff389 },
    { ref_name = "province_ff72ec00", name = _("Province_ec72ff"), color = 0xec72ff },
    { ref_name = "province_ff708500", name = _("Province_8570ff"), color = 0x8570ff },
}
for k, v in pairs(serbia_provinces) do
    province = Province:new(v)
    province:register()

    --province:add_industry(wheat_farm, turkey)
    province:add_pop(artisan, serbian, orthodox, 5000, 0.9)
    province:add_pop(farmer, serbian, orthodox, 40000, 0.8)
    province:add_pop(soldier, serbian, orthodox, 8000, 0.8)
    province:add_pop(craftsmen, serbian, orthodox, 5000, 0.9)
    province:add_pop(bureaucrat, serbian, orthodox, 5000, 0.9)
    province:add_pop(burgeoise, serbian, orthodox, 5000, 0.9)
    province:add_pop(clergymen, serbian, orthodox, 5000, 0.9)
    province:add_pop(laborer, serbian, orthodox, 50000, 0.8)
    province:add_pop(burgeoise, serbian, orthodox, 800, 0.9)
    province:add_nucleus(serbia)
    province:give_to(turkey)
    province:set_terrain(plains)
end

-- Wallachia
local wallachia_provinces = {
    { ref_name = "province_7597ff00", name = _("Province_ff9775"), color = 0xff9775 },
    { ref_name = "province_2bffca00", name = _("Province_caff2b"), color = 0xcaff2b },
    { ref_name = "province_a5ffaa00", name = _("Province_aaffa5"), color = 0xaaffa5 },
    { ref_name = "province_ff7a8e00", name = _("Province_8e7aff"), color = 0x8e7aff },
    { ref_name = "province_fffbaf00", name = _("Province_affbff"), color = 0xaffbff },
    { ref_name = "province_873dff00", name = _("Province_ff3d87"), color = 0xff3d87 },
    { ref_name = "province_a5ff5b00", name = _("Province_5bffa5"), color = 0x5bffa5 },
    { ref_name = "province_5bd8ff00", name = _("Province_ffd85b"), color = 0xffd85b },
    { ref_name = "province_ff7d6300", name = _("Province_637dff"), color = 0x637dff },
    { ref_name = "province_f763ff00", name = _("Province_ff63f7"), color = 0xff63f7 },
}
for k, v in pairs(wallachia_provinces) do
    province = Province:new(v)
    province:register()

    --province:add_industry(wheat_farm, turkey)
    province:add_pop(artisan, romanian, orthodox, 5000, 0.9)
    province:add_pop(farmer, romanian, orthodox, 40000, 0.8)
    province:add_pop(soldier, romanian, orthodox, 8000, 0.8)
    province:add_pop(craftsmen, romanian, orthodox, 5000, 0.9)
    province:add_pop(bureaucrat, romanian, orthodox, 5000, 0.9)
    province:add_pop(burgeoise, romanian, orthodox, 5000, 0.9)
    province:add_pop(clergymen, romanian, orthodox, 5000, 0.9)
    province:add_pop(laborer, romanian, orthodox, 50000, 0.8)
    province:add_pop(burgeoise, romanian, orthodox, 800, 0.9)
    province:add_nucleus(romania)
    province:add_nucleus(wallachia)
    province:give_to(wallachia)
    province:set_terrain(plains)
end

-- Moldavia
local moldavia_provinces = {
    { ref_name = "province_ff0a6300", name = _("Province_630aff"), color = 0x630aff },
    { ref_name = "province_15ff00", name = _("Province_ff1500"), color = 0xff1500 },
    { ref_name = "province_f1ffdb00", name = _("Province_dbfff1"), color = 0xdbfff1 },
    { ref_name = "province_87ffb900", name = _("Province_b9ff87"), color = 0xb9ff87 },
    { ref_name = "province_23302300", name = _("Province_233023"), color = 0x233023 },
    { ref_name = "province_a0a78800", name = _("Province_88a7a0"), color = 0x88a7a0 },
    { ref_name = "province_728aff00", name = _("Province_ff8a72"), color = 0xff8a72 },
}
for k, v in pairs(moldavia_provinces) do
    province = Province:new(v)
    province:register()

    --province:add_industry(wheat_farm, turkey)
    province:add_pop(artisan, romanian, orthodox, 5000, 0.9)
    province:add_pop(farmer, romanian, orthodox, 40000, 0.8)
    province:add_pop(soldier, romanian, orthodox, 8000, 0.8)
    province:add_pop(craftsmen, romanian, orthodox, 5000, 0.9)
    province:add_pop(bureaucrat, romanian, orthodox, 5000, 0.9)
    province:add_pop(burgeoise, romanian, orthodox, 5000, 0.9)
    province:add_pop(clergymen, romanian, orthodox, 5000, 0.9)
    province:add_pop(laborer, romanian, orthodox, 50000, 0.8)
    province:add_pop(burgeoise, romanian, orthodox, 800, 0.9)
    province:add_nucleus(romania)
    province:add_nucleus(moldavia)
    province:give_to(moldavia)
    province:set_terrain(plains)
end

-- Bulgaria
local bulgaria_provinces = {
    { ref_name = "blagoevgrad", name = _("Blagoevgrad"), color = 0xff666b },
    { ref_name = "lovetch", name = _("Lovetch"), color = 0xbcffdd },
    { ref_name = "vidin", name = _("Vidin"), color = 0xff2119 },
    { ref_name = "bourgas", name = _("Bourgas"), color = 0xff7c89 },
    { ref_name = "varna", name = _("Varna"), color = 0x9844ff },
}
for k, v in pairs(bulgaria_provinces) do
    province = Province:new(v)
    province:register()

    province:add_industry(wheat_farm, turkey)
    province:add_pop(artisan, bulgarian, orthodox, 5000, 0.9)
    province:add_pop(farmer, bulgarian, orthodox, 40000, 0.8)
    province:add_pop(soldier, bulgarian, orthodox, 8000, 0.8)
    province:add_pop(craftsmen, bulgarian, orthodox, 5000, 0.9)
    province:add_pop(bureaucrat, bulgarian, orthodox, 5000, 0.9)
    province:add_pop(burgeoise, bulgarian, orthodox, 5000, 0.9)
    province:add_pop(clergymen, bulgarian, orthodox, 5000, 0.9)
    province:add_pop(laborer, bulgarian, orthodox, 50000, 0.8)
    province:add_pop(burgeoise, bulgarian, orthodox, 800, 0.9)
    province:add_nucleus(bulgaria)
    province:give_to(turkey)
    province:set_terrain(plains)
end