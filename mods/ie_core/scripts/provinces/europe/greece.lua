--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Balkan
-- Country: Greece

-- Turkish part
local turkish_greece_provinces = {
    { ref_name = "attika", name = _("Attika"), color = 0x5842ff },
    { ref_name = "korinthia", name = _("Korinthia"), color = 0xeff400 },
    { ref_name = "thives", name = _("Thives"), color = 0xc58cff },
    { ref_name = "evia", name = _("Evia"), color = 0x3fff62 },
    { ref_name = "valtos", name = _("Valtos"), color = 0xff2b7c },
    { ref_name = "sami", name = _("Sami"), color = 0xff8572 },
    { ref_name = "lefkada", name = _("Lefkada"), color = 0xff7587 },
    { ref_name = "zakynthos", name = _("Zakynthos"), color = 0xeeff56 },
    { ref_name = "cyclades", name = _("Cyclades"), color = 0xef42ff },
    { ref_name = "kalymnos", name = _("Kalymnos"), color = 0xbdff7f },
    { ref_name = "dodecanese", name = _("Dodecanese"), color = 0xff3a5e },
    { ref_name = "samos", name = _("Samos"), color = 0xff5f56 },
    { ref_name = "ikaria", name = _("Ikaria"), color = 0x9763ff },
    { ref_name = "plomari", name = _("Plomari"), color = 0x4cffff },
    { ref_name = "thessaly", name = _("Thessaly"), color = 0xd5ffb7 },
    { ref_name = "skopelos", name = _("Skopelos"), color = 0xff7a83 },
    { ref_name = "drama", name = _("Drama"), color = 0xffad7a },
    { ref_name = "chalkidi", name = _("Chalkidi"), color = 0x554cff },
    { ref_name = "thessalonki", name = _("Thessalonki"), color = 0xfffdce },
    { ref_name = "pieria", name = _("Pieria"), color = 0xcdff87 },
    { ref_name = "grevena", name = _("Grevena"), color = 0xffe0f4 },
    { ref_name = "florina", name = _("Florina"), color = 0x7fffee },
    { ref_name = "dodoni", name = _("Dodoni"), color = 0xad6bff },
    { ref_name = "korfu", name = _("Korfu"), color = 0xf882ff },
    { ref_name = "thasos", name = _("Thasos"), color = 0xff30b6 },
    -- { ref_name = "agio_oros", name = _("Agio Oros"), color = 0x9bd3ff },

    -- Greece patches
    { ref_name = "province_fed39b00", name = _("Province_9bd3fe"), color = 0x9bd3fe },
    { ref_name = "province_ff5b6600", name = _("Province_665bff"), color = 0x665bff },
    { ref_name = "province_ff596100", name = _("Province_6159ff"), color = 0x6159ff },
    { ref_name = "province_ff4f4f00", name = _("Province_4f4fff"), color = 0x4f4fff },
}
for k, v in pairs(turkish_greece_provinces) do
    province = Province:new(v)
    province:register()

    --province:add_industry(wheat_farm, turkey, andes_transport)
    province:add_pop(artisan, greek, christian, 5000, 0.9)
    province:add_pop(farmer, greek, christian, 40000, 0.8)
    province:add_pop(soldier, greek, christian, 8000, 0.8)
    province:add_pop(craftsmen, greek, christian, 5000, 0.9)
    province:add_pop(bureaucrat, greek, christian, 5000, 0.9)
    province:add_pop(aristocrat, greek, christian, 5000, 0.9)
    province:add_pop(clergymen, greek, christian, 5000, 0.9)
    province:add_pop(laborer, greek, christian, 50000, 0.8)
    province:add_pop(entrepreneur, greek, christian, 800, 0.9)
    province:add_nucleus(greece)
    province:give_to(turkey)
end

local greek_greece_provinces = {
    { ref_name = "kythira", name = _("Kythira"), color = 0x7cbbff },
    { ref_name = "lakedemona", name = _("Lakedemona"), color = 0x70ff9d },
    { ref_name = "patras", name = _("Patras"), color = 0xff42c9 },
}
for k, v in pairs(greek_greece_provinces) do
    province = Province:new(v)
    province:register()

    province:add_industry(wheat_farm, turkey, andes_transport)
    province:add_pop(artisan, greek, christian, 5000, 0.9)
    province:add_pop(farmer, greek, christian, 40000, 0.8)
    province:add_pop(soldier, greek, christian, 8000, 0.8)
    province:add_pop(craftsmen, greek, christian, 5000, 0.9)
    province:add_pop(bureaucrat, greek, christian, 5000, 0.9)
    province:add_pop(aristocrat, greek, christian, 5000, 0.9)
    province:add_pop(clergymen, greek, christian, 5000, 0.9)
    province:add_pop(laborer, greek, christian, 50000, 0.8)
    province:add_pop(entrepreneur, greek, christian, 800, 0.9)
    province:add_nucleus(greece)
    province:give_to(turkey)
end
