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
--  	provinces/america/central_america.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: Central America
-- Country: USCA

local upca_provinces_list = {
    { ref_name = "province_ff5e8900", name = _("Province_895eff"), color = 0x895eff },
    { ref_name = "province_ce75ff00", name = _("Province_ff75ce"), color = 0xff75ce },
    { ref_name = "province_40338d00", name = _("Province_8d3340"), color = 0x8d3340 },
    { ref_name = "province_20268d00", name = _("Province_8d2620"), color = 0x8d2620 },
    { ref_name = "province_838300", name = _("Province_838300"), color = 0x838300 },
    { ref_name = "province_c0c84900", name = _("Province_49c8c0"), color = 0x49c8c0 },
    { ref_name = "province_80b8dc00", name = _("Province_dcb880"), color = 0xdcb880 },
    { ref_name = "province_ff00ff00", name = _("Province_ff00ff"), color = 0xff00ff },
    { ref_name = "province_20e41c00", name = _("Province_1ce420"), color = 0x1ce420 },
    { ref_name = "province_000000", name = _("Province_000000"), color = 0x000000 },
    { ref_name = "province_8020ec00", name = _("Province_ec2080"), color = 0xec2080 },
    { ref_name = "province_b7ff5400", name = _("Province_54ffb7"), color = 0x54ffb7 },
    { ref_name = "province_6061c600", name = _("Province_c66160"), color = 0xc66160 },
}
for k, v in pairs(upca_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(gold_mine, upca)
    province:add_industry(wheat_farm, upca)
    province:add_pop(artisan, german, protestant, 5000, 0.7)
    province:add_pop(farmer, german, protestant, 5000, 0.1)
    province:add_pop(soldier, german, protestant, 5000, 0.2)
    province:add_pop(craftsmen, german, protestant, 5000, 0.2)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
    province:add_pop(burgeoise, german, protestant, 5000, 0.2)
    province:add_pop(clergymen, german, protestant, 5000, 0.3)
    province:add_pop(laborer, german, protestant, 5000, 0.2)
    province:add_pop(burgeoise, german, protestant, 500, 0.9)
    province:add_pop(slave, german, protestant, 500, 0.5)
    province:add_nucleus(upca)
    province:give_to(upca)
end

province = Province:new{ ref_name = "bocas_del_toro", color = 0xB7CAFF }
province.name = _("Bocas del Toro")
province:register()
province:add_industry(gold_mine, russia)
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "cabagra", color = 0xFF87D5 }
province.name = _("Cabagra")
province:register()
province:add_industry(gold_mine, russia)
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "san_jose", color = 0x9EFFFF }
province.name = _("San José")
province:register()
province:add_industry(sulfur_mine, russia)
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "liberia", color = 0x7C7AFF }
province.name = _("Libéria")
province:register()
province:add_industry(sulfur_mine, russia)
province:add_industry(sulfur_mine, russia)
province:add_industry(sulfur_mine, russia)
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "nicoya", color = 0xFF7CC0 }
province.name = _("Nicoya")
province:register()
province:add_industry(sulfur_mine, russia)
province:add_industry(sulfur_mine, russia)
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "mosquito_coast", color = 0x5199FF }
province.name = _("Mosquito Coast")
province:register()
province:add_industry(sulfur_mine, russia)
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "nicaragua", color = 0xFFCEAD }
province.name = _("Nicarágua")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "puerto_lempira", color = 0xCD9BFF }
province.name = _("Puerto Lempira")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "honduras", color = 0xFF89B8 }
province.name = _("Honduras")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "el_salvador", color = 0xB599FF }
province.name = _("El Salvador")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)
upca:set_capital(province)

province = Province:new{ ref_name = "ahuachapan", color = 0xF572FF }
province.name = _("Ahuachapán")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "sonsonate", color = 0xA47AFF }
province.name = _("Sonsonate")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "guatemala", color = 0xFFDD82 }
province.name = _("Guatemala")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "alacranes", color = 0xFF0019 }
province.name = _("Alacranes")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "calakmul", color = 0xFF66FF }
province.name = _("Calakmul")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "el_ceibo", color = 0x54FF79 }
province.name = _("El Ceibo")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "selva_lancadona", color = 0xFF8A6D }
province.name = _("Selva Lancadona")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "motozintla", color = 0x77B9FF }
province.name = _("Motozintla")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "tapachula", color = 0x7C5BFF }
province.name = _("Tapachula")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)

province = Province:new{ ref_name = "islas_de_la_bahia", color = 0xFF11A8 }
province.name = _("Islas de La Bahía")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(upca)
province:give_to(upca)
