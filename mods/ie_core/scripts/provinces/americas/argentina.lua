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
--  	provinces/america/argentina.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Argentina + Paragonia

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Argentina + Paragonia

-- Argentina has these provinces "occupied"
local owned_provinces_list = {
    { ref_name = "buenos_aires", name = _("Buenos Aires"), color = rgb(255, 25, 0) },
    { ref_name = "misiones", name = _("Misiones"), color = rgb(255, 158, 84) },
    { ref_name = "entre_rios", name = _("Entre Ríos"), color = rgb(0, 169, 255) },
    { ref_name = "tandil", name = _("Tandil"), color = rgb(150, 112, 255) },
    { ref_name = "mendoza", name = _("Mendoza"), color = rgb(255, 240, 178) },
    { ref_name = "rosario", name = _("Rosario"), color = rgb(222, 183, 255) },
    { ref_name = "mar_chiquita", name = _("Mar Chiquita"), color = rgb(253, 224, 255) },
    { ref_name = "cordoba", name = _("Córdoba"), color = rgb(0, 255, 157) },
    { ref_name = "san_luis", name = _("San Luis"), color = rgb(34, 0, 255) },
    { ref_name = "san_juan", name = _("San Juan"), color = rgb(0, 255, 76) },
    { ref_name = "la_rioja", name = _("La Rioja"), color = rgb(255, 117, 119) },
    { ref_name = "santa_fe", name = _("Santa Fe"), color = rgb(255, 10, 242) },
    { ref_name = "catamarca", name = _("Catamarca"), color = rgb(61, 158, 255) },
    { ref_name = "tucuman", name = _("Tucumán"), color = rgb(255, 109, 170) },
    { ref_name = "southern_puna", name = _("Southern Puna"), color = rgb(255, 135, 217) },
    { ref_name = "central_puna", name = _("Central Puna"), color = rgb(225, 255, 0) },
    { ref_name = "rinconada", name = _("Rinconada"), color = rgb(205, 255, 186) },
    { ref_name = "jujuy", name = _("Jujuy"), color = rgb(150, 255, 195) },
    { ref_name = "santiago_del_estero", name = _("Santiago del Estero"), color = rgb(255, 106, 86) },
    { ref_name = "chaco", name = _("Chaco"), color = rgb(228, 168, 255) },
    { ref_name = "resistencia", name = _("Resistencia"), color = rgb(13, 27, 58) },
    { ref_name = "formosa", name = _("Formosa"), color = rgb(96, 255, 236) },
    { ref_name = "las_lomitas", name = _("Las Lomitas"), color = rgb(218, 170, 249) },
    { ref_name = "monte_quemado", name = _("Monte Quemado"), color = rgb(199, 255, 79) },
    { ref_name = "province_ff9ef200", name = _("Province_f29eff"), color = 0xf29eff },
    { ref_name = "province_29273800", name = _("Province_382729"), color = 0x382729 },
    { ref_name = "province_fcbb00", name = _("Province_bbfc00"), color = 0xbbfc00 },
}

for k, v in pairs(owned_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_pop(artisan, argentinian, catholic, 2000, 0.8)
    province:add_pop(farmer, argentinian, catholic, 8000, 0.5)
    province:add_pop(soldier, argentinian, catholic, 1000, 0.6)
    province:add_pop(craftsmen, argentinian, catholic, 2000, 0.4)
    province:add_pop(bureaucrat, argentinian, catholic, 500, 0.6)
    province:add_pop(burgeoise, argentinian, catholic, 100, 0.8)
    province:add_pop(clergymen, argentinian, catholic, 1000, 0.5)
    province:add_pop(laborer, argentinian, catholic, 1000, 0.5)
    province:add_pop(burgeoise, argentinian, catholic, 50, 0.9)
    province:add_nucleus(argentina)
    province:give_to(argentina)
    province:add_industry(sulfur_mine, argentina)
end

-- Uncolonized south of argentina (disputed by chile too because nobody owns them)
local mapuche_owned_provinces_list = {
    { ref_name = "corrientes", name = _("Corrientes"), color = rgb(241, 158, 254) },
    { ref_name = "la_pampa", name = _("La Pampa"), color = rgb(255, 165, 234) },
    { ref_name = "viedma", name = _("Viedma"), color = rgb(136, 0, 152) },
    { ref_name = "rio_negro", name = _("Río Negro"), color = rgb(255, 76, 76) },
    { ref_name = "tierra_del_fuego", name = _("Tierra del Fuego"), color = rgb(4, 13, 136) },
    { ref_name = "chubut", name = _("Chubut"), color = rgb(36, 31, 61) },
    { ref_name = "santa_cruz_andes", name = _("Santa Cruz"), color = rgb(61, 34, 42) },
    { ref_name = "neuquen", name = _("Neuquén"), color = rgb(226, 142, 255) },
    { ref_name = "laboulaye", name = _("Laboulaye"), color = rgb(255, 128, 96) },
    { ref_name = "dupuy", name = _("Dupuy"), color = rgb(255, 150, 109) },
    { ref_name = "llancanelo", name = _("Llancanelo"), color = rgb(175, 255, 178) }
}
for k, v in pairs(mapuche_owned_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_pop(artisan, Culture:get("mapuche"), Religion:get("mapuche"), 2000, 0.8)
    province:add_pop(farmer, Culture:get("mapuche"), Religion:get("mapuche"), 8000, 0.5)
    province:add_pop(soldier, Culture:get("mapuche"), Religion:get("mapuche"), 1000, 0.6)
    province:add_pop(craftsmen, Culture:get("mapuche"), Religion:get("mapuche"), 2000, 0.4)
    province:add_pop(bureaucrat, Culture:get("mapuche"), Religion:get("mapuche"), 500, 0.6)
    province:add_pop(burgeoise, Culture:get("mapuche"), Religion:get("mapuche"), 100, 0.8)
    province:add_pop(clergymen, Culture:get("mapuche"), Religion:get("mapuche"), 1000, 0.5)
    province:add_pop(laborer, Culture:get("mapuche"), Religion:get("mapuche"), 1000, 0.5)
    province:add_pop(burgeoise, Culture:get("mapuche"), Religion:get("mapuche"), 50, 0.9)
    
    province:add_nucleus(argentina)
    province:add_nucleus(chile)
    province:add_nucleus(Nation:get("mapuche"))
    province:give_to(Nation:get("mapuche"))
    province:add_industry(sulfur_mine, Nation:get("mapuche"))
    province:add_industry(copper_mine, Nation:get("mapuche"))
end

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: United Kingdom
province = Province:new{ ref_name = "falkland_islands", color = 0x078689 }
province.name = _("Falkland Islands")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, british, protestant, 300, 0.7)
province:add_pop(farmer, british, protestant, 2700, 0.6)
province:add_pop(soldier, british, protestant, 800, 0.8)
province:add_pop(craftsmen, british, protestant, 700, 0.5)
province:add_pop(bureaucrat, british, protestant, 200, 0.7)
province:add_pop(burgeoise, british, protestant, 200, 0.9)
province:add_pop(clergymen, british, protestant, 500, 0.9)
province:add_pop(laborer, british, protestant, 2700, 0.8)
province:add_pop(burgeoise, british, protestant, 100, 0.9)
province:add_nucleus(united_kingdom)
province:give_to(united_kingdom)
