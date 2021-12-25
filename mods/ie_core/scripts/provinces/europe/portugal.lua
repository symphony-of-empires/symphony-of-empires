--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Iberia
-- Country: Portugal

local portugal_provinces_list = {
    { ref_name = "porto", name = _("Porto"), color = rgb(174, 77, 161), density = 1.0 },
    { ref_name = "villa_real", name = _("Villa Real"), color = rgb(174, 126, 161), density = 0.5 },
    { ref_name = "aveiro", name = _("Aveiro"), color = rgb(157, 122, 121), density = 0.8 },
    { ref_name = "guarda", name = _("Guarda"), color = rgb(157, 107, 159), density = 0.3 },
    { ref_name = "lisbon", name = _("Lisbon"), color = rgb(134, 84, 159), density = 1.41 },
    { ref_name = "evora", name = _("Évora"), color = rgb(157, 108, 144), density = 0.7 },
    { ref_name = "faro", name = _("Faro"), color = rgb(157, 60, 144), density = 0.3 }
}

for k, v in pairs(portugal_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_pop(artisan, portuguese, catholic, 5000 * v.density, 0.9)
    province:add_pop(farmer, portuguese, catholic, 40000 * v.density, 0.8)
    province:add_pop(soldier, portuguese, catholic, 8000 * v.density, 0.8)
    province:add_pop(craftsmen, portuguese, catholic, 5000 * v.density, 0.9)
    province:add_pop(bureaucrat, portuguese, catholic, 5000 * v.density, 0.9)
    province:add_pop(burgeoise, portuguese, catholic, 5000 * v.density, 0.9)
    province:add_pop(clergymen, portuguese, catholic, 5000 * v.density, 0.9)
    province:add_pop(laborer, portuguese, catholic, 50000 * v.density, 0.8)
    province:add_pop(burgeoise, portuguese, catholic, 800 * v.density, 0.9)
    province:add_nucleus(portugal)
    province:add_nucleus(iberia)
    province:give_to(portugal)
    province:add_industry(wheat_farm, portugal)
    province:set_terrain(plains)
end