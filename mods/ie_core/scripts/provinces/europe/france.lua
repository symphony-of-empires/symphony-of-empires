--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Middle Europe
-- Country: France

local france_provinces_list = {
    { ref_name = "finistere", name = _("Finistère"), color = rgb(151, 153, 57) },
    { ref_name = "cotes_d_armor", name = _("Côtes d' Armor"), color = rgb(207, 59, 224) },
    { ref_name = "morbihan", name = _("Morbihan"), color = rgb(89, 45, 192) },
    { ref_name = "ille_et_vilaine", name = _("Ille et Vilaine"), color = rgb(72, 127, 128) },
    { ref_name = "manche", name = _("Manche"), color = rgb(159, 29, 160) },
    { ref_name = "calvados", name = _("Calvados"), color = rgb(48, 30, 192) },
    { ref_name = "orne", name = _("Orne"), color = rgb(153, 75, 225) },
    { ref_name = "eure", name = _("Eure"), color = rgb(202, 25, 192) },
    { ref_name = "seine_maritime", name = _("Seine Maritime"), color = rgb(52, 155, 160) },
    { ref_name = "alpes_maritimes", name = _("Alpes Maritimes"), color = rgb(70, 147, 96) },
    { ref_name = "alpes_de_haute_provence", name = _("Alpes de Haute Provence"), color = rgb(163, 155, 0) },
    { ref_name = "hautes_alpes", name = _("Hautes Alpes"), color = rgb(35, 73, 160) },
    { ref_name = "var", name = _("Var"), color = rgb(168, 188, 192) },
    { ref_name = "cannes", name = _("Cannes"), color = rgb(55, 45, 0) },
    { ref_name = "bouches_du_rhone", name = _("Bouches du Rhône"), color = rgb(52, 147, 224) },
    { ref_name = "vaucluse", name = _("Vaucluse"), color = rgb(25, 5, 160) },
    
    { ref_name = "gard", name = _("Gard"), color = rgb(179, 0, 192) },
    { ref_name = "heraut", name = _("Hérault"), color = rgb(39, 34, 192) },
    { ref_name = "lozere", name = _("Lozère"), color = rgb(198, 228, 32) },
    { ref_name = "aveyron", name = _("Aveyron"), color = rgb(26, 97, 204) },
    { ref_name = "tarn", name = _("Tarn"), color = rgb(60, 79, 32) },
    { ref_name = "lot", name = _("Lot"), color = rgb(138, 149, 160) },
    { ref_name = "tarn_et_garonne", name = _("Tarn et Garonne"), color = rgb(32, 184, 128) },
    { ref_name = "gers", name = _("Gers"), color = rgb(181, 146, 0) },
    { ref_name = "hautes_pyrenees", name = _("Hautes Pyrénées"), color = rgb(220, 16, 160) },
    { ref_name = "haute_garonne", name = _("Haute_Garonne"), color = rgb(57, 26, 0) },
    { ref_name = "ariege", name = _("Ariège"), color = rgb(105, 221, 32) },
    { ref_name = "aude", name = _("Aude"), color = rgb(229, 11, 160) },
    { ref_name = "pyrenees_orientales", name = _("Pyrénées Orientales"), color = rgb(79, 142, 64) },
    { ref_name = "savoie", name = _("Savoie"), color = rgb(78, 69, 224) },
    { ref_name = "haute_savoie", name = _("Haute Savoie"), color = rgb(185, 107, 32) },
    { ref_name = "pyrenees_atlantiques", name = _("Pyrénées Atlantiques"), color = 0x402880 },
    
    -- { ref_name = "landes", name = _("Landes"), color = 0xc788a0 },
    -- { ref_name = "gironde", name = _("Gironde"), color = 0x417120 },
    -- { ref_name = "lotet_garonne", name = _("Lotet Garonne"), color = 0xcfdfc0 },
    -- { ref_name = "dordogne", name = _("Dordogne"), color = 0x7f81b9 },
    -- { ref_name = "correze", name = _("Corrèze"), color = 0xbb6dcd },
    -- { ref_name = "creuse", name = _("Creuse"), color = 0xb1b840 },
    -- { ref_name = "haute_vienne", name = _("Haute Vienne"), color = 0x45ef20 },
    -- { ref_name = "charente", name = _("Charente"), color = 0x5bbec0 },
    -- { ref_name = "charente_maritime", name = _("Charente Maritime"), color = 0xeeaba0 },
    -- { ref_name = "deux_sevres", name = _("Deux Sèvres"), color = 0xa1ae40 },
    -- { ref_name = "vienne", name = _("Vienne"), color = 0x3baa60 }

    -- Uncategorized
    { ref_name = "province_c0dfcf00", name = _("Unknown"), color = 0xcfdfc0 },
    { ref_name = "province_c023da00", name = _("Unknown"), color = 0xda23c0 },
    { ref_name = "province_40026600", name = _("Unknown"), color = 0x660240 },
    { ref_name = "province_40545500", name = _("Unknown"), color = 0x555440 },
    { ref_name = "province_50ef00", name = _("Unknown"), color = 0xef5000 },
    { ref_name = "province_a031bf00", name = _("Unknown"), color = 0xbf31a0 },
    { ref_name = "province_80e43500", name = _("Unknown"), color = 0x35e480 },
    { ref_name = "province_ae0b6200", name = _("Unknown"), color = 0x620bae },
    { ref_name = "province_bda800", name = _("Unknown"), color = 0xa8bd00 },
    { ref_name = "province_207f2600", name = _("Unknown"), color = 0x267f20 },
    { ref_name = "province_e0312e00", name = _("Unknown"), color = 0x2e31e0 },
    { ref_name = "province_4014b100", name = _("Unknown"), color = 0xb11440 },
    { ref_name = "province_a0000000", name = _("Unknown"), color = 0xa0 },
    { ref_name = "province_a657cd00", name = _("Unknown"), color = 0xcd57a6 },
    { ref_name = "province_20128f00", name = _("Unknown"), color = 0x8f1220 },
    { ref_name = "province_6027af00", name = _("Unknown"), color = 0xaf2760 },
    { ref_name = "province_8066c200", name = _("Unknown"), color = 0xc26680 },
    { ref_name = "province_a0c23000", name = _("Unknown"), color = 0x30c2a0 },
    { ref_name = "province_80aacc00", name = _("Unknown"), color = 0xccaa80 },
    { ref_name = "province_a0741600", name = _("Unknown"), color = 0x1674a0 },
    { ref_name = "province_a0fcff00", name = _("Unknown"), color = 0xfffca0 },
    { ref_name = "province_a0063b00", name = _("Unknown"), color = 0x3b06a0 },
    { ref_name = "province_ffe50000", name = _("Unknown"), color = 0xe5ff },
}
for k, v in pairs(france_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(wheat_farm, united_states, andes_transport)
    province:add_pop(artisan, french, catholic, 5000, 0.9)
    province:add_pop(farmer, french, catholic, 40000, 0.8)
    province:add_pop(soldier, french, catholic, 8000, 0.8)
    province:add_pop(craftsmen, french, catholic, 5000, 0.9)
    province:add_pop(bureaucrat, french, catholic, 5000, 0.9)
    province:add_pop(aristocrat, french, catholic, 5000, 0.9)
    province:add_pop(clergymen, french, catholic, 5000, 0.9)
    province:add_pop(laborer, french, catholic, 50000, 0.8)
    province:add_pop(entrepreneur, french, catholic, 800, 0.9)
    province:add_nucleus(france)
    province:give_to(france)
end

--Lille
province = Province:new{ ref_name = "lille", color = 0x842ba0 }
province.name = _("Lille")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Nord-Pas-De-Calais
province = Province:new{ ref_name = "nord_pas_de_calais", color = 0x368880 }
province.name = _("Nord-Pas-De-Calais")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Oise
province = Province:new{ ref_name = "oise", color = 0x7a05bc }
province.name = _("Oise")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Aisne
province = Province:new{ ref_name = "aisne", color = 0x99fbc0 }
province.name = _("Aisne")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Paris
province = Province:new{ ref_name = "paris", color = 0x4b1060 }
province.name = _("Paris")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
france:set_capital(province)
--Essonne
province = Province:new{ ref_name = "essonne", color = 0xc97580 }
province.name = _("Essonne")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Val-d'Oise
province = Province:new{ ref_name = "val_doise", color = 0xb63600 }
province.name = _("Val-d'Oise")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Yvelines
province = Province:new{ ref_name = "yvelines", color = 0x9f66cc }
province.name = _("Yvelines")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Ardennes
province = Province:new{ ref_name = "ardennes", color = 0xa77440 }
province.name = _("Ardennes")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Aube
province = Province:new{ ref_name = "aube", color = 0x335420 }
province.name = _("Aube")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Marne
province = Province:new{ ref_name = "marne", color = 0xc5b192 }
province.name = _("Marne")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Maurthe-et-Moselle
province = Province:new{ ref_name = "maurthe_et_moselle", color = 0xc64040 }
province.name = _("Maurthe-et-Moselle")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Meuse
province = Province:new{ ref_name = "meuse", color = 0x1a4e60 }
province.name = _("Meuse")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Moselle
province = Province:new{ ref_name = "moselle", color = 0x2f64a2 }
province.name = _("Moselle")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Vosges
province = Province:new{ ref_name = "vosges", color = 0x4bb440 }
province.name = _("Vosges")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Bas-Rhin
province = Province:new{ ref_name = "bas_rhin", color = 0x2afc40 }
province.name = _("Bas-Rhin")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Haut-Rhin
province = Province:new{ ref_name = "haut_rhin", color = 0xcf3ba0 }
province.name = _("Haut-Rhin")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Doubs
province = Province:new{ ref_name = "doubs", color = 0xdfe9c0 }
province.name = _("Doubs")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Haute-Saône
province = Province:new{ ref_name = "haute_saone", color = 0x592da0 }
province.name = _("Haute-Saône")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Côte-d'Or
province = Province:new{ ref_name = "cote_dor", color = 0x723440 }
province.name = _("Côte-d'Or")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Saône-et-Loire
province = Province:new{ ref_name = "saone_et_loire", color = 0xdb6c40 }
province.name = _("Saône-et-Loire")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Yonne
province = Province:new{ ref_name = "yonne", color = 0xe13280 }
province.name = _("Yonne")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Nièvre
province = Province:new{ ref_name = "nievre", color = 0x2e636b }
province.name = _("Nièvre")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Dordogne
province = Province:new{ ref_name = "dordogne", color = 0x7f81b9 }
province.name = _("Dordogne")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Gironde
province = Province:new{ ref_name = "gironde", color = 0x417120 }
province.name = _("Gironde")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Landes
province = Province:new{ ref_name = "landes", color = 0xc788a0 }
province.name = _("Landes")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Lot-et-Garonne
province = Province:new{ ref_name = "lot_et_garonne", color = 0xcfcfC0 }
province.name = _("Lot-et-Garonne")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Charente
province = Province:new{ ref_name = "charente", color = 0x5bbec0 }
province.name = _("Charente")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Charente-Maritime
province = Province:new{ ref_name = "charente_maritime", color = 0xeeaba0 }
province.name = _("Charente-Maritime")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Deux-Sèvres
province = Province:new{ ref_name = "deux_sevres", color = 0xa1ae40 }
province.name = _("Deux-Sèvres")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Vienne
province = Province:new{ ref_name = "vienne", color = 0x3baa60 }
province.name = _("Vienne")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Corrèze
province = Province:new{ ref_name = "correze", color = 0xc07a20 }
province.name = _("Corrèze")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Creuse
province = Province:new{ ref_name = "creuse", color = 0xb1b840 }
province.name = _("Creuse")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Haute-Vienne
province = Province:new{ ref_name = "haute_vienne", color = 0x45ef20 }
province.name = _("Haute-Vienne")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Allier
province = Province:new{ ref_name = "allier", color = 0x69dbe0 }
province.name = _("Allier")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Cantal
province = Province:new{ ref_name = "cantal", color = 0xbb6dcd }
province.name = _("Cantal")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Haute-Loire
province = Province:new{ ref_name = "haute_loire", color = 0x9fc1a0 }
province.name = _("Haute-Loire")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)
--Puy-de-Dôme
province = Province:new{ ref_name = "puy_de_dome", color = 0xdcb420 }
province.name = _("Puy-de-Dôme")
province:register()
province:add_pop(artisan, french, christian, 5000, 0.9)
province:add_pop(farmer, french, christian, 40000, 0.8)
province:add_pop(soldier, french, christian, 8000, 0.8)
province:add_pop(craftsmen, french, christian, 5000, 0.9)
province:add_pop(bureaucrat, french, christian, 5000, 0.9)
province:add_pop(aristocrat, french, christian, 5000, 0.9)
province:add_pop(clergymen, french, christian, 5000, 0.9)
province:add_pop(laborer, french, christian, 50000, 0.8)
province:add_pop(entrepreneur, french, christian, 800, 0.9)
province:add_nucleus(france)
province:give_to(france)