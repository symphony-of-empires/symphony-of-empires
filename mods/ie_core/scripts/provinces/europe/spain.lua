--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Iberia
-- Country: Spain
local spain_provinces_list = {
    { ref_name = "a_coruna", name = _("A Coruña"), color = rgb(155, 157, 203) },
    { ref_name = "lugo", name = _("Lugo"), color = rgb(190, 141, 96) },
    { ref_name = "pontevedra", name = _("Pontevedra"), color = rgb(55, 209, 96) },
    { ref_name = "ourense", name = _("Ourense"), color = rgb(106, 128, 128) },
    { ref_name = "asturias", name = _("Asturias"), color = rgb(187, 225, 96) },
    { ref_name = "avila", name = _("Ávila"), color = rgb(86, 157, 160) },
    { ref_name = "burgos", name = _("Burgos"), color = rgb(189, 235, 64) },
    { ref_name = "leon", name = _("León"), color = rgb(70, 147, 64) },
    { ref_name = "palencia", name = _("Palencia"), color = rgb(201, 157, 157) },
    { ref_name = "salamanca", name = _("Salamanca"), color = rgb(225, 50, 32) },
    { ref_name = "soria", name = _("Soria"), color = rgb(247, 167, 32) },
    { ref_name = "valladolid", name = _("Valladolid"), color = rgb(91, 25, 96) },
    { ref_name = "zamora", name = _("Zamora"), color = rgb(68, 166, 32) },
    { ref_name = "cantabria", name = _("Cantabria"), color = rgb(201, 203, 157) },
    { ref_name = "badajoz", name = _("Badajoz"), color = rgb(247, 167, 0) },
    { ref_name = "caceres", name = _("Cáceres"), color = rgb(155, 157, 149) },
    { ref_name = "almeria", name = _("Almería"), color = rgb(76, 253, 192) },
    { ref_name = "cadiz", name = _("Cádiz"), color = rgb(114, 51, 64) },
    { ref_name = "cordoba_spain", name = _("Córdoba"), color = rgb(195, 11, 0) },
    { ref_name = "granada", name = _("Granada"), color = rgb(193, 29, 224) },
    { ref_name = "huelva", name = _("Huelva"), color = rgb(75, 180, 224) },
    { ref_name = "jaen", name = _("Jaén"), color = rgb(85, 85, 0) },
    { ref_name = "malaga", name = _("Málaga"), color = rgb(206, 151, 96) },
    { ref_name = "seville", name = _("Seville"), color = rgb(92, 157, 145) },
    { ref_name = "madrid", name = _("Madrid"), color = rgb(110, 114, 209) },
    { ref_name = "albacete", name = _("Albacete"), color = rgb(103, 75, 0) },
    { ref_name = "ciudad_real", name = _("Ciudad Real"), color = rgb(155, 203, 210) },
    { ref_name = "cuenca", name = _("Cuenca"), color = rgb(209, 204, 128) },
    { ref_name = "guadalajara", name = _("Guadalajara"), color = rgb(224, 142, 64) },
    { ref_name = "toledo", name = _("Toledo"), color = rgb(71, 54, 224) },
    { ref_name = "murcia", name = _("Murcia"), color = rgb(255, 238, 157) },
    { ref_name = "mallorca", name = _("Mallorca"), color = rgb(134, 85, 127) },
    { ref_name = "menorca", name = _("Menorca"), color = rgb(134, 85, 150) },
    { ref_name = "ibiza", name = _("Ibiza"), color = rgb(157, 85, 127) },
    { ref_name = "barcelona", name = _("Barcelona"), color = rgb(235, 227, 108) },
    { ref_name = "girona", name = _("Girona"), color = rgb(34, 1, 128) },
    { ref_name = "lleida", name = _("Lleida"), color = rgb(105, 220, 0) },
    { ref_name = "tarragona", name = _("Tarragona"), color = rgb(205, 78, 160) },
    { ref_name = "alicante", name = _("Alicante"), color = rgb(210, 112, 160) },
    { ref_name = "castellon", name = _("Castellón"), color = rgb(255, 227, 108) },
    { ref_name = "valencia", name = _("Valencia"), color = rgb(213, 1, 192) },
    { ref_name = "segovia", name = _("Segovia"), color = rgb(193, 30, 32) },
    { ref_name = "pequenotereul", name = _("Pequeñotereul"), color = rgb(206, 151, 64) },
    { ref_name = "huesca", name = _("Huesca"), color = rgb(216, 199, 118) },
    { ref_name = "zaragoza", name = _("Zaragoza"), color = rgb(42, 253, 64) },
    { ref_name = "teruel", name = _("Teruel"), color = rgb(89, 210, 32) },
    { ref_name = "alava", name = _("Álava"), color = rgb(125, 187, 187) },
    { ref_name = "biscay", name = _("Biscay"), color = rgb(182, 222, 160) },
    { ref_name = "gipuzkoa", name = _("Gipuzkoa"), color = rgb(87, 229, 160) },
    { ref_name = "navarre", name = _("Navarre"), color = rgb(151, 100, 120) }
}

for k, v in pairs(spain_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_pop(artisan, spanish, catholic, 5000, 0.9)
    province:add_pop(farmer, spanish, catholic, 40000, 0.8)
    province:add_pop(soldier, spanish, catholic, 8000, 0.8)
    province:add_pop(craftsmen, spanish, catholic, 5000, 0.9)
    province:add_pop(bureaucrat, spanish, catholic, 5000, 0.9)
    province:add_pop(aristocrat, spanish, catholic, 5000, 0.9)
    province:add_pop(clergymen, spanish, catholic, 5000, 0.9)
    province:add_pop(laborer, spanish, catholic, 50000, 0.8)
    province:add_pop(entrepreneur, spanish, catholic, 800, 0.9)
    province:add_nucleus(spain)
    province:add_nucleus(iberia)
    province:give_to(spain)
    province:set_terrain(plains)
    province:add_industry(wheat_farm, spain, andes_transport)
end