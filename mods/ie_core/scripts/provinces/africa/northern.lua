--------------------------------------------------------------
-- Continent: Africa
-- Subcontinent: Northern Africa
-- Country: Tunis/Tunisia/French North Africa
local tunis_provinces_list = {
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
for k, v in pairs(tunis_provinces_list) do
    province = Province:new(v)
    province:register()
    
    --province:add_industry(wheat_farm, united_states)
    province:add_pop(artisan, arabic, islamic, 10, 0.6)
    province:add_pop(farmer, arabic, islamic, 8000, 0.4)
    province:add_pop(soldier, arabic, islamic, 10, 0.6)
    province:add_pop(craftsmen, arabic, islamic, 10, 0.4)
    province:add_pop(bureaucrat, arabic, islamic, 10, 0.8)
    province:add_pop(burgeoise, arabic, islamic, 25, 0.5)
    province:add_pop(clergymen, arabic, islamic, 10, 0.6)
    province:add_pop(laborer, arabic, islamic, 1000, 0.5)
    province:add_pop(burgeoise, arabic, islamic, 5, 0.9)
    province:add_nucleus(arabia)
    province:add_nucleus(tunis)
    province:give_to(tunis)
end

--------------------------------------------------------------
-- Continent: Africa
-- Subcontinent: Northern Africa
-- Country: Morroco/French North Africa/Spanish North Africa
local morrocco_provinces_list = {
    { ref_name = "province_a1ff3d00", name = _("Province_3dffa1"), color = 0x3dffa1 },
    { ref_name = "province_c093ff00", name = _("Province_ff93c0"), color = 0xff93c0 },
    { ref_name = "province_ffecb500", name = _("Province_b5ecff"), color = 0xb5ecff },
    { ref_name = "province_f8c4ff00", name = _("Province_ffc4f8"), color = 0xffc4f8 },
    { ref_name = "province_787eff00", name = _("Province_ff7e78"), color = 0xff7e78 },
    { ref_name = "province_e2c6ff00", name = _("Province_ffc6e2"), color = 0xffc6e2 },
    { ref_name = "province_ffdcb200", name = _("Province_b2dcff"), color = 0xb2dcff },
    { ref_name = "province_bffbff00", name = _("Province_fffbbf"), color = 0xfffbbf },
    { ref_name = "province_da63ff00", name = _("Province_ff63da"), color = 0xff63da },
    { ref_name = "province_f0ffad00", name = _("Province_adfff0"), color = 0xadfff0 },
    { ref_name = "province_ff91ac00", name = _("Province_ac91ff"), color = 0xac91ff },
    { ref_name = "province_ffd1bf00", name = _("Province_bfd1ff"), color = 0xbfd1ff },
    { ref_name = "province_a3ffbb00", name = _("Province_bbffa3"), color = 0xbbffa3 },
    { ref_name = "province_bbb5ff00", name = _("Province_ffb5bb"), color = 0xffb5bb },
    { ref_name = "province_fff2e800", name = _("Province_e8f2ff"), color = 0xe8f2ff },
    { ref_name = "province_dda3ff00", name = _("Province_ffa3dd"), color = 0xffa3dd },
}
for k, v in pairs(morrocco_provinces_list) do
    province = Province:new(v)
    province:register()
    
    --province:add_industry(wheat_farm, united_states)
    province:add_pop(artisan, arabic, islamic, 10, 0.6)
    province:add_pop(farmer, arabic, islamic, 8000, 0.4)
    province:add_pop(soldier, arabic, islamic, 10, 0.6)
    province:add_pop(craftsmen, arabic, islamic, 10, 0.4)
    province:add_pop(bureaucrat, arabic, islamic, 10, 0.8)
    province:add_pop(burgeoise, arabic, islamic, 25, 0.5)
    province:add_pop(clergymen, arabic, islamic, 10, 0.6)
    province:add_pop(laborer, arabic, islamic, 1000, 0.5)
    province:add_pop(burgeoise, arabic, islamic, 5, 0.9)
    province:add_nucleus(arabia)
    province:add_nucleus(morrocco)
    province:give_to(morrocco)
end
