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
--  	provinces/europe/scandinavia.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: North America
-- Country: Denmark (Greenland)
province = Province:new{ ref_name = "nuuk", color = 0xff8efd }
province.name = _("Nuuk")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(bureaucrat, danish, protestant, 2, 0.8)
province:add_pop(clergymen, danish, protestant, 5, 0.8)
province:add_pop(laborer, danish, protestant, 50, 0.5)
province:add_nucleus(denmark)
province:give_to(denmark)

province = Province:new{ ref_name = "qanaaq", color = 0x6bffb0 }
province.name = _("Qanaaq")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(bureaucrat, danish, protestant, 2, 0.8)
province:add_pop(clergymen, danish, protestant, 5, 0.8)
province:add_pop(laborer, danish, protestant, 50, 0.5)
province:add_nucleus(denmark)
province:give_to(denmark)

province = Province:new{ ref_name = "tasiilaq", color = 0xffbcbd }
province.name = _("Tasiilaq")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(bureaucrat, danish, protestant, 2, 0.8)
province:add_pop(clergymen, danish, protestant, 5, 0.8)
province:add_pop(laborer, danish, protestant, 50, 0.5)
province:add_nucleus(denmark)
province:give_to(denmark)

province = Province:new{ ref_name = "artic_greenland", color = 0x568eff }
province.name = _("Artic Greenland")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(bureaucrat, danish, protestant, 2, 0.8)
province:add_pop(clergymen, danish, protestant, 5, 0.8)
province:add_pop(laborer, danish, protestant, 50, 0.5)
province:add_nucleus(denmark)
province:give_to(denmark)

--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Scandinavia
-- Country: Iceland
province = Province:new{ ref_name = "austurland", color = 0x9effae }
province.name = _("Austurland")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(bureaucrat, icelandic, protestant, 2, 0.8)
province:add_pop(clergymen, icelandic, protestant, 5, 0.8)
province:add_pop(laborer, icelandic, protestant, 50, 0.5)
province:add_nucleus(iceland)
province:add_nucleus(denmark)
province:give_to(denmark)
province = Province:new{ ref_name = "norourland_eystra", color = 0x2723ff }
province.name = _("Norðurland eystra")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(bureaucrat, icelandic, protestant, 2, 0.8)
province:add_pop(clergymen, icelandic, protestant, 5, 0.8)
province:add_pop(laborer, icelandic, protestant, 50, 0.5)
province:add_nucleus(iceland)
province:add_nucleus(denmark)
province:give_to(denmark)
province = Province:new{ ref_name = "hofouborgarsvaeoio", color = 0xff0720 }
province.name = _("Höfuðborgarsvæðið")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(bureaucrat, icelandic, protestant, 2, 0.8)
province:add_pop(clergymen, icelandic, protestant, 5, 0.8)
province:add_pop(laborer, icelandic, protestant, 50, 0.5)
province:add_nucleus(iceland)
province:add_nucleus(denmark)
province:give_to(denmark)
province = Province:new{ ref_name = "suorland", color = 0xffb2d6 }
province.name = _("Suðurland")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(bureaucrat, icelandic, protestant, 2, 0.8)
province:add_pop(clergymen, icelandic, protestant, 5, 0.8)
province:add_pop(laborer, icelandic, protestant, 50, 0.5)
province:add_nucleus(iceland)
province:add_nucleus(denmark)
province:give_to(denmark)
province = Province:new{ ref_name = "vestfiroir", color = 0x87ffda }
province.name = _("Vestfirðir")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(bureaucrat, icelandic, protestant, 2, 0.8)
province:add_pop(clergymen, icelandic, protestant, 5, 0.8)
province:add_pop(laborer, icelandic, protestant, 50, 0.5)
province:add_nucleus(iceland)
province:add_nucleus(denmark)
province:give_to(denmark)
province = Province:new{ ref_name = "vesturland", color = 0xba75ff }
province.name = _("Vesturland")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(bureaucrat, icelandic, protestant, 2, 0.8)
province:add_pop(clergymen, icelandic, protestant, 5, 0.8)
province:add_pop(laborer, icelandic, protestant, 50, 0.5)
province:add_nucleus(iceland)
province:add_nucleus(denmark)
province:give_to(denmark)

--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Scandinavia
-- Country: Sweden

local sweden_provinces_list = {
    { ref_name = "skane", name = _("Skåne"), color = 0xff5b84 },
    { ref_name = "blekinge", name = _("Blekinge"), color = 0x84ff92 },
    { ref_name = "smaland", name = _("Småland"), color = 0xa3fff4 },
    { ref_name = "oland", name = _("Öland"), color = 0x622dff },
    { ref_name = "vastersmaland", name = _("Västersmåland"), color = 0xd400ff },
    { ref_name = "gotland", name = _("Gotland"), color = 0x00ff26 },
    { ref_name = "halsingland", name = _("Hälsingland"), color = 0xecffaf },
    { ref_name = "vastergotland", name = _("Västergötland"), color = 0x1000ff },
    { ref_name = "uppland", name = _("Uppland"), color = 0xea82ff },
    { ref_name = "dalarna", name = _("Dalarna"), color = 0x7ae6ff },
    { ref_name = "varmland", name = _("Värmland"), color = 0xff7fdd },
    { ref_name = "jamtland", name = _("Jämtland"), color = 0x82abff },
    { ref_name = "vasterbotten", name = _("Västerbotten"), color = 0xff6bc3 },
    { ref_name = "lappland", name = _("Lappland"), color = 0x0cdaff },
    { ref_name = "province_ff70c800", name = _("Province_c870ff"), color = 0xc870ff },
}
for k, v in pairs(sweden_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(wheat_farm, sweden)
    province:add_pop(artisan, swedish, protestant, 5000, 0.9)
    province:add_pop(farmer, swedish, protestant, 40000, 0.8)
    province:add_pop(soldier, swedish, protestant, 8000, 0.8)
    province:add_pop(craftsmen, swedish, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, swedish, protestant, 5000, 0.9)
    province:add_pop(burgeoise, swedish, protestant, 5000, 0.9)
    province:add_pop(clergymen, swedish, protestant, 5000, 0.9)
    province:add_pop(laborer, swedish, protestant, 50000, 0.8)
    province:add_pop(burgeoise, swedish, protestant, 800, 0.9)
    province:add_nucleus(denmark)
    province:add_nucleus(sweden)
    province:add_nucleus(scandinavia)
    province:give_to(sweden)
    province:set_terrain(plains)
end

--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Scandinavia
-- Country: Finland (Russia)

local finland_provinces_list = {
    { ref_name = "turku", name = _("Turku"), color = 0x3aff5e },
    { ref_name = "helsinki", name = _("Helsinki"), color = 0xffc942 },
    { ref_name = "vaasa", name = _("Vaasa"), color = 0x1cff91 },
    { ref_name = "hame", name = _("Häme"), color = 0xa3fff1 },
    { ref_name = "uusimaa", name = _("Uusimaa"), color = 0x70d1ff },
    { ref_name = "kymi", name = _("Kymi"), color = 0x00ffb2 },
    { ref_name = "north_karelia", name = _("North Karelia"), color = 0xe5ff75 },
    { ref_name = "mikkeli", name = _("Mikkeli"), color = 0xf95bff },
    { ref_name = "kuopio", name = _("Kuopio"), color = 0x91ffc6 },
    { ref_name = "central finland", name = _("Central Finland"), color = 0x8c49ff },
    { ref_name = "oulu", name = _("Oulu"), color = 0xff42e8 },
    { ref_name = "east_oulu", name = _("East Oulu"), color = 0xe6d8ff },
    { ref_name = "lapland", name = _("Lapland"), color = 0xcd91ff },
}
for k, v in pairs(finland_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(wheat_farm, russia)
    province:add_pop(artisan, russian, orthodox, 5000, 0.9)
    province:add_pop(farmer, russian, orthodox, 40000, 0.8)
    province:add_pop(soldier, russian, orthodox, 8000, 0.8)
    province:add_pop(craftsmen, russian, orthodox, 5000, 0.9)
    province:add_pop(bureaucrat, russian, orthodox, 5000, 0.9)
    province:add_pop(burgeoise, russian, orthodox, 5000, 0.9)
    province:add_pop(clergymen, russian, orthodox, 5000, 0.9)
    province:add_pop(laborer, russian, orthodox, 50000, 0.8)
    province:add_pop(burgeoise, russian, orthodox, 800, 0.9)
    province:add_pop(artisan, finnish, orthodox, 5000, 0.9)
    province:add_pop(farmer, finnish, orthodox, 40000, 0.8)
    province:add_pop(soldier, finnish, orthodox, 8000, 0.8)
    province:add_pop(craftsmen, finnish, orthodox, 5000, 0.9)
    province:add_pop(bureaucrat, finnish, orthodox, 5000, 0.9)
    province:add_pop(burgeoise, finnish, orthodox, 5000, 0.9)
    province:add_pop(clergymen, finnish, orthodox, 5000, 0.9)
    province:add_pop(laborer, finnish, orthodox, 50000, 0.8)
    province:add_pop(burgeoise, finnish, orthodox, 800, 0.9)
    province:add_nucleus(finland)
    province:add_nucleus(russia)
    province:add_nucleus(scandinavia)
    province:give_to(russia)
    province:set_terrain(plains)
end

province = Province:new{ ref_name = "aland", color = 0x2dffd1 }
province.name = _("Åland")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_pop(artisan, russian, orthodox, 5000, 0.9)
province:add_pop(farmer, russian, orthodox, 40000, 0.8)
province:add_pop(soldier, russian, orthodox, 8000, 0.8)
province:add_pop(craftsmen, russian, orthodox, 5000, 0.9)
province:add_pop(bureaucrat, russian, orthodox, 5000, 0.9)
province:add_pop(burgeoise, russian, orthodox, 5000, 0.9)
province:add_pop(clergymen, russian, orthodox, 5000, 0.9)
province:add_pop(laborer, russian, orthodox, 50000, 0.8)
province:add_pop(burgeoise, russian, orthodox, 800, 0.9)
province:add_pop(artisan, finnish, orthodox, 5000, 0.9)
province:add_pop(farmer, finnish, orthodox, 40000, 0.8)
province:add_pop(soldier, finnish, orthodox, 8000, 0.8)
province:add_pop(craftsmen, finnish, orthodox, 5000, 0.9)
province:add_pop(bureaucrat, finnish, orthodox, 5000, 0.9)
province:add_pop(burgeoise, finnish, orthodox, 5000, 0.9)
province:add_pop(clergymen, finnish, orthodox, 5000, 0.9)
province:add_pop(laborer, finnish, orthodox, 50000, 0.8)
province:add_pop(burgeoise, finnish, orthodox, 800, 0.9)
province:add_nucleus(aland)
province:add_nucleus(sweden)
province:add_nucleus(finland)
province:add_nucleus(russia)
province:add_nucleus(scandinavia)
province:give_to(russia)

--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Scandinavia
-- Country: Norway
local norway_provinces_list = {
    { ref_name = "bohusland", name = _("Bohuslän"), color = 0xceb2ff },
    { ref_name = "hedmark", name = _("Hedmark"), color = 0xff001d },
    { ref_name = "agder", name = _("Agder"), color = 0xff5bfc },
    { ref_name = "rogaland", name = _("Rogaland"), color = 0xd154ff },
    { ref_name = "vestland", name = _("Vestland"), color = 0x9000ff },
    { ref_name = "vestfold_og_telemark", name = _("Vestfold og telemark"), color = 0x729fff },
    { ref_name = "oppland", name = _("Oppland"), color = 0xb5ffc6 },
    { ref_name = "oslo", name = _("Oslo"), color = 0x2bffb4 },
    { ref_name = "trondelag", name = _("Trøndelag"), color = 0x947fff },
    { ref_name = "nordland", name = _("Nordland"), color = 0x5005ff },
    { ref_name = "troms", name = _("Troms"), color = 0x00ffbf },
    { ref_name = "ostfold", name = _("Østfold"), color = 0x8cfbff },
}
for k, v in pairs(norway_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(wheat_farm, sweden)
    province:add_pop(artisan, norwegian, protestant, 5000, 0.9)
    province:add_pop(farmer, norwegian, protestant, 40000, 0.8)
    province:add_pop(soldier, norwegian, protestant, 8000, 0.8)
    province:add_pop(craftsmen, norwegian, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, norwegian, protestant, 5000, 0.9)
    province:add_pop(burgeoise, norwegian, protestant, 5000, 0.9)
    province:add_pop(clergymen, norwegian, protestant, 5000, 0.9)
    province:add_pop(laborer, norwegian, protestant, 50000, 0.8)
    province:add_pop(burgeoise, norwegian, protestant, 800, 0.9)
    province:add_nucleus(norway)
    province:add_nucleus(scandinavia)
    province:give_to(sweden)
    province:set_terrain(plains)
end