--------------------------------------------------------------
-- Continent: America
-- Subcontinent: South America
-- Country: Brazil
local brazil_provinces_list = {
    { ref_name = "ceara", name = _("Ceará"), color = rgb(0, 255, 255) },
    { ref_name = "rio_grande_do_norte", name = _("Rio Grande do Norte"), color = rgb(255, 0, 110) },
    { ref_name = "paraiba", name = _("Paraíba"), color = rgb(76, 255, 0) },
    { ref_name = "pernambuco", name = _("Pernambuco"), color = rgb(0, 127, 127) },
    { ref_name = "alagoas", name = _("Alagoas"), color = rgb(182, 255, 0) },
    { ref_name = "sergipe", name = _("Sergipe"), color = rgb(255, 106, 0) },
    { ref_name = "bahia", name = _("Bahia"), color = rgb(160, 160, 160) },
    { ref_name = "sao_francisco", name = _("São Francisco"), color = rgb(178, 0, 255) },
    { ref_name = "piaui", name = _("Piauí"), color = rgb(73, 176, 255) },
    { ref_name = "maranhao", name = _("Maranhão"), color = rgb(194, 137, 255) },
    { ref_name = "grao_para", name = _("Grão-Pará"), color = rgb(255, 109, 221) },
    { ref_name = "oiapoque", name = _("Oiapoque"), color = rgb(46, 213, 0) },
    { ref_name = "maraca", name = _("Maracá"), color = rgb(147, 145, 32) },
    { ref_name = "roraima", name = _("Roraima"), color = rgb(255, 253, 142) },
    { ref_name = "mato_grosso", name = _("Mato Grosso"), color = rgb(157, 255, 53) },
    { ref_name = "goias", name = _("Goiás"), color = rgb(73, 255, 98) },
    { ref_name = "minas_gerais", name = _("Minas Gerais"), color = rgb(158, 0, 73) },
    { ref_name = "espirito_santo", name = _("Espirito Santo"), color = rgb(103, 99, 158) },
    { ref_name = "sao_paulo", name = _("São Paulo"), color = rgb(127, 255, 142) },
    { ref_name = "rio_de_janeiro", name = _("Rio de Janeiro"), color = rgb(135, 99, 158) },
    { ref_name = "santa_catarina", name = _("Santa Catarina"), color = rgb(236, 24, 129) },
    { ref_name = "parana", name = _("Paraná"), color = rgb(209, 204, 64) },
    { ref_name = "oeste_catarinense", name = _("Oeste Catarinense"), color = rgb(255, 38, 164) },
    { ref_name = "porto_alegre", name = _("Porto Alegre"), color = rgb(150, 255, 248) },
    { ref_name = "bage", name = _("Bagé"), color = rgb(208, 137, 255) },
    { ref_name = "sao_borja", name = _("São Borja"), color = rgb(111, 162, 128) },
    { ref_name = "cisplatina", name = _("Cisplatina"), color = rgb(255, 234, 132) }
}

for k, v in pairs(brazil_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_pop(artisan, brazilian, catholic, 5000, 0.7)
    province:add_pop(farmer, brazilian, catholic, 5000, 0.1)
    province:add_pop(soldier, brazilian, catholic, 5000, 0.2)
    province:add_pop(craftsmen, brazilian, catholic, 5000, 0.2)
    province:add_pop(bureaucrat, brazilian, catholic, 5000, 0.7)
    province:add_pop(aristocrat, brazilian, catholic, 5000, 0.2)
    province:add_pop(clergymen, brazilian, catholic, 5000, 0.3)
    province:add_pop(laborer, brazilian, catholic, 5000, 0.2)
    province:add_pop(entrepreneur, brazilian, catholic, 500, 0.9)
    province:add_pop(slave, brazilian, catholic, 500, 0.5)
    province:add_nucleus(brazil)
    province:give_to(brazil)
    province:add_industry(wheat_farm, brazil, andes_transport)
end

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Paraguay
local paraguay_provinces_list = {
    { ref_name = "asuncion", name = _("Asunción"), color = rgb(0, 199, 255) },
    { ref_name = "vila_sacaron", name = _("Vila Sacarón"), color = rgb(183, 28, 255) },
    { ref_name = "nioaque", name = _("Nioaque"), color = rgb(81, 116, 255) }
}

for k, v in pairs(paraguay_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_pop(artisan, paraguayan, catholic, 5000, 0.7)
    province:add_pop(farmer, paraguayan, catholic, 5000, 0.1)
    province:add_pop(soldier, paraguayan, catholic, 5000, 0.2)
    province:add_pop(craftsmen, paraguayan, catholic, 5000, 0.2)
    province:add_pop(bureaucrat, paraguayan, catholic, 5000, 0.7)
    province:add_pop(aristocrat, paraguayan, catholic, 5000, 0.2)
    province:add_pop(clergymen, paraguayan, catholic, 5000, 0.3)
    province:add_pop(laborer, paraguayan, catholic, 5000, 0.2)
    province:add_pop(entrepreneur, paraguayan, catholic, 500, 0.9)
    province:add_pop(slave, paraguayan, catholic, 500, 0.5)
    province:add_nucleus(paraguay)
    province:give_to(paraguay)
    province:add_industry(wheat_farm, paraguay, andes_transport)
end


--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Bolivia + Chaco
local bolivia_provinces_list = {
    { ref_name = "pando", name = _("Pando"), color = rgb(71, 251, 255) },
    { ref_name = "santa_cruz", name = _("Santa Cruz"), color = rgb(255, 196, 238) },
    { ref_name = "carvoal", name = _("Carvoal"), color = rgb(255, 129, 104) },
--    { ref_name = "corumba", name = _("Corumbá"), color = rgb(71, 251, 255) },
    { ref_name = "santissima_trinidade", name = _("Santíssima Trinidade"), color = rgb(94, 30, 255) },
    { ref_name = "fuerte_olimpo", name = _("Fuerte Olimpo"), color = rgb(111, 96, 113) },
    { ref_name = "nueva_burdeos", name = _("Nueva Burdeos"), color = rgb(255, 209, 73) },
    { ref_name = "la_paz", name = _("La Paz"), color = rgb(255, 33, 88) },
    { ref_name = "cochabamba", name = _("Cochabamba"), color = rgb(203, 63, 255) },
    { ref_name = "oruro", name = _("Oruro"), color = rgb(117, 255, 204) },
    { ref_name = "potosi", name = _("Potosí"), color = rgb(252, 99, 255) },
    { ref_name = "chuquisaca", name = _("Chuquisaca"), color = rgb(79, 167, 255) },
    { ref_name = "tarija", name = _("Tarija"), color = rgb(144, 124, 255) }
}

for k, v in pairs(bolivia_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_pop(artisan, bolivian, catholic, 5000, 0.7)
    province:add_pop(farmer, bolivian, catholic, 5000, 0.1)
    province:add_pop(soldier, bolivian, catholic, 5000, 0.2)
    province:add_pop(craftsmen, bolivian, catholic, 5000, 0.2)
    province:add_pop(bureaucrat, bolivian, catholic, 5000, 0.7)
    province:add_pop(aristocrat, bolivian, catholic, 5000, 0.2)
    province:add_pop(clergymen, bolivian, catholic, 5000, 0.3)
    province:add_pop(laborer, bolivian, catholic, 5000, 0.2)
    province:add_pop(entrepreneur, bolivian, catholic, 500, 0.9)
    province:add_pop(slave, bolivian, catholic, 500, 0.5)
    province:add_nucleus(bolivia)
    province:give_to(bolivia)
    province:add_industry(wheat_farm, bolivia, andes_transport)
end

