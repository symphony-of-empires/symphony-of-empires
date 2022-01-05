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
--  	provinces/america/mexico.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--------------------------------------------------------------
-- Continent: America
-- Subcontinent: North America
-- Country: Mexico

local mexico_provinces_list = {
    { ref_name = "yucatan", name = _("Yucatán"), color = rgb(86, 123, 255) },
    { ref_name = "veracruz", name = _("Veracruz"), color = rgb(255, 117, 142) },
    { ref_name = "campico", name = _("Campico"), color = rgb(96, 81, 255) },
    { ref_name = "coahuila_y_texas", name = _("Coahuila"), color = rgb(255, 114, 121) },
    { ref_name = "chihuahua", name = _("Chihuahua"), color = rgb(56, 218, 255) },
    { ref_name = "alta_california", name = _("Alta California"), color = rgb(198, 203, 255) },
    { ref_name = "baja_california", name = _("Baja California"), color = rgb(255, 56, 231) },
    { ref_name = "tamaulipas", name = _("Tamaulipas"), color = rgb(255, 135, 189) },
    { ref_name = "nuevo_leon", name = _("Nuevo León"), color = rgb(191, 255, 206) },
    { ref_name = "sonora", name = _("Sonora"), color = rgb(155, 255, 233) },
    { ref_name = "sinaloa", name = _("Sinaloa"), color = rgb(255, 173, 135) },
    { ref_name = "jalisco", name = _("Jalisco"), color = rgb(54, 28, 255) },
    { ref_name = "durango", name = _("Durango"), color = rgb(146, 255, 96) },
    { ref_name = "zacatecas", name = _("Zacatecas"), color = rgb(255, 147, 160) },
    { ref_name = "san_luis_potosi", name = _("San Luis Potosi"), color = rgb(63, 255, 232) },
    { ref_name = "guanajuato", name = _("Guanajuato"), color = rgb(247, 15, 255) },
    { ref_name = "colima", name = _("Colima"), color = rgb(255, 126, 94) },
    { ref_name = "michoacan", name = _("Michoacán"), color = rgb(196, 255, 196) },
    { ref_name = "queretaro", name = _("Querétaro"), color = rgb(187, 178, 255) },
    { ref_name = "mexico", name = _("México"), color = rgb(255, 50, 227) },
    { ref_name = "puebla", name = _("Puebla"), color = rgb(146, 122, 255) },
    { ref_name = "tlaxcala", name = _("Tlaxcala"), color = rgb(137, 255, 225) },
    { ref_name = "oaxaca", name = _("Oaxaca"), color = rgb(142, 255, 168) },
    { ref_name = "chiapas", name = _("Chiapas"), color = rgb(191, 114, 255) },
    { ref_name = "nuevo_mexico", name = _("Nuevo México"), color = rgb(86, 255, 184) },
    { ref_name = "south_new_mexico", name = _("South New Mexico"), color = rgb(145, 161, 255) },

    { ref_name = "province_e04e3c00", name = _("Province_3c4ee0"), color = 0x3c4ee0 },
    { ref_name = "province_60ee2300", name = _("Province_23ee60"), color = 0x23ee60 },
    { ref_name = "province_80916800", name = _("Province_689180"), color = 0x689180 },
    { ref_name = "province_01bc00", name = _("Province_bc0100"), color = 0xbc0100 },
    { ref_name = "province_e052ab00", name = _("Province_ab52e0"), color = 0xab52e0 },
}

for k, v in pairs(mexico_provinces_list) do
    province = Province:new(v)
    province:register()
    province:add_industry(sulfur_mine, mexico)
    province:add_pop(artisan, mexican, catholic, 2000, 0.8)
    province:add_pop(farmer, mexican, catholic, 8000, 0.5)
    province:add_pop(soldier, mexican, catholic, 1000, 0.6)
    province:add_pop(craftsmen, mexican, catholic, 2000, 0.4)
    province:add_pop(bureaucrat, mexican, catholic, 500, 0.6)
    province:add_pop(burgeoise, mexican, catholic, 100, 0.8)
    province:add_pop(clergymen, mexican, catholic, 1000, 0.5)
    province:add_pop(laborer, mexican, catholic, 1000, 0.5)
    province:add_nucleus(mexico)
    province:give_to(mexico)
    province:set_terrain(plains)
end