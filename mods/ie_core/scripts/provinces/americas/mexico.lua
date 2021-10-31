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
    { ref_name = "south_new_mexico", name = _("South New Mexico"), color = rgb(145, 161, 255) }
}

for k, v in pairs(mexico_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_pop(artisan, mexican, catholic, 2000, 0.8)
    province:add_pop(farmer, mexican, catholic, 8000, 0.5)
    province:add_pop(soldier, mexican, catholic, 1000, 0.6)
    province:add_pop(craftsmen, mexican, catholic, 2000, 0.4)
    province:add_pop(bureaucrat, mexican, catholic, 500, 0.6)
    province:add_pop(aristocrat, mexican, catholic, 100, 0.8)
    province:add_pop(clergymen, mexican, catholic, 1000, 0.5)
    province:add_pop(laborer, mexican, catholic, 1000, 0.5)
    province:add_pop(entrepreneur, mexican, catholic, 50, 0.9)
    province:add_nucleus(mexico)
    province:give_to(mexico)
    province:add_industry(wheat_farm, russia, andes_transport)
end