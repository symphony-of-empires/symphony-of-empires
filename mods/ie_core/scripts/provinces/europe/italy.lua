local two_sicilies_provinces = {
    { ref_name = "province_b1ff8200", name = _("Province_82ffb1"), color = 0x82ffb1 },
    { ref_name = "province_6e5bff00", name = _("Province_ff5b6e"), color = 0xff5b6e },
    { ref_name = "province_b7d5ff00", name = _("Province_ffd5b7"), color = 0xffd5b7 },
    { ref_name = "province_cbff9e00", name = _("Province_9effcb"), color = 0x9effcb },
    { ref_name = "province_ff91c800", name = _("Province_c891ff"), color = 0xc891ff },
    { ref_name = "province_8fff8700", name = _("Province_87ff8f"), color = 0x87ff8f },
    { ref_name = "province_fbafff00", name = _("Province_ffaffb"), color = 0xffaffb },
    { ref_name = "province_77abff00", name = _("Province_ffab77"), color = 0xffab77 },
    { ref_name = "province_d8f7ff00", name = _("Province_fff7d8"), color = 0xfff7d8 },
    { ref_name = "province_a2ff5b00", name = _("Province_5bffa2"), color = 0x5bffa2 },
    { ref_name = "province_4538ff00", name = _("Province_ff3845"), color = 0xff3845 },
    { ref_name = "province_ffa0aa00", name = _("Province_aaa0ff"), color = 0xaaa0ff },
    { ref_name = "province_efffd600", name = _("Province_d6ffef"), color = 0xd6ffef },
    { ref_name = "province_b73aff00", name = _("Province_ff3ab7"), color = 0xff3ab7 },
    { ref_name = "province_ff947c00", name = _("Province_7c94ff"), color = 0x7c94ff },
    { ref_name = "province_cfffbc00", name = _("Province_bcffcf"), color = 0xbcffcf },
    { ref_name = "province_d4b7ff00", name = _("Province_ffb7d4"), color = 0xffb7d4 },
}
for k, v in pairs(two_sicilies_provinces) do
    province = Province:new(v)
    province:register()
    province:add_industry(wheat_farm, two_sicilies, andes_transport)
    province:add_pop(artisan, italian, catholic, 5000, 0.9)
    province:add_pop(farmer, italian, catholic, 40000, 0.8)
    province:add_pop(soldier, italian, catholic, 8000, 0.8)
    province:add_pop(craftsmen, italian, catholic, 5000, 0.9)
    province:add_pop(bureaucrat, italian, catholic, 5000, 0.9)
    province:add_pop(aristocrat, italian, catholic, 5000, 0.9)
    province:add_pop(clergymen, italian, catholic, 5000, 0.9)
    province:add_pop(laborer, italian, catholic, 50000, 0.8)
    province:add_pop(entrepreneur, italian, catholic, 800, 0.9)
    province:add_nucleus(two_sicilies)
    province:add_nucleus(italy)
    province:give_to(two_sicilies)
    province:set_terrain(plains)
end

local papal_states_provinces = {
    { ref_name = "province_9bff9e00", name = _("Province_9eff9b"), color = 0x9eff9b },
    { ref_name = "province_ff7aa200", name = _("Province_a27aff"), color = 0xa27aff },
    { ref_name = "province_5e61ff00", name = _("Province_ff615e"), color = 0xff615e },
    { ref_name = "province_e15eff00", name = _("Province_ff5ee1"), color = 0xff5ee1 },
    { ref_name = "province_dfffb200", name = _("Province_b2ffdf"), color = 0xb2ffdf },
    { ref_name = "province_de9eff00", name = _("Province_ff9ede"), color = 0xff9ede },
    { ref_name = "province_bcffe800", name = _("Province_e8ffbc"), color = 0xe8ffbc }
}
for k, v in pairs(papal_states_provinces) do
    province = Province:new(v)
    province:register()
    province:add_industry(wheat_farm, papal_states, andes_transport)
    province:add_pop(artisan, italian, catholic, 5000, 0.9)
    province:add_pop(farmer, italian, catholic, 40000, 0.8)
    province:add_pop(soldier, italian, catholic, 8000, 0.8)
    province:add_pop(craftsmen, italian, catholic, 5000, 0.9)
    province:add_pop(bureaucrat, italian, catholic, 5000, 0.9)
    province:add_pop(aristocrat, italian, catholic, 5000, 0.9)
    province:add_pop(clergymen, italian, catholic, 5000, 0.9)
    province:add_pop(laborer, italian, catholic, 50000, 0.8)
    province:add_pop(entrepreneur, italian, catholic, 800, 0.9)
    province:add_nucleus(papal_states)
    province:add_nucleus(italy)
    province:give_to(papal_states)
    province:set_terrain(plains)
end

local piedmont_sardinia_provinces = {
    { ref_name = "province_15d300", name = _("Province_d31500"), color = 0xd31500 },
    { ref_name = "province_60f33c00", name = _("Province_3cf360"), color = 0x3cf360 },
    { ref_name = "province_603d8200", name = _("Province_823d60"), color = 0x823d60 },
    { ref_name = "province_2037d800", name = _("Province_d83720"), color = 0xd83720 },
    { ref_name = "province_208dbe00", name = _("Province_be8d20"), color = 0xbe8d20 },
    { ref_name = "province_c04f5e00", name = _("Province_5e4fc0"), color = 0x5e4fc0 },
    { ref_name = "province_6088c700", name = _("Province_c78860"), color = 0xc78860 },
    { ref_name = "province_a0246b00", name = _("Province_6b24a0"), color = 0x6b24a0 },
    { ref_name = "province_c0048800", name = _("Province_8804c0"), color = 0x8804c0 },
    { ref_name = "province_a022b800", name = _("Province_b822a0"), color = 0xb822a0 },
    { ref_name = "province_60a76600", name = _("Province_66a760"), color = 0x66a760 },
    { ref_name = "province_e1a54400", name = _("Province_44a5e1"), color = 0x44a5e1 },
    { ref_name = "haute_savoie", name = _("Haute Savoie"), color = 0xb96b20 },
    { ref_name = "savoie", name = _("Savoie"), color = 0x4e45e0 },
    { ref_name = "alpes_maritimes", name = _("Alpes Maritimes"), color = 0x469360 }
}
for k, v in pairs(piedmont_sardinia_provinces) do
    province = Province:new(v)
    province:register()
    province:add_industry(wheat_farm, piedmont_sardinia, andes_transport)
    province:add_pop(artisan, italian, catholic, 5000, 0.9)
    province:add_pop(farmer, italian, catholic, 40000, 0.8)
    province:add_pop(soldier, italian, catholic, 8000, 0.8)
    province:add_pop(craftsmen, italian, catholic, 5000, 0.9)
    province:add_pop(bureaucrat, italian, catholic, 5000, 0.9)
    province:add_pop(aristocrat, italian, catholic, 5000, 0.9)
    province:add_pop(clergymen, italian, catholic, 5000, 0.9)
    province:add_pop(laborer, italian, catholic, 50000, 0.8)
    province:add_pop(entrepreneur, italian, catholic, 800, 0.9)
    province:add_nucleus(piedmont_sardinia)
    province:add_nucleus(italy)
    province:give_to(piedmont_sardinia)
    province:set_terrain(plains)
end