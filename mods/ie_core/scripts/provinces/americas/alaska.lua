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
--  	provinces/america/alaska.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: North America
-- Country: Russian Empire

provinces_list = {
    -- Change to Western Alaska if taken by the US or UK
    { ref_name = "zapadnaya_alyaska", name = _("Zapadnaya Alyaska"), color = rgb(214, 255, 215) },
    -- Change to Inner Alaska if taken by US or UK
    { ref_name = "vnutrennyaya_alyaska", name = _("Vnutrennyaya Alyaska"), color = rgb(212, 198, 255) },
    -- Change to Sitka if taken by US or UK
    { ref_name = "novoarkhangelsk", name = _("Novoarkhangelsk"), color = rgb(181, 232, 255) },
    -- Change to Kodiak if taken by US or UK
    { ref_name = "kad_yak", name = _("Kad'yak"), color = rgb(255, 173, 176) },
    -- Change to Big Diomede when take by the US or UK
    { ref_name = "bolshoy_diomida", name = _("Bolshoy Diomida"), color = rgb(255, 69, 66) },
    { ref_name = "unalaska", name = _("Unalaska"), color = rgb(211, 255, 213) },
    -- Change to Little Diomede
    { ref_name = "malenkiy_diomid", name = _("Malenkiy Diomid"), color = rgb(137, 122, 255) },
    -- Change to Nunivak Island when US or UK gets it
    { ref_name = "nunivok", name = _("Nunivok"), color = rgb(172, 155, 255) },
    -- Change to Norton Sound when US or UK gets it
    { ref_name = "zaliv_norton", name = _("Zaliv Norton"), color = rgb(96, 255, 104) },
    { ref_name = "alexandrovski", name = _("Alexandrovski"), color = rgb(155, 211, 255) },
    { ref_name = "kenai", name = _("Kenai"), color = rgb(119, 248, 255) },
    -- Change to North Chugach when taken by US or UK
    { ref_name = "severnyy_chugach", name = _("Severnyy Chugach"), color = rgb(255, 99, 174) },
    -- Change to Hichinbrook Island when taken by US or UK
    { ref_name = "ostrov_nuchek", name = _("Ostrov Nuchek"), color = rgb(218, 145, 255) },
    -- Change to Alaska Peninsula when taken by US or UK
    { ref_name = "poluostrov_alyaska", name = _("Poluostrov Alyaska"), color = rgb(255, 204, 243) },
    -- Change to Fort Ross when taken by US in california!
    { ref_name = "krepost_ross", name = _("Krepostʹ Ross"), color = rgb(119, 255, 135) },
    { ref_name = "yakutat", name = _("Yakutat"), color = rgb(187, 168, 255) }
}

for k, v in pairs(provinces_list) do
    province = Province:new(v)
    province:register()
    
    province:add_industry(wheat_farm, russia)
    province:add_pop(artisan, russian, orthodox, 300, 0.7)
    province:add_pop(farmer, russian, orthodox, 2700, 0.6)
    province:add_pop(soldier, russian, orthodox, 800, 0.8)
    province:add_pop(craftsmen, russian, orthodox, 700, 0.5)
    province:add_pop(bureaucrat, russian, orthodox, 200, 0.7)
    province:add_pop(burgeoise, russian, orthodox, 200, 0.9)
    province:add_pop(clergymen, russian, orthodox, 500, 0.9)
    province:add_pop(laborer, russian, orthodox, 2700, 0.8)
    province:add_pop(burgeoise, russian, orthodox, 100, 0.9)
    province:add_nucleus(russia)
    province:give_to(russia)
end