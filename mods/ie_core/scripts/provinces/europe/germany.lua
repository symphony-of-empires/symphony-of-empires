--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Middle Europe
-- Country: Germany (and all her minor states)

-- "So how many german minors are you gonna add?"
-- ALL OF THEM

local oldenburg_provinces_list = {
    { ref_name = "oldenburg", name = _("Oldenburg"), color = 0xffeebf },
    { ref_name = "birkenfeld", name = _("Birkenfeld"), color = 0xffa8c6 },
}
for k, v in pairs(oldenburg_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, oldenburg, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(oldenburg)
    province:add_nucleus(germany)
    province:give_to(oldenburg)
    province:set_terrain(plains)
end

local wolfenbuttel_provinces_list = {
    { ref_name = "wolfenbuttel", name = _("Wolfenbüttel"), color = 0x00ff21 },
}
for k, v in pairs(wolfenbuttel_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, wolfenbuttel, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(wolfenbuttel)
    province:add_nucleus(germany)
    province:give_to(wolfenbuttel)
    province:set_terrain(plains)
end

local bremen_provinces_list = {
    { ref_name = "bremen", name = _("Bremen"), color = 0xff054b },
}
for k, v in pairs(bremen_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, bremen, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(bremen)
    province:add_nucleus(germany)
    province:give_to(bremen)
    province:set_terrain(plains)
end

local brunswick_provinces_list = {
    { ref_name = "brunswick", name = _("Brunswick"), color = 0xd8ffd6 },
}
for k, v in pairs(brunswick_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, brunswick, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(brunswick)
    province:add_nucleus(germany)
    province:give_to(brunswick)
    province:set_terrain(plains)
end

local ritzebuttel_provinces_list = {
    { ref_name = "ritzebuttel", name = _("Ritzebüttel"), color = 0x96eaff },
}
for k, v in pairs(ritzebuttel_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, ritzebuttel, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(ritzebuttel)
    province:add_nucleus(germany)
    province:give_to(ritzebuttel)
    province:set_terrain(plains)
end

local grand_duchy_of_hesse_and_by_rhine_provinces_list = {
    { ref_name = "darnustadt", name = _("Darnustadt"), color = 0xffd3ec },
    { ref_name = "giessen", name = _("Giessen"), color = 0x84ff84 },
}
for k, v in pairs(grand_duchy_of_hesse_and_by_rhine_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, grand_duchy_of_hesse_and_by_rhine, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(grand_duchy_of_hesse_and_by_rhine)
    province:add_nucleus(germany)
    province:give_to(grand_duchy_of_hesse_and_by_rhine)
    province:set_terrain(plains)
end


local luxembourg_provinces_list = {
    { ref_name = "luxembourg", name = _("Luxembourg"), color = 0xb056ff },
}
for k, v in pairs(luxembourg_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, luxembourg, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(luxembourg)
    province:give_to(luxembourg)
    province:set_terrain(plains)
end

local prussia_provinces_list = {
    { ref_name = "province_b770ff00", name = _("Unknown"), color = 0xb770fe },
    { ref_name = "province_8aff5900", name = _("Unknown"), color = 0x59ff8a },
    { ref_name = "province_ff000000", name = _("Unknown"), color = 0xff },
    { ref_name = "province_ffd1a000", name = _("Unknown"), color = 0xa0d1ff },
    { ref_name = "province_9dff7f00", name = _("Unknown"), color = 0x7fff9d },
    { ref_name = "province_d3afff00", name = _("Unknown"), color = 0xffafd3 },
    { ref_name = "province_ff6b8d00", name = _("Unknown"), color = 0x8d6bff },
    { ref_name = "province_206cd500", name = _("Unknown"), color = 0xd56c20 },
    { ref_name = "province_e0a91900", name = _("Unknown"), color = 0x19a9e0 },
    { ref_name = "province_6032e100", name = _("Unknown"), color = 0xe13260 },
    { ref_name = "province_c089da00", name = _("Unknown"), color = 0xda89c0 },
    { ref_name = "province_9f23ff00", name = _("Unknown"), color = 0xff239f },
    { ref_name = "province_e2ffc400", name = _("Unknown"), color = 0xc4ffe2 },
    { ref_name = "province_803b5c00", name = _("Unknown"), color = 0x5c3b80 },
    { ref_name = "province_20452c00", name = _("Unknown"), color = 0x2c4520 },
    { ref_name = "province_2216ff00", name = _("Unknown"), color = 0xff1622 },
    { ref_name = "province_a0c07d00", name = _("Unknown"), color = 0x7dc0a0 },
    { ref_name = "province_a049b400", name = _("Unknown"), color = 0xb449a0 },
    { ref_name = "province_ff60e700", name = _("Unknown"), color = 0xe760ff },
    { ref_name = "province_a064af00", name = _("Unknown"), color = 0xaf64a0 },
    { ref_name = "province_c0cc9d00", name = _("Unknown"), color = 0x9dccc0 },
    { ref_name = "province_c6ff1c00", name = _("Unknown"), color = 0x1cffc6 },
    { ref_name = "province_ff70b700", name = _("Unknown"), color = 0xb770ff },
    { ref_name = "province_f6ff7a00", name = _("Unknown"), color = 0x7afff6 },
    { ref_name = "province_87a1ff00", name = _("Unknown"), color = 0xffa187 },
    { ref_name = "province_b5fff600", name = _("Unknown"), color = 0xf6ffb5 },
    { ref_name = "province_ff857200", name = _("Unknown"), color = 0x7285ff },
    { ref_name = "province_ffdebf00", name = _("Unknown"), color = 0xbfdeff },
    { ref_name = "province_60646e00", name = _("Unknown"), color = 0x6e6460 },
    { ref_name = "province_a2ff4c00", name = _("Unknown"), color = 0x4cffa2 },
}
for k, v in pairs(prussia_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, prussia, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(prussia)
    province:add_nucleus(germany)
    province:give_to(prussia)
    province:set_terrain(plains)
end

local bavaria_provinces_list = {
    { ref_name = "province_9fff9900", name = _("Unknown"), color = 0x99ff9f },
    { ref_name = "province_e856ff00", name = _("Unknown"), color = 0xff56e8 },
    { ref_name = "province_d2ff8e00", name = _("Unknown"), color = 0x8effd2 },
    { ref_name = "province_ffc38400", name = _("Unknown"), color = 0x84c3ff },
    { ref_name = "weimar", name = _("Weimar"), color = 0xff4f98 },
    { ref_name = "rhineland_palatinate", name = _("Rhineland-Palatinate"), color = 0xffe242 },
}
for k, v in pairs(bavaria_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, bavaria, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(bavaria)
    province:add_nucleus(germany)
    province:give_to(bavaria)
    province:set_terrain(plains)
end

local belgium_provinces_list = {
    { ref_name = "province_beff7500", name = _("Unknown"), color = 0x75ffbe },
    { ref_name = "province_f4ffa500", name = _("Unknown"), color = 0xa5fff4 },
    { ref_name = "province_ffd6fc00", name = _("Unknown"), color = 0xfcd6ff },
    { ref_name = "province_8d6dff00", name = _("Unknown"), color = 0xff6d8d },
    { ref_name = "province_ff597d00", name = _("Unknown"), color = 0x7d59ff },
    { ref_name = "province_00ff00", name = _("Unknown"), color = 0xff0000 },
    { ref_name = "province_e7ff7f00", name = _("Unknown"), color = 0x7fffe7 },
    { ref_name = "province_ffc26d00", name = _("Unknown"), color = 0x6dc2ff },
    { ref_name = "province_feafc500", name = _("Unknown"), color = 0xc5affe },
}
for k, v in pairs(belgium_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, belgium, andes_transport)
    province:add_pop(artisan, belgian, protestant, 5000, 0.9)
    province:add_pop(farmer, belgian, protestant, 40000, 0.8)
    province:add_pop(soldier, belgian, protestant, 8000, 0.8)
    province:add_pop(craftsmen, belgian, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, belgian, protestant, 5000, 0.9)
    province:add_pop(aristocrat, belgian, protestant, 5000, 0.9)
    province:add_pop(clergymen, belgian, protestant, 5000, 0.9)
    province:add_pop(laborer, belgian, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, belgian, protestant, 800, 0.9)
    province:add_nucleus(belgium)
    province:give_to(belgium)
    province:set_terrain(plains)
end

local hannover_provinces_list = {
    { ref_name = "province_c832ff00", name = _("Unknown"), color = 0xff32c8 },
    { ref_name = "province_20dfad00", name = _("Unknown"), color = 0xaddf20 },
}
for k, v in pairs(hannover_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, hannover, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(hannover)
    province:add_nucleus(germany)
    province:give_to(hannover)
    province:set_terrain(plains)
end

local switzerland_provinces_list = {
    { ref_name = "province_e09f9a00", name = _("Unknown"), color = 0x9a9fe0 },
    { ref_name = "province_60e43500", name = _("Unknown"), color = 0x35e460 },
    { ref_name = "province_605aff00", name = _("Unknown"), color = 0xff5a60 },
    { ref_name = "province_40d99400", name = _("Unknown"), color = 0x94d940 },
    { ref_name = "province_e0bf7d00", name = _("Unknown"), color = 0x7dbfe0 },
    { ref_name = "province_4001bf00", name = _("Unknown"), color = 0xbf0140 },
    { ref_name = "province_a0dead00", name = _("Unknown"), color = 0xaddea0 },
    { ref_name = "province_e0d52e00", name = _("Unknown"), color = 0x2ed5e0 },
    { ref_name = "province_203b1c00", name = _("Unknown"), color = 0x1c3b20 },
}
for k, v in pairs(switzerland_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, switzerland, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(switzerland)
    province:give_to(switzerland)
    province:set_terrain(plains)
end

local baden_provinces_list = {
    { ref_name = "baden", name = _("Baden"), color = 0xff703d },
}
for k, v in pairs(baden_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, baden, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(baden)
    province:add_nucleus(germany)
    province:give_to(baden)
    province:set_terrain(plains)
end

local wurttemberg_provinces_list = {
    { ref_name = "wurttemberg", name = _("Württemberg"), color = 0x9d7ffe },
}
for k, v in pairs(wurttemberg_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, wurttemberg, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(wurttemberg)
    province:add_nucleus(germany)
    province:give_to(wurttemberg)
    province:set_terrain(plains)
end

local mecklenburg_provinces_list = {
    { ref_name = "mecklenburg", name = _("Mecklenburg"), color = 0xff7a59 },
}
for k, v in pairs(mecklenburg_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, mecklenburg, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(mecklenburg)
    province:add_nucleus(germany)
    province:give_to(mecklenburg)
    province:set_terrain(plains)
end

local saxonyg_provinces_list = {
    { ref_name = "dresden", name = _("Dresden"), color = 0x8cb2ff },
}
for k, v in pairs(saxonyg_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(iron_mine, saxony, andes_transport)
    province:add_pop(artisan, german, protestant, 5000, 0.9)
    province:add_pop(farmer, german, protestant, 40000, 0.8)
    province:add_pop(soldier, german, protestant, 8000, 0.8)
    province:add_pop(craftsmen, german, protestant, 5000, 0.9)
    province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
    province:add_pop(aristocrat, german, protestant, 5000, 0.9)
    province:add_pop(clergymen, german, protestant, 5000, 0.9)
    province:add_pop(laborer, german, protestant, 50000, 0.8)
    province:add_pop(entrepreneur, german, protestant, 800, 0.9)
    province:add_nucleus(saxony)
    province:add_nucleus(germany)
    province:give_to(saxony)
    province:set_terrain(plains)
end
saxony:set_capital(Province:get("dresden"))
