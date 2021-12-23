--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Middle Europe
-- Country: Dutch Empire

local dutch_provinces_list = {
   { ref_name = "province_cd4000", name = _("Unknown"), color = 0x40cd00 },
   { ref_name = "province_9effd100", name = _("Unknown"), color = 0xd1ff9e },
   { ref_name = "province_0cff00", name = _("Unknown"), color = 0xff0c00 },
   { ref_name = "province_96ff5100", name = _("Unknown"), color = 0x51ff96 },
   { ref_name = "province_7a97ff00", name = _("Unknown"), color = 0xff977a },
   { ref_name = "province_edffc100", name = _("Unknown"), color = 0xc1ffed },
   { ref_name = "province_96ff9900", name = _("Unknown"), color = 0x99ff96 },
   { ref_name = "province_ff778900", name = _("Unknown"), color = 0x8977ff },
   { ref_name = "province_c5a8ff00", name = _("Unknown"), color = 0xffa8c5 },
   { ref_name = "province_ff7cd300", name = _("Unknown"), color = 0xd37cff },
   { ref_name = "province_dbff9e00", name = _("Unknown"), color = 0x9effdb },
   { ref_name = "province_805aca00", name = _("Unknown"), color = 0xca5a80 },
   { ref_name = "province_cdc700", name = _("Unknown"), color = 0xc7cd00 },
   { ref_name = "province_40bd7b00", name = _("Unknown"), color = 0x7bbd40 },
   { ref_name = "province_a04a5400", name = _("Unknown"), color = 0x544aa0 },
   { ref_name = "province_40ce9b00", name = _("Unknown"), color = 0x9bce40 },
   { ref_name = "province_c0b1ec00", name = _("Unknown"), color = 0xecb1c0 },
   { ref_name = "province_60368a00", name = _("Unknown"), color = 0x8a3660 },
   { ref_name = "province_2abc00", name = _("Unknown"), color = 0xbc2a00 },
   { ref_name = "province_80478000", name = _("Unknown"), color = 0x804780 },
   { ref_name = "province_baa700", name = _("Unknown"), color = 0xa7ba00 },
   { ref_name = "province_808ec100", name = _("Unknown"), color = 0xc18e80 },
   { ref_name = "province_a0e1e500", name = _("Unknown"), color = 0xe5e1a0 },
   { ref_name = "province_c0e7b700", name = _("Unknown"), color = 0xb7e7c0 },
   { ref_name = "province_a0ec5f00", name = _("Unknown"), color = 0x5feca0 },
   { ref_name = "province_0f3700", name = _("Unknown"), color = 0x370f00 },
   { ref_name = "province_60656e00", name = _("Unknown"), color = 0x6e6560 },
   { ref_name = "province_60c7b800", name = _("Unknown"), color = 0xb8c760 },
   { ref_name = "province_a01ee300", name = _("Unknown"), color = 0xe31ea0 },
   { ref_name = "province_e08d4f00", name = _("Unknown"), color = 0x4f8de0 },
   { ref_name = "province_c0f9ff00", name = _("Unknown"), color = 0xfff9c0 },
   { ref_name = "province_40998100", name = _("Unknown"), color = 0x819940 },
   { ref_name = "province_60c35200", name = _("Unknown"), color = 0x52c360 },
   { ref_name = "province_80555500", name = _("Unknown"), color = 0x555580 },
   { ref_name = "province_a0b56d00", name = _("Unknown"), color = 0x6db5a0 },
   { ref_name = "province_80eed600", name = _("Unknown"), color = 0xd6ee80 },
   { ref_name = "province_e38f00", name = _("Unknown"), color = 0x8fe300 },
   { ref_name = "province_a76600", name = _("Unknown"), color = 0x66a700 },
   { ref_name = "province_a00ac300", name = _("Unknown"), color = 0xc30aa0 },
   { ref_name = "province_20868300", name = _("Unknown"), color = 0x838620 },
   { ref_name = "province_e0677500", name = _("Unknown"), color = 0x7567e0 },
   { ref_name = "province_a0fddd00", name = _("Unknown"), color = 0xddfda0 },
   { ref_name = "province_a0c98d00", name = _("Unknown"), color = 0x8dc9a0 },
   { ref_name = "province_40c72700", name = _("Unknown"), color = 0x27c740 },
   { ref_name = "province_66c200", name = _("Unknown"), color = 0xc26600 },
   { ref_name = "province_c03bcf00", name = _("Unknown"), color = 0xcf3bc0 },
   { ref_name = "province_a045bd00", name = _("Unknown"), color = 0xbd45a0 },
   { ref_name = "province_80c3e300", name = _("Unknown"), color = 0xe3c380 },
   { ref_name = "province_765a00", name = _("Unknown"), color = 0x5a7600 },
   { ref_name = "province_a02e7b00", name = _("Unknown"), color = 0x7b2ea0 },
   { ref_name = "province_e058a000", name = _("Unknown"), color = 0xa058e0 },
   { ref_name = "province_fffd8f00", name = _("Unknown"), color = 0xfffd8f },
}

for k, v in pairs(dutch_provinces_list) do
    province = Province:new(v)
    province:register()

    --province:add_industry(wheat_farm, netherlands, andes_transport)
    province:add_pop(artisan, dutch, catholic, 5000, 0.9)
    province:add_pop(farmer, dutch, catholic, 40000, 0.8)
    province:add_pop(soldier, dutch, catholic, 8000, 0.8)
    province:add_pop(craftsmen, dutch, catholic, 5000, 0.9)
    province:add_pop(bureaucrat, dutch, catholic, 5000, 0.9)
    province:add_pop(burgeoise, dutch, catholic, 5000, 0.9)
    province:add_pop(clergymen, dutch, catholic, 5000, 0.9)
    province:add_pop(laborer, dutch, catholic, 50000, 0.8)
    province:add_pop(burgeoise, dutch, catholic, 800, 0.9)
    province:add_nucleus(netherlands)
    province:give_to(netherlands)
    province:set_terrain(plains)
end
