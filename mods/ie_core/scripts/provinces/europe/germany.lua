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
--  	provinces/europe/germany.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Middle Europe
-- Country: Germany (and all her minor states)

-- "So how many german minors are you gonna add?"
-- ALL OF THEM

local liechtenstein_provinces_list = {
    { ref_name = "vaduz", name = _("Vaduz"), color = 0x4c67ff },
}
for k, v in pairs(liechtenstein_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(copper_mine, Nation:get("liechtenstein"))
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(Nation:get("liechtenstein"))
    --province:add_nucleus(Nation:get("germany"))
    province:give_to(Nation:get("liechtenstein"))
    province:set_terrain(plains)
end
Nation:get("liechtenstein"):set_capital(Province:get("vaduz"))

local oldenburg_provinces_list = {
    { ref_name = "oldenburg", name = _("Oldenburg"), color = 0xffeebf },
    { ref_name = "birkenfeld", name = _("Birkenfeld"), color = 0xffa8c6 },
}
for k, v in pairs(oldenburg_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(copper_mine, oldenburg)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(oldenburg)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(oldenburg)
    province:set_terrain(plains)
end
oldenburg:set_capital(Province:get("oldenburg"))

-- Wolfenbuttel is actually owned by brunswick
local wolfenbuttel_provinces_list = {
    { ref_name = "wolfenbuttel", name = _("Wolfenbüttel"), color = 0x00ff21 },
    { ref_name = "province_7cffb300", name = _("Province_b3ff7c"), color = 0xb3ff7c },
}
for k, v in pairs(wolfenbuttel_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(copper_mine, brunswick)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(wolfenbuttel)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(Nation:get("brunswick"))
    province:set_terrain(plains)
end
wolfenbuttel:set_capital(Province:get("wolfenbuttel"))

local bremen_provinces_list = {
    { ref_name = "bremen", name = _("Bremen"), color = 0xff054b },
}
for k, v in pairs(bremen_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(copper_mine, bremen)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(bremen)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(bremen)
    province:set_terrain(plains)
end
bremen:set_capital(Province:get("bremen"))

local brunswick_provinces_list = {
    { ref_name = "brunswick", name = _("Brunswick"), color = 0xd8ffd6 },
}
for k, v in pairs(brunswick_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, brunswick)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(brunswick)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(brunswick)
    province:set_terrain(plains)
end
brunswick:set_capital(Province:get("brunswick"))

local waldeck_provinces_list = {
    { ref_name = "province_ff3f4600", name = _("Province_463fff"), color = 0x463fff },
    { ref_name = "province_ebffa500", name = _("Province_a5ffeb"), color = 0xa5ffeb },    
}
for k, v in pairs(waldeck_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, Nation:get("waldeck"))
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(Nation:get("waldeck"))
    province:add_nucleus(Nation:get("germany"))
    province:give_to(Nation:get("waldeck"))
    province:set_terrain(plains)
end

local hamburg_provinces_list = {
    { ref_name = "province_ff49d100", name = _("Province_d149ff"), color = 0xd149ff },
}
for k, v in pairs(hamburg_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, Nation:get("hamburg"))
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(Nation:get("hamburg"))
    province:add_nucleus(Nation:get("germany"))
    province:give_to(Nation:get("hamburg"))
    province:set_terrain(plains)
end

local frankfurt_provinces_list = {
    { ref_name = "province_ff305d00", name = _("Province_5d30ff"), color = 0x5d30ff },
}
for k, v in pairs(frankfurt_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, Nation:get("frankfurt"))
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(Nation:get("frankfurt"))
    province:add_nucleus(Nation:get("germany"))
    province:give_to(Nation:get("frankfurt"))
    province:set_terrain(plains)
end

local schaumburg_lippe_provinces_list = {
    { ref_name = "province_ffeace00", name = _("Province_ceeaff"), color = 0xceeaff },
}
for k, v in pairs(schaumburg_lippe_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, Nation:get("schaumburg_lippe"))
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(Nation:get("schaumburg_lippe"))
    province:add_nucleus(Nation:get("germany"))
    province:give_to(Nation:get("schaumburg_lippe"))
    province:set_terrain(plains)
end

-- Apparently it's part of hamburg
local ritzebuttel_provinces_list = {
    { ref_name = "ritzebuttel", name = _("Ritzebüttel"), color = 0x96eaff },
}
for k, v in pairs(ritzebuttel_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(arms_factory, ritzebuttel)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(ritzebuttel)
    province:add_nucleus(hamburg)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(hamburg)
    province:set_terrain(plains)
end
ritzebuttel:set_capital(Province:get("ritzebuttel"))

local grand_duchy_of_hesse_and_by_rhine_provinces_list = {
    { ref_name = "darnustadt", name = _("Darnustadt"), color = 0xffd3ec },
    { ref_name = "giessen", name = _("Giessen"), color = 0x84ff84 },
    { ref_name = "province_ffda8c00", name = _("Province_8cdaff"), color = 0x8cdaff },
}
for k, v in pairs(grand_duchy_of_hesse_and_by_rhine_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(coal_mine, grand_duchy_of_hesse_and_by_rhine)
    province:add_industry(ammunition_factory, grand_duchy_of_hesse_and_by_rhine)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(grand_duchy_of_hesse_and_by_rhine)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(grand_duchy_of_hesse_and_by_rhine)
    province:set_terrain(plains)
end

local holstein_provinces_list = {
    { ref_name = "province_a0300c00", name = _("Province_0c30a0"), color = 0x0c30a0 },
    { ref_name = "province_c08c9c00", name = _("Province_9c8cc0"), color = 0x9c8cc0 },
    { ref_name = "province_40eadf00", name = _("Province_dfea40"), color = 0xdfea40 },
    { ref_name = "province_ffd6f700", name = _("Province_f7d6ff"), color = 0xf7d6ff },
    { ref_name = "province_409ba300", name = _("Province_a39b40"), color = 0xa39b40 },
    { ref_name = "province_a08d0b00", name = _("Province_0b8da0"), color = 0x0b8da0 },
    { ref_name = "province_40531100", name = _("Province_115340"), color = 0x115340 },
    { ref_name = "province_ff195600", name = _("Province_5619ff"), color = 0x5619ff },
}
for k, v in pairs(holstein_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(coal_mine, holstein)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(holstein)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(holstein)
    province:set_terrain(plains)
end

local anhalt_provinces_list = {
    { ref_name = "province_c1ff7000", name = _("Province_70ffc1"), color = 0x70ffc1 },
}
for k, v in pairs(anhalt_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(coal_mine, anhalt)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(anhalt)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(anhalt)
    province:set_terrain(plains)
end

local lippe_provinces_list = {
    { ref_name = "province_ffb1a500", name = _("Province_a5b1ff"), color = 0xa5b1ff },
}
for k, v in pairs(lippe_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(coal_mine, lippe)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(lippe)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(lippe)
    province:set_terrain(plains)
end

local nassau_provinces_list = {
    { ref_name = "province_c1ff9900", name = _("Province_99ffc1"), color = 0x99ffc1 },
}
for k, v in pairs(nassau_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(coal_mine, nassau)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(nassau)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(nassau)
    province:set_terrain(plains)
end

local saxe_weimar_provinces_list = {
    { ref_name = "province_da56ff00", name = _("Province_ff56da"), color = 0xff56da },
    { ref_name = "province_ffb7d100", name = _("Province_d1b7ff"), color = 0xd1b7ff },
    { ref_name = "province_ff99ed00", name = _("Province_ed99ff"), color = 0xed99ff },
}
for k, v in pairs(saxe_weimar_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(copper_mine, saxe_weimar)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(saxe_weimar)
    province:add_nucleus(Nation:get("thuringia"))
    province:add_nucleus(Nation:get("germany"))
    province:give_to(saxe_weimar)
    province:set_terrain(plains)
end

local saxe_gotha_provinces_list = {
    { ref_name = "province_5b8fff00", name = _("Province_ff8f5b"), color = 0xff8f5b },
    { ref_name = "province_ffbf5900", name = _("Province_59bfff"), color = 0x59bfff },
    { ref_name = "province_4f60ff00", name = _("Province_ff604f"), color = 0xff604f },
    { ref_name = "province_e8cb00", name = _("Province_cbe800"), color = 0xcbe800 },
}
for k, v in pairs(saxe_gotha_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, saxe_gotha)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(saxe_gotha)
    province:add_nucleus(Nation:get("thuringia"))
    province:add_nucleus(Nation:get("germany"))
    province:give_to(saxe_gotha)
    province:set_terrain(plains)
end

local saxe_coburg_provinces_list = {
    { ref_name = "province_ffd47000", name = _("Province_70d4ff"), color = 0x70d4ff },
    { ref_name = "province_ff0a4f00", name = _("Province_4f0aff"), color = 0x4f0aff },
}
for k, v in pairs(saxe_coburg_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, saxe_coburg)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(saxe_coburg)
    province:add_nucleus(Nation:get("thuringia"))
    province:add_nucleus(Nation:get("germany"))
    province:give_to(saxe_coburg)
    province:set_terrain(plains)
end

local saxe_hildburghausen_provinces_list = {
    { ref_name = "province_addaff00", name = _("Province_ffdaad"), color = 0xffdaad },
    { ref_name = "province_a4ff9e00", name = _("Province_9effa4"), color = 0x9effa4 },
}
for k, v in pairs(saxe_hildburghausen_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, saxe_hildburghausen)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(saxe_hildburghausen)
    province:add_nucleus(Nation:get("thuringia"))
    province:add_nucleus(Nation:get("germany"))
    province:give_to(saxe_hildburghausen)
    province:set_terrain(plains)
end

local saxe_meiningen_provinces_list = {
    { ref_name = "province_ff057100", name = _("Province_7105ff"), color = 0x7105ff },
}
for k, v in pairs(saxe_meiningen_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, saxe_meiningen)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(saxe_meiningen)
    province:add_nucleus(Nation:get("thuringia"))
    province:add_nucleus(Nation:get("germany"))
    province:give_to(saxe_meiningen)
    province:set_terrain(plains)
end

local reuss_gera_provinces_list = {
    { ref_name = "province_ff940000", name = _("Province_0094ff"), color = 0x0094ff },
    { ref_name = "province_f760ff00", name = _("Province_ff60f7"), color = 0xff60f7 },
    { ref_name = "province_c1ffc400", name = _("Province_c4ffc1"), color = 0xc4ffc1 },
    { ref_name = "province_5b8cff00", name = _("Province_ff8c5b"), color = 0xff8c5b },
    { ref_name = "province_ffd20a00", name = _("Province_0ad2ff"), color = 0x0ad2ff },
}
for k, v in pairs(reuss_gera_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(coal_mine, reuss_gera)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(reuss_gera)
    province:add_nucleus(Nation:get("thuringia"))
    province:add_nucleus(Nation:get("germany"))
    province:give_to(reuss_gera)
    province:set_terrain(plains)
end

local hesse_kassel_provinces_list = {
    { ref_name = "province_bbff3500", name = _("Province_35ffbb"), color = 0x35ffbb },
    { ref_name = "province_ff6db800", name = _("Province_b86dff"), color = 0xb86dff },
    { ref_name = "province_d2ff9b00", name = _("Province_9bffd2"), color = 0x9bffd2 },
    { ref_name = "province_82ff8800", name = _("Province_88ff82"), color = 0x88ff82 },
    { ref_name = "province_2c19ff00", name = _("Province_ff192c"), color = 0xff192c },
}
for k, v in pairs(hesse_kassel_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(copper_mine, hesse_kassel)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(hesse_kassel)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(hesse_kassel)
    province:set_terrain(plains)
end

local luxembourg_provinces_list = {
    { ref_name = "luxembourg", name = _("Luxembourg"), color = 0xb056ff },
}
for k, v in pairs(luxembourg_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(sulfur_mine, luxembourg)
    province:add_industry(coal_mine, luxembourg)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(luxembourg)
    province:give_to(luxembourg)
    province:set_terrain(plains)
end
luxembourg:set_capital(Province:get("luxembourg"))

local prussia_provinces_list = {
    { ref_name = "province_b770ff00", name = _("Unknown"), color = 0xb770fe },
    { ref_name = "province_8aff5900", name = _("Unknown"), color = 0x59ff8a },
    { ref_name = "province_ff000000", name = _("Unknown"), color = 0xff },
    { ref_name = "province_ffd1a000", name = _("Unknown"), color = 0xa0d1ff },
    { ref_name = "province_9dff7f00", name = _("Unknown"), color = 0x7fff9d },
    { ref_name = "province_d3afff00", name = _("Unknown"), color = 0xffafd3 },
    { ref_name = "province_ff6b8d00", name = _("Unknown"), color = 0x8d6bff },
    { ref_name = "province_206cd500", name = _("Unknown"), color = 0xd56c20 },
    { ref_name = "province_e0a91900", name = _("Unknown"), color = 0x19a9e0 },
    { ref_name = "province_6032e100", name = _("Unknown"), color = 0xe13260 },
    { ref_name = "province_c089da00", name = _("Unknown"), color = 0xda89c0 },
    { ref_name = "province_9f23ff00", name = _("Unknown"), color = 0xff239f },
    { ref_name = "province_e2ffc400", name = _("Unknown"), color = 0xc4ffe2 },
    { ref_name = "province_803b5c00", name = _("Unknown"), color = 0x5c3b80 },
    { ref_name = "province_20452c00", name = _("Unknown"), color = 0x2c4520 },
    { ref_name = "province_2216ff00", name = _("Unknown"), color = 0xff1622 },
    { ref_name = "province_a0c07d00", name = _("Unknown"), color = 0x7dc0a0 },
    { ref_name = "province_a049b400", name = _("Unknown"), color = 0xb449a0 },
    { ref_name = "province_ff60e700", name = _("Unknown"), color = 0xe760ff },
    { ref_name = "province_a064af00", name = _("Unknown"), color = 0xaf64a0 },
    { ref_name = "province_c0cc9d00", name = _("Unknown"), color = 0x9dccc0 },
    { ref_name = "province_c6ff1c00", name = _("Unknown"), color = 0x1cffc6 },
    { ref_name = "province_ff70b700", name = _("Unknown"), color = 0xb770ff },
    { ref_name = "province_f6ff7a00", name = _("Unknown"), color = 0x7afff6 },
    { ref_name = "province_87a1ff00", name = _("Unknown"), color = 0xffa187 },
    { ref_name = "province_b5fff600", name = _("Unknown"), color = 0xf6ffb5 },
    { ref_name = "province_ff857200", name = _("Unknown"), color = 0x7285ff },
    { ref_name = "province_ffdebf00", name = _("Unknown"), color = 0xbfdeff },
    { ref_name = "province_60646e00", name = _("Unknown"), color = 0x6e6460 },
    { ref_name = "province_a2ff4c00", name = _("Unknown"), color = 0x4cffa2 },
    { ref_name = "province_8057bb00", name = _("Province_bb5780"), color = 0xbb5780 },
    { ref_name = "province_40b17600", name = _("Province_76b140"), color = 0x76b140 },
    { ref_name = "province_fe288900", name = _("Province_8928fe"), color = 0x8928fe },
}
for k, v in pairs(prussia_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, prussia)
    province:add_industry(wheat_farm, prussia)
    province:add_industry(bakery, prussia)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(prussia)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(prussia)
    province:set_terrain(plains)
end

local bavaria_provinces_list = {
    { ref_name = "province_9fff9900", name = _("Unknown"), color = 0x99ff9f },
    { ref_name = "province_e856ff00", name = _("Unknown"), color = 0xff56e8 },
    { ref_name = "province_d2ff8e00", name = _("Unknown"), color = 0x8effd2 },
    { ref_name = "province_ffc38400", name = _("Unknown"), color = 0x84c3ff },
    { ref_name = "weimar", name = _("Weimar"), color = 0xff4f98 },
    { ref_name = "rhineland_palatinate", name = _("Rhineland-Palatinate"), color = 0xffe242 },
    { ref_name = "province_19aaff00", name = _("Province_ffaa19"), color = 0xffaa19 },
}
for k, v in pairs(bavaria_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, bavaria)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(bavaria)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(bavaria)
    province:set_terrain(plains)
end

local belgium_provinces_list = {
    { ref_name = "province_beff7500", name = _("Unknown"), color = 0x75ffbe },
    { ref_name = "province_f4ffa500", name = _("Unknown"), color = 0xa5fff4 },
    { ref_name = "province_ffd6fc00", name = _("Unknown"), color = 0xfcd6ff },
    { ref_name = "province_8d6dff00", name = _("Unknown"), color = 0xff6d8d },
    { ref_name = "province_ff597d00", name = _("Unknown"), color = 0x7d59ff },
    { ref_name = "province_00ff00", name = _("Unknown"), color = 0xff0000 },
    { ref_name = "province_e7ff7f00", name = _("Unknown"), color = 0x7fffe7 },
    { ref_name = "province_ffc26d00", name = _("Unknown"), color = 0x6dc2ff },
    { ref_name = "province_feafc500", name = _("Unknown"), color = 0xc5affe },
}
for k, v in pairs(belgium_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, belgium)
    province:add_pop(artisan, belgian, protestant, 5000, 0.9)
    province:add_pop(farmer, belgian, protestant, 40000, 0.8)
    province:add_pop(soldier, belgian, protestant, 8000, 0.8)
    province:add_pop(craftsmen, belgian, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, belgian, protestant, 5000, 0.9)
    province:add_pop(burgeoise, belgian, protestant, 5000, 0.9)
    province:add_pop(clergymen, belgian, protestant, 5000, 0.9)
    province:add_pop(laborer, belgian, protestant, 50000, 0.8)
    province:add_pop(burgeoise, belgian, protestant, 800, 0.9)
    province:add_nucleus(belgium)
    province:give_to(belgium)
    province:set_terrain(plains)
end

local hannover_provinces_list = {
    { ref_name = "province_c832ff00", name = _("Unknown"), color = 0xff32c8 },
    { ref_name = "province_20dfad00", name = _("Unknown"), color = 0xaddf20 },
}
for k, v in pairs(hannover_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, hannover)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(hannover)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(hannover)
    province:set_terrain(plains)
end

local lubeck_provinces_list = {
    { ref_name = "province_82c2ff00", name = _("Province_ffc282"), color = 0xffc282 },
}
for k, v in pairs(lubeck_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, Nation:get("lubeck"))
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(Nation:get("lubeck"))
    province:add_nucleus(Nation:get("germany"))
    province:give_to(Nation:get("lubeck"))
    province:set_terrain(plains)
end

local switzerland_provinces_list = {
    { ref_name = "province_e09f9a00", name = _("Unknown"), color = 0x9a9fe0 },
    { ref_name = "province_60e43500", name = _("Unknown"), color = 0x35e460 },
    { ref_name = "province_605aff00", name = _("Unknown"), color = 0xff5a60 },
    { ref_name = "province_40d99400", name = _("Unknown"), color = 0x94d940 },
    { ref_name = "province_e0bf7d00", name = _("Unknown"), color = 0x7dbfe0 },
    { ref_name = "province_4001bf00", name = _("Unknown"), color = 0xbf0140 },
    { ref_name = "province_a0dead00", name = _("Unknown"), color = 0xaddea0 },
    { ref_name = "province_e0d52e00", name = _("Unknown"), color = 0x2ed5e0 },
    { ref_name = "province_203b1c00", name = _("Unknown"), color = 0x1c3b20 },
}
for k, v in pairs(switzerland_provinces_list) do
    province = Province:new(v)
    province:register()
    province:add_industry(iron_mine, switzerland)
    province:add_industry(copper_mine, switzerland)
    province:add_industry(coal_mine, switzerland)
    province:add_industry(sulfur_mine, switzerland)
    province:add_pop(artisan, german, catholic, 5000, 0.9)
    province:add_pop(farmer, german, catholic, 40000, 0.8)
    province:add_pop(soldier, german, catholic, 8000, 0.8)
    province:add_pop(craftsmen, german, catholic, 5000, 0.9)
    province:add_pop(bureaucrat, german, catholic, 5000, 0.9)
    province:add_pop(burgeoise, german, catholic, 5000, 0.9)
    province:add_pop(clergymen, german, catholic, 5000, 0.9)
    province:add_pop(laborer, german, catholic, 50000, 0.8)
    province:add_pop(burgeoise, german, catholic, 800, 0.9)
    province:add_nucleus(switzerland)
    province:give_to(switzerland)
    province:set_terrain(plains)
end

-- Radical catholics on switzerland
local prov_list = Nation:get("switzerland"):get_owned_provinces()
for k, p in pairs(prov_list) do
    local pops = p:get_pops()
    for k, v in pairs(pops) do
        if v.religion.ref_name == "catholic" then
            -- Give them +50 militancy
            v.militancy = v.militancy + 50.0
            -- Synchronize changes with the world
            p:update_pop(v)
        end
    end
    p:update_pops()
end

local baden_provinces_list = {
    { ref_name = "baden", name = _("Baden"), color = 0xff703d },
}
for k, v in pairs(baden_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, baden)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(baden)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(baden)
    province:set_terrain(plains)
end
baden:set_capital(Province:get("baden"))

local wurttemberg_provinces_list = {
    { ref_name = "wurttemberg", name = _("Württemberg"), color = 0x9d7ffe },
}
for k, v in pairs(wurttemberg_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, wurttemberg)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(wurttemberg)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(wurttemberg)
    province:set_terrain(plains)
end
wurttemberg:set_capital(Province:get("wurttemberg"))

local mecklenburg_provinces_list = {
    -- Schwerin
    { ref_name = "mecklenburg", name = _("Mecklenburg"), color = 0xff7a59 },
    { ref_name = "province_ff66f700", name = _("Province_f766ff"), color = 0xf766ff },
    { ref_name = "province_ff8ea300", name = _("Province_a38eff"), color = 0xa38eff },

    -- Sterlitz
    { ref_name = "province_a0ffd100", name = _("Province_d1ffa0"), color = 0xd1ffa0 },
    { ref_name = "province_5000ff00", name = _("Province_ff0050"), color = 0xff0050 },
}
for k, v in pairs(mecklenburg_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, mecklenburg)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(mecklenburg)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(mecklenburg)
    province:set_terrain(plains)
end
mecklenburg:set_capital(Province:get("mecklenburg"))

local saxonyg_provinces_list = {
    { ref_name = "dresden", name = _("Dresden"), color = 0x8cb2ff },
}
for k, v in pairs(saxonyg_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, saxony)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(burgeoise, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(burgeoise, german, protestant, 800, 0.9)
    province:add_nucleus(saxony)
    province:add_nucleus(Nation:get("germany"))
    province:give_to(saxony)
    province:set_terrain(plains)
end
saxony:set_capital(Province:get("dresden"))
