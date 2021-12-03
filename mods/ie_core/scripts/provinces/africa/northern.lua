--------------------------------------------------------------
-- Continent: Africa
-- Subcontinent: Northern Africa
-- Country: Tunis/Tunisia/French North Africa

provinces_list = {
    { ref_name = "nabeul", name = _("Nabeul"), color = 0x7d7140 },
    { ref_name = "monastir", name = _("Monastir"), color = 0x32cd60 },
    { ref_name = "sfax", name = _("Sfax"), color = 0xee1d00 },
    { ref_name = "soghuan", name = _("Soghuan"), color = 0x46aac0 },
    { ref_name = "kasserine", name = _("Kasserine"), color = 0xfb3dff },
    { ref_name = "bizerte", name = _("Bizerte"), color = 0x083bc0 },
    { ref_name = "gabes", name = _("Gabès"), color = 0x7930c0 },
    { ref_name = "tatouine", name = _("Tatouine"), color = 0xa45260 },
    { ref_name = "tozeur", name = _("Tozeur"), color = 0xe27980 },
    { ref_name = "sidi_bouzid", name = _("Sidi Bouzid"), color = 0x39e5c0 },
    { ref_name = "beja", name = _("Béja"), color = 0xa9b7a0 },
    { ref_name = "kairouan", name = _("Kairouan"), color = 0xa772c0 },
    { ref_name = "kebili", name = _("Kebili"), color = 0x9f7e20 },
}

for k, v in pairs(provinces_list) do
    province = Province:new(v)
    province:register()
    
    --province:add_industry(wheat_farm, united_states, andes_transport)
    province:add_pop(artisan, arabic, islamic, 10, 0.6)
    province:add_pop(farmer, arabic, islamic, 8000, 0.4)
    province:add_pop(soldier, arabic, islamic, 10, 0.6)
    province:add_pop(craftsmen, arabic, islamic, 10, 0.4)
    province:add_pop(bureaucrat, arabic, islamic, 10, 0.8)
    province:add_pop(aristocrat, arabic, islamic, 25, 0.5)
    province:add_pop(clergymen, arabic, islamic, 10, 0.6)
    province:add_pop(laborer, arabic, islamic, 1000, 0.5)
    province:add_pop(entrepreneur, arabic, islamic, 5, 0.9)
    province:add_nucleus(arabia)
    province:add_nucleus(tunis)
    province:give_to(tunis)
end