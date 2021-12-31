--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Middle Europe
-- Country: Denmark

local denmark_provinces_list = {
    { ref_name = "syddanmark", name = _("Syddanmark"), color = 0xa3ddff, density = 1.2 },
    { ref_name = "hedensted", name = _("Hedensted"), color = 0xdcb2ff, density = 9.4 },
    { ref_name = "frederiksborg", name = _("Frederiksborg"), color = 0x00d0ff, density = 5.2 },
    { ref_name = "midtjylland", name = _("Midtjylland"), color = 0xc972ff, density = 3.2 },
    { ref_name = "nordjylland", name = _("Nordjylland"), color = 0x7cb7ff, density = 2.0 },
    { ref_name = "odense", name = _("Odense"), color = 0x84eeff, density = 10.0 },
}
for k, v in pairs(denmark_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(wheat_farm, denmark)
    province:add_pop(artisan, danish, protestant, 5000 * v.density, 0.9)
    province:add_pop(farmer, danish, protestant, 40000 * v.density, 0.8)
    province:add_pop(soldier, danish, protestant, 8000 * v.density, 0.8)
    province:add_pop(craftsmen, danish, protestant, 5000 * v.density, 0.9)
    province:add_pop(bureaucrat, danish, protestant, 5000 * v.density, 0.9)
    province:add_pop(burgeoise, danish, protestant, 5000 * v.density, 0.9)
    province:add_pop(clergymen, danish, protestant, 5000 * v.density, 0.9)
    province:add_pop(laborer, danish, protestant, 50000 * v.density, 0.8)
    province:add_pop(burgeoise, danish, protestant, 800 * v.density, 0.9)
    province:add_nucleus(denmark)
    province:add_nucleus(scandinavia)
    province:give_to(denmark)
end
