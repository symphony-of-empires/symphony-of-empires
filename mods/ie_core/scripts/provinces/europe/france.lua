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
    
    { ref_name = "landes", name = _("Landes"), color = 0xc788a0 },
    { ref_name = "gironde", name = _("Gironde"), color = 0x417120 },
    { ref_name = "lotet_garonne", name = _("Lotet Garonne"), color = 0xcfdfc0 },
    { ref_name = "dordogne", name = _("Dordogne"), color = 0x7f81b9 },
    { ref_name = "correze", name = _("Corrèze"), color = 0xbb6dcd },
    { ref_name = "creuse", name = _("Creuse"), color = 0xb1b840 },
    { ref_name = "haute_vienne", name = _("Haute Vienne"), color = 0x45ef20 },
    { ref_name = "charente", name = _("Charente"), color = 0x5bbec0 },
    { ref_name = "charente_maritime", name = _("Charente Maritime"), color = 0xeeaba0 },
    { ref_name = "deux_sevres", name = _("Deux Sèvres"), color = 0xa1ae40 },
    { ref_name = "vienne", name = _("Vienne"), color = 0x3baa60 }
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