--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Chile + Patagonian Chile + Ex-Bolivian territory

local chile_owned_provinces_list = {
    -- To be taken from bolivia
    { ref_name = "iquique", name = _("Iquique"), color = 0xFF7856 },
    { ref_name = "calama", name = _("Calama"), color = 0x5157FF },
    { ref_name = "antofagasta", name = _("Antofagasta"), color = 0xF2FFD8 },

    -- Mainland chile (without mapuche's land)
    { ref_name = "copiapo", name = _("Copiapó"), color = 0x9849FF },
    { ref_name = "coquimbo", name = _("Coquimbo"), color = 0x8C4FFF },
    { ref_name = "illapel", name = _("Illapel"), color = 0x49F5FF },
    { ref_name = "santiago", name = _("Santiago"), color = 0xCB23FF },
    { ref_name = "curico", name = _("Curicó"), color = 0xFFE5C1 },
    { ref_name = "chillan", name = _("Chillán"), color = 0x1691FF },
    { ref_name = "concepcion", name = _("Concepción"), color = 0xFF4D3D },
}
for k, v in pairs(chile_owned_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_pop(artisan, chilean, catholic, 2000, 0.8)
    province:add_pop(farmer, chilean, catholic, 8000, 0.5)
    province:add_pop(soldier, chilean, catholic, 1000, 0.6)
    province:add_pop(craftsmen, chilean, catholic, 2000, 0.4)
    province:add_pop(bureaucrat, chilean, catholic, 500, 0.6)
    province:add_pop(burgeoise, chilean, catholic, 100, 0.8)
    province:add_pop(clergymen, chilean, catholic, 1000, 0.5)
    province:add_pop(laborer, chilean, catholic, 1000, 0.5)
    province:add_pop(burgeoise, chilean, catholic, 50, 0.9)

    province:add_nucleus(argentina)
    province:add_nucleus(chile)
    province:give_to(chile)
    province:add_industry(zinc_mine, chile)
    province:add_industry(copper_mine, chile)
    province:add_industry(iron_mine, chile)
end
chile:set_capital(Province:get("santiago"))

-- Mapuche's provinces that chile wants to take
local mapuche_owned_provinces_list = {
    { ref_name = "valdivia", name = _("Valdivia"), color = 0xA2FF00 },
    { ref_name = "osorno", name = _("Osorno"), color = 0xFFE554 },
    { ref_name = "chiloe", name = _("Chiloé"), color = 0xFF00E5 },
    { ref_name = "aysen", name = _("Aysén"), color = 0x0090FF },
    { ref_name = "punta_arenas", name = _("Punta Arenas"), color = 0x3F3B39 },
    { ref_name = "san_gregorio", name = _("San Gregorio"), color = 0xFFD800 },
    { ref_name = "puerto_porvenir", name = _("Puerto Porvenir"), color = 0x0f145d },
    { ref_name = "los_rios", name = _("Los Rios"), color = 0xb6ff50 },
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
    province:add_industry(zinc_mine, Nation:get("mapuche"))
    province:add_industry(copper_mine, Nation:get("mapuche"))
end
